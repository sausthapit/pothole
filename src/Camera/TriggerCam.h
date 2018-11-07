#pragma once
///////////////////////////////////////////////////////////////////////////////
//
// This example demonstrates how to use a GrabberListener derived
// callback handler object to process events
//
// A class CListener is derived from GrabberListener. It is used to handle callbacks.
// The method CListener::frameReady() simulates a time expensive processing. Therefore,
// the method CListener::frameReady() is not called for every captured buffer. 
// The member CListener::m_pBufferWritten is used to record, which buffers were processed
// by CListener::frameReady().
// After snapImages() returns, the main function will save the buffers that were not
// processed. This sample shows that all buffers have been copied correctly to the
// MembufferCollection, although CListener::frameReady() was not called for every buffer.
//

#define _WIN32_WINNT 0x0500

#include <future>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <iostream>
#include <conio.h>

#include <tisudshl.h>

#include "CmdHelper.h"
#include "Listener.h"

using namespace _DSHOWLIB_NAMESPACE;

// Specify the number of buffers to be used.
#define NUM_BUFFERS 10

extern std::atomic_int num_captured;


class TriggerCam {

public:
	TriggerCam() {};
	TriggerCam(const std::string device_name, const std::string device_properties) :
		listener(nullptr),
		trigger_button(nullptr),
		trigger_switch(nullptr)
	{

		grabber.openDev(device_name);

		if (!grabber.isDevValid()) {
			std::cout << "Camera may not be connected. Ensure both cameras are connected."<<std::endl;
			std::cout << "Press any key to continue";
			std::cin.get();
			exit(1);
			//throw std::runtime_error("Invalid device");
		}
		grabber.loadDeviceStateFromFile(device_properties);
		listener = new CListener();

		loadTrigger();

		// Assign the number of buffers to the cListener object.
		listener->setBufferSize(NUM_BUFFERS);

		// Enable the overlay bitmap to display the frame counter in the live video.
		grabber.getOverlay()->setEnable(false);

		// Register the pListener object for the frame ready and 
		// the overlay callback event.
		grabber.addListener(listener, GrabberListener::eFRAMEREADY);

		// Create a FrameTypeInfoArray data structure describing the allowed color formats.
		FrameTypeInfoArray acceptedTypes = FrameTypeInfoArray::createRGBArray();

		// Create the frame handler sink
		pSink = FrameHandlerSink::create(acceptedTypes, NUM_BUFFERS);

		// enable snap mode (formerly tFrameGrabberMode::eSNAP).
		pSink->setSnapMode(false);

		switchTrigger(true);

		// Apply the sink to the grabber.
		grabber.setSinkType(pSink);

		grabber.startLive(false);				// Start the grabber.
	}

	void loadTrigger()
	{

		auto property_items = grabber.getAvailableVCDProperties();

		if (property_items != 0)
		{
			// Try to find the trigger item
			auto trigger_item = property_items->findItem(VCDID_Trigger);

			if (trigger_item != nullptr) {

				auto software_trigger_element = trigger_item->findElement(VCDElement_SoftwareTrigger);

				if (software_trigger_element != nullptr) {
					software_trigger_element->getInterfacePtr(trigger_button);
				}

				auto trigger_switch_element = trigger_item->findElement(VCDElement_Value);

				if (trigger_switch_element != nullptr) {
					trigger_switch_element->getInterfacePtr(trigger_switch);
				}
			}
		}

	}

	bool switchTrigger(bool val) {

		if (trigger_switch != nullptr) {
			trigger_switch->setSwitch(val);
			return true;
		}
		else {
			return false;
		}
	}

	void trigger() {

		if (trigger_button != nullptr) {

			std::future<void> fut = std::async(std::launch::async, &DShowLib::IVCDButtonProperty::push, trigger_button);
			std::cerr << "Pushing trigger ...";
			std::future_status stat;

			int retry = 0;

			do {
				std::chrono::milliseconds span(100);
				stat = fut.wait_for(span);

				if (stat == std::future_status::timeout) {
					if (retry >= 10) {
						throw(std::runtime_error("Trigger timeout"));
					}
					else {
						std::cerr << "Trigger timeout, retry " << retry << " / 10 \n";
						++retry;
					}
				}

				else {
					std::cerr << '.';
				}
			} while (stat != std::future_status::ready);

			std::cerr << "done" << std::endl;

		}
		else {
			throw(std::runtime_error("Trigger button invalid"));
		}

	}

	void stop()
	{
		if (grabber.isLive()) {
			grabber.stopLive();
		}

		grabber.removeListener(listener);


		while (grabber.isListenerRegistered(listener))
		{
			Sleep(0); // Wait and give pending callbacks a chance to complete.
		}

		delete listener;
	}

private:

	Grabber grabber;
	CListener *listener;
	tIVCDButtonPropertyPtr trigger_button;
	tIVCDSwitchPropertyPtr trigger_switch;
	smart_ptr<FrameHandlerSink> pSink;

};


//
//int main(int argc, char* argv[])
//{
//	LARGE_INTEGER frequency;
//
//	DShowLib::tIVCDButtonPropertyPtr trigger_button_;
//	DShowLib::tIVCDSwitchPropertyPtr trigger_switch_;
//
//	QueryPerformanceFrequency(&frequency);
//
//	DShowLib::InitLibrary();
//
//	atexit( ExitLibrary );
//	
//	TriggerCam cam1("DFK 33UX264 1"), cam2("DFK 33UX264");;
//
//	int kk = 0;
//
//	last_trigger_time.QuadPart = 0;
//	trigger_time.QuadPart = 0;
//	last_frame_time.QuadPart = 0;
//
//	LARGE_INTEGER elapsed_microsecs;
//
//	while (kk < 10) {
//
//		std::unique_lock<std::mutex> lck(m);
//		num_captured.store(0);
//
//		last_trigger_time = trigger_time;
//		QueryPerformanceCounter(&trigger_time);
//
//		
//		cam1.trigger();
//		cam2.trigger();
//
//		while (num_captured.load() != 2) cv.wait(lck);
//
//		num_captured.store(0);
//
//		kk++;
//
//		elapsed_microsecs.QuadPart = frame_time.QuadPart - last_frame_time.QuadPart;
//		elapsed_microsecs.QuadPart *= 1000000;
//		elapsed_microsecs.QuadPart /= frequency.QuadPart;
//
//		std::cerr << "Time from frame to frame: " << elapsed_microsecs.QuadPart << "\n";
//
//	}
//
//	cam1.stop();
//	cam2.stop();
//	
//
//	std::cout << "Press any key to continue!" << std::endl;
//	std::cin.get();
//	return 0;
//}

