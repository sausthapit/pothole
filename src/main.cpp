

#include<iostream>
#include<conio.h>
#include <chrono>
#include "Vehicle.h"

using namespace std;

//#include "xsdevicedetector.h"


int main(int argc, char** argv)
{

//	cout << "Application for testing GPS. " << "Version:" << VERSION_MAJOR << "." << VERSION_MINOR << endl;





//	XsString fName = "gps.txt";
//	XsFile f;
//
//	f.createText(fName, TRUE);
//
//
//	//XsDeviceCallbackHandler m_CallBackHandler(5);
//	//detectedDevice->addCallbackHandler(&m_CallBackHandler);
//	//detectedDevice->gotoMeasurement();
//
//	//while (!_kbhit())
//	//{
//	//	if (m_CallBackHandler.numberOfPacketsInBuffer() > 0)
//	//	{
//	//		//int64_t stamp = std::chrono::duration_cast<std::chrono::duration<int64_t, std::micro>>(std::chrono::steady_clock::now().time_since_epoch()).count();
//	//		// Retrieve a packet
//	//		XsDataPacket packet = m_CallBackHandler.popOldestPacket();
//
//	//		// Get the quaternion data
//	//		XsQuaternion quaternion = packet.orientationQuaternion();
//	//		XsCalibratedData cal = packet.calibratedData();
//	//		XsVector gps = packet.positionLLA();
//	//		cout << gps[0] << "," << gps[1] << "," << gps[2] << ";" << std::endl;
//	//	}
//	//}
//
//
//	for (itr = gpsList.begin(); itr != gpsList.end(); itr++) {
//
//		while (!_kbhit())
//		{
//			if ((*itr)->packetAvailable())
//			{
//				int64_t stamp = std::chrono::duration_cast<std::chrono::duration<int64_t, std::micro>>(std::chrono::steady_clock::now().time_since_epoch()).count();
//				// Retrieve a packet
//				XsDataPacket packet = (*itr)->popOldestPacket();
//
//				// Get the quaternion data
//				XsQuaternion quaternion = packet.orientationQuaternion();
//				XsCalibratedData cal = packet.calibratedData();
//				XsVector gps = packet.positionLLA();
//				cout << gps[0] << "," << gps[1] << "," << gps[2] << ";" << std::endl;
//			}
//		}
//	}
//	/*for (DeviceContainer *gps : gpsList) {
//	gps->gotoMeasurement();
//
//	}*/
    Vehicle v;

	cout << "Press [ENTER] to continue." << std::endl; std::cin.get();
	return 0;
}