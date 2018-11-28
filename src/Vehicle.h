//
// Created by saurav on 10/19/2018.
//
/* Refers to the code related to the vehicle, explicitly the sensors and the communication module.*/

#ifndef POTHOLE_VEHICLE_H
#define POTHOLE_VEHICLE_H
#include<map>
#include <vector>
#include "DeviceContainer.h"
#include "tisudshl.h"
#include "TriggerCam.h"

//#include "PeriodicData.h"
//#include "TriggerCam.h"
using namespace std;
//using namespace cv;
typedef map<XsDevice *, XsPortInfo const *> DeviceInfo;
typedef DeviceInfo::iterator DeviceInfoIterator;

typedef vector<DeviceContainer*> ListOfGPS;
typedef ListOfGPS::iterator GPSIterator;

// Specify the number of buffers to be used.
//#define NUM_BUFFERS_CAM 10
class Vehicle {
private:
//    GPS information
    DeviceInfo gpsDevices;
    ListOfGPS gpsList;
    GPSIterator itr;
    DeviceInfoIterator pos;
    XsControl* control;
	//list<PeriodicData> dataBuffer;
//    Camera information
	//TriggerCam cam1, cam2;
	LARGE_INTEGER frequency;
	TriggerCam camLeft, camRight;
//  Communication to cloud
public:
    Vehicle();
	
	Vehicle(std::string startTime);

	bool initGPS(std:: string);
    bool initCamera();
	XsDataPacket latestGPSLocation();
	std::vector<std::string> latestGPSLocationString();
	void recordCamera(cv::Mat &, cv::Mat &);
	//void recordCamera();
	void recordGPS();

		


};


#endif //POTHOLE_VEHICLE_H
