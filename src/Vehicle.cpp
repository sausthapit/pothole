//
// Created by saurav on 10/19/2018.
//

#include "Vehicle.h"
#define _WIN32_WINNT 0x0500
#include <iostream>
#include <conio.h>

#include <tisudshl.h>

#include "CmdHelper.h"
#include "Listener.h"
using namespace _DSHOWLIB_NAMESPACE;
Vehicle::Vehicle(){
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
            gpsDevices.insert(DeviceInfo::value_type(detectedDevice, &portInfo));
            DeviceContainer dc = DeviceContainer(detectedDevice, &portInfo, 10);
            gpsList.push_back(&dc);
            cout << "All the devices detected are configured" << endl;
        }
        for (itr = gpsList.begin(); itr != gpsList.end(); itr++) {
            XsDeviceCallbackHandler m_CallBackHandler(5);
            (*itr)->addCallback();
            (*itr)->gotoMeasurement();

            cout << "Device set for measurement." << endl;
        }
    }catch(runtime_error err){
        cout<<"Cannot start GPS"<<endl;
        return false;
    }
    return true;
}
bool Vehicle::initCamera(){
		DShowLib::InitLibrary();
	
		atexit( ExitLibrary );
	
		Grabber grabber;
	
		// Create the GrabberListener object.
		// CListener is derived from GrabberListener.
		CListener *pListener = new CListener();

		if (!setupDeviceFromFile(grabber))
		{
			return -1;
		}

		// Assign the number of buffers to the cListener object.
		pListener->setBufferSize(NUM_BUFFERS_CAM);

		// Enable the ovrlay bitmap to display the frame counter in the live video.
		grabber.getOverlay()->setEnable(true);

		// Register the pListener object for the frame ready and 
		// the overlay callback event.
		grabber.addListener(pListener, GrabberListener::eFRAMEREADY |
			GrabberListener::eOVERLAYCALLBACK);


		// Create a FrameTypeInfoArray data structure describing the allowed color formats.
		FrameTypeInfoArray acceptedTypes = FrameTypeInfoArray::createRGBArray();

		// Create the frame handler sink
		smart_ptr<FrameHandlerSink> pSink = FrameHandlerSink::create(acceptedTypes, NUM_BUFFERS_CAM);

		// enable snap mode (formerly tFrameGrabberMode::eSNAP).
		pSink->setSnapMode(true);

		// Apply the sink to the grabber.
		grabber.setSinkType(pSink);

		grabber.startLive();				// Start the grabber.

		pSink->snapImages(NUM_BUFFERS_CAM);	// Grab NUM_BUFFERS images.

		grabber.stopLive();					// Stop the grabber.

											// Save the buffers for which CListener::frameReady() has not been called.
											// Since CListener::frameReady() calls Sleep(250), it cannot be called for
											// every buffer. Nevertheless, all buffers are copied to the MemBufferCollection.
		for (size_t i = 0; i < pListener->m_BufferWritten.size(); i++)
		{
			if (!pListener->m_BufferWritten[i])
			{
				std::cout << "Buffer " << i << " processed in main()." << std::endl;
				pListener->saveImage(pSink->getMemBufferCollection()->getBuffer(i), i);
			}
		}
		// The CListener object must be unregistered for all events
		// before it may be destroyed.
		grabber.removeListener(pListener);

		// Now, it must be checked whether the CListener object has been unregistered
		// for all events.
		while (grabber.isListenerRegistered(pListener))
		{
			Sleep(0); // Wait and give pending callbacks a chance to complete.
		}

		// Now, the application can be sure that no callback methods of pListener
		// will be called anymore. It is now safe to delete pListener.
		delete pListener;

		std::cout << "Press any key to continue!" << std::endl;
		std::cin.get();
	return true;
}