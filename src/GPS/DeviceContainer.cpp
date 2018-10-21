#include "DeviceContainer.h"



DeviceContainer::DeviceContainer(size_t maxBufferSize = 10) : m_maxNumberOfPacketsInBuffer(maxBufferSize), m_numberOfPacketsInBuffer(0)
{

	//this->m_device->addCallbackHandler(this);

}
DeviceContainer::DeviceContainer(XsDevice* device, const XsPortInfo* portInfo, size_t maxBufferSize = 10) : m_maxNumberOfPacketsInBuffer(maxBufferSize), m_numberOfPacketsInBuffer(0) {
	this->m_device = device;
	this->m_portInfo = portInfo;
	
	/*XsDeviceCallbackHandler m_CallBackHandler(5);
	this->m_device->addCallbackHandler(&m_CallBackHandler);*/
	this->configure();
	

	std::cout << "Device:" + this->m_device->deviceId().toString().toStdString() + " is successfully configured." << std::endl;

}
void DeviceContainer::addCallback()
{
	XsDeviceCallbackHandler* tmp=new XsDeviceCallbackHandler(5);
	m_CallBackHandler = tmp;
	m_device->addCallbackHandler(tmp);
}
XsDataPacket DeviceContainer::popOldestPacket() {
	/*XsMutexLocker lock(m_mutex);
	XsDataPacket oldestPacket(m_packetBuffer.front()); 
	m_packetBuffer.pop_front(); 
	--m_numberOfPacketsInBuffer; 
	return oldestPacket; */
	return m_CallBackHandler->popOldestPacket();
}

bool DeviceContainer::configure()
{
	try {
		m_device->gotoConfig();
	}
	catch (...) {
		throw std::runtime_error("Could not set " +m_device->deviceId().toString().toStdString() +" to configure mode.");
	}
	if (m_device->deviceId().isMt9c() || m_device->deviceId().isLegacyMtig())
	{
		std::cout << "legacy mode" << std::endl;
		XsOutputMode outputMode = XOM_Orientation | XOM_Calibrated | XOM_GpsPvt_Pressure; // output orientation data
		XsOutputSettings outputSettings = XOS_OrientationMode_Quaternion | XOS_CalibratedMode_All | XOS_GpsInGpsPvt; // output orientation data as quaternion
		XsDeviceMode deviceMode(100); // make a device mode with update rate: 100 Hz
		deviceMode.setModeFlag(outputMode);
		deviceMode.setSettingsFlag(outputSettings);

		// set the device configuration
		if (!m_device->setDeviceMode(deviceMode))
		{
			throw std::runtime_error("Could not configure MTmki device. Aborting.");
		}
	}
	else if (m_device->deviceId().isMtMk4() || m_device->deviceId().isFmt_X000())
	{
		/*				std::cout << "non legacy device" << std::endl;
		XsOutputMode outputMode = XOM_Orientation | XOM_Calibrated | XOM_GpsPvt_Pressure; // output orientation data
		XsOutputSettings outputSettings = XOS_OrientationMode_Quaternion | XOS_CalibratedMode_All | XOS_GpsInGpsPvt ; // output orientation data$
		XsDeviceMode deviceMode(100); // make a device mode with update rate: 100 Hz
		deviceMode.setModeFlag(outputMode);
		deviceMode.setSettingsFlag(outputSettings);*/

		XsOutputConfiguration quat(XDI_Quaternion, 0);
		XsOutputConfiguration acc(XDI_Acceleration, 0);
		XsOutputConfiguration avel(XDI_RateOfTurn, 0);
		XsOutputConfiguration gps(XDI_LatLon | XDI_SubFormatDouble, 0);
		XsOutputConfiguration alt(XDI_AltitudeEllipsoid, 0);
		XsOutputConfigurationArray configArray;
		configArray.push_back(quat);
		configArray.push_back(acc);
		configArray.push_back(avel);
		configArray.push_back(gps);
		configArray.push_back(alt);
		if (!m_device->setOutputConfiguration(configArray))
			//				if(!device->setDeviceMode(deviceMode))
		{

			throw std::runtime_error("Could not configure MTmk4 device. Aborting.");
		}
	}
	else
	{
		throw std::runtime_error("Unknown device while configuring. Aborting.");
	}
	return true;
}

bool DeviceContainer::gotoMeasurement()
{
	if (!m_device->gotoMeasurement())
	{
		throw std::runtime_error("Could not put device into measurement mode. Aborting.");
	}
	return true;
}
bool DeviceContainer::packetAvailable() const
{
	
		//XsMutexLocker lock(m_mutex);
		return m_CallBackHandler->numberOfPacketsInBuffer()>0;
	
}
void DeviceContainer::operator()()
{
	std::cout << "from thread" << std::endl;
}
DeviceContainer::~DeviceContainer()
{
}
 /*void DeviceContainer::onLiveDataAvailable(XsDevice*, const XsDataPacket* packet) {
	XsMutexLocker lock(m_mutex);
	assert(packet != 0);
	while (m_numberOfPacketsInBuffer >= m_maxNumberOfPacketsInBuffer)
	{
		(void)popOldestPacket();
	}
	m_packetBuffer.push_back(*packet);
	++m_numberOfPacketsInBuffer;
	assert(m_numberOfPacketsInBuffer <= m_maxNumberOfPacketsInBuffer);
}*/