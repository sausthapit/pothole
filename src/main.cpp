

#include<iostream>
#include<conio.h>
#include <chrono>
#include "Vehicle.h"
#include <iomanip>
#include <cassert>


#include "AddMetadata.h"
//using namespace std;

//#include "xsdevicedetector.h"

//uint64 a;
int main(int argc, char** argv)
{

	//	cout << "Application for testing GPS. " << "Version:" << VERSION_MAJOR << "." << VERSION_MINOR << endl;

	bool realData = true;
	//cv::Mat m1, m2;
	//addXMP();
	/*if (realData)
	{
		Vehicle v;
		v.recordGPS();
		v.recordCamera(m1,m2);
		v.latestGPSLocation();
	}
	else {
		
		XsDataPacket p3;
		
		
	}
	Stereo(1);*/

	while (!_kbhit())
				{
		cout << "Running"<<endl;
		//Sleep(10000);
	}



	cout << "Press [ENTER] to continue." << std::endl; std::cin.get();
	return 0;
}