#pragma once
#include <xsensdeviceapi.h>
#include <iostream>
#include <list>

#include "xsdevicecallbackhandler.h"
#include<thread>



class DeviceContainer
{
private:
	//XsDevice * m_device;
	const XsPortInfo* m_portInfo;
	mutable XsMutex m_mutex;
	string fname;
	size_t m_maxNumberOfPacketsInBuffer;
	size_t m_numberOfPacketsInBuffer;
	std::list<XsDataPacket> m_packetBuffer;
protected:
	
	//virtual void onLiveDataAvailable(XsDevice*, const XsDataPacket* packet);
public:
	XsDevice * m_device;
	XsDeviceCallbackHandler* m_CallBackHandler;
	DeviceContainer(size_t);
	~DeviceContainer();
	DeviceContainer(XsDevice* device, const XsPortInfo* portInfo, size_t);
	void addCallback();
	XsDataPacket popOldestPacket();
	bool configure();
	bool gotoMeasurement();
	bool packetAvailable() const;
	void operator()();
	
};

