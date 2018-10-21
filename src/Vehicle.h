//
// Created by saurav on 10/19/2018.
//
/* Refers to the code related to the vehicle, explicitly the sensors and the communication module.*/

#ifndef POTHOLE_VEHICLE_H
#define POTHOLE_VEHICLE_H
#include<map>
#include <vector>
#include "DeviceContainer.h"

using namespace std;
typedef map<XsDevice *, XsPortInfo const *> DeviceInfo;
typedef DeviceInfo::iterator DeviceInfoIterator;

typedef vector<DeviceContainer*> ListOfGPS;
typedef ListOfGPS::iterator GPSIterator;

// Specify the number of buffers to be used.
#define NUM_BUFFERS_CAM 10
class Vehicle {
private:
//    GPS information
    DeviceInfo gpsDevices;
    ListOfGPS gpsList;
    GPSIterator itr;
    DeviceInfoIterator pos;
    XsControl* control;

//    Camera information

//  Communication to cloud
public:
    Vehicle();
    bool initGPS();
    bool initCamera();


};


#endif //POTHOLE_VEHICLE_H
