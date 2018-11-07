//
// Created by saurav on 10/19/2018.
//

#include "Vehicle.h"




Vehicle::Vehicle() {
	initGPS();
	initCamera();
}
bool Vehicle::initGPS() {
	try {
		control = XsControl::construct();
		assert(control != 0);
		cout << "XsControl object constructed" << std::endl;
		// Scan for connected devices
		std::cout << "Scanning for devices..." << std::endl;
		XsPortInfoArray portInfoArray = XsScanner::scanPorts();
		size_t deviceCounter = 0;
		XsDevice *detectedDevice;
		for (XsPortInfoArray::const_iterator portInfoIter = portInfoArray.begin();
			portInfoIter != portInfoArray.end(); ++portInfoIter) {
			++deviceCounter;
			if (!control->openPort(portInfoIter->portName(), portInfoIter->baudrate())) {
				throw runtime_error("Error opening port " + portInfoIter->portName().toStdString());
			}
			XsPortInfo const portInfo = *portInfoIter;
			detectedDevice = control->device(portInfoIter->deviceId());
			//gpsDevices.insert(DeviceInfo::value_type(detectedDevice, &portInfo));
			DeviceContainer *dc = new DeviceContainer(detectedDevice, &portInfo, 10);
			gpsList.push_back(dc);
			cout << "All the devices detected are configured" << endl;
		}
		for (itr = gpsList.begin(); itr != gpsList.end(); itr++) {
			//XsDeviceCallbackHandler m_CallBackHandler(5);
			(*itr)->addCallback();
			//(*itr)->gotoMeasurement();

			cout << "Device set for measurement." << endl;
		}				
	}
	catch (runtime_error err) {
		cout << "Cannot start GPS" << endl;
		return false;
	}
	return true;
}
bool Vehicle::initCamera() {
	
	DShowLib::tIVCDButtonPropertyPtr trigger_button_;
	DShowLib::tIVCDSwitchPropertyPtr trigger_switch_;

	QueryPerformanceFrequency(&frequency);

	DShowLib::InitLibrary();

	atexit(ExitLibrary);
	
	camLeft.initCam("DFK 33UX264", "device_left.xml");
	camRight.initCam("DFK 33UX264 1", "device_right.xml");
	
	return true;
	

	


}
XsDataPacket Vehicle::latestGPSLocation() {
	return this->gpsList[0]->m_CallBackHandler->latestGPS;
}
void Vehicle::recordCamera(cv::Mat &imgLeft, cv::Mat &imgRight)
{
	int kk = 0;

	last_trigger_time.QuadPart = 0;
	trigger_time.QuadPart = 0;
	last_frame_time.QuadPart = 0;
	
	LARGE_INTEGER elapsed_microsecs;
	//while (kk < 10) {
		//while(true)
		{
		std::unique_lock<std::mutex> lck(m);
		num_captured.store(0);

		last_trigger_time = trigger_time;
		QueryPerformanceCounter(&trigger_time);


		camLeft.trigger();
		camRight.trigger();
		

		while (num_captured.load() != 2) conditionalVariable.wait(lck);

		imgLeft = this->camLeft.getImage();
		imgRight = this->camRight.getImage();
		
		num_captured.store(0);

		kk++;

		elapsed_microsecs.QuadPart = frame_time.QuadPart - last_frame_time.QuadPart;
		elapsed_microsecs.QuadPart *= 1000000;
		elapsed_microsecs.QuadPart /= frequency.QuadPart;

		std::cerr << "Time from frame to frame: " << elapsed_microsecs.QuadPart << "\n";

	}

	//camLeft.stop();
	//camRight.stop();
	/*std::cout << "Press any key to continue!" << std::endl;
	std::cin.get();*/


	//return true;
}
void Vehicle::recordGPS()
{
	try {
		for (itr = gpsList.begin(); itr != gpsList.end(); itr++) {
			(*itr)->gotoMeasurement();

		}
	}
	catch (runtime_error e) {
		std::cerr << "Could not set the device for measurement" << endl;
	}
	cout << "GPS set for measurement" << endl;
}
