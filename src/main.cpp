

#include<iostream>
#include<conio.h>
#include <chrono>
#include <iomanip>
#include <cassert>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Vehicle.h"
#include "AddMetadata.h"
#include "HeavyWorker.h"
//using namespace std;

int main(int argc, char** argv)
{

	//	cout << "Application for testing GPS. " << "Version:" << VERSION_MAJOR << "." << VERSION_MINOR << endl;
	unsigned int max_threads = 0;
	try {
		max_threads = std::thread::hardware_concurrency();
		std::cout << max_threads << " concurrent threads are supported.\n";

	}
	catch (std::runtime_error e) {
		std::cerr << "could not determine maximum thread. using 4" << std::endl;
	}
	if (max_threads <= 0)
		max_threads = 4;
	max_threads = max_threads < 100 ? max_threads : 100;
	cv::Mat m1, m2;
	std::ostringstream strs1, strs2, strs3;
	std::vector<std::string> gpsString;
	XsVector gps(3);
	gps[0] = 0;
	gps[1] = 1;
	gps[2] = 2;

	HeavyWorker hw;

	/*time_t tt = t.msTime() / 1000;
	char *time = ctime(&tt);
	std::string timeString(time);
	*/
	string timeString = "timestamp";
	gpsString.push_back(timeString);
	strs1 << gps[0];
	gpsString.push_back(strs1.str());
	strs2 << gps[1];
	gpsString.push_back(strs2.str());
	strs3 << gps[2];
	gpsString.push_back(strs3.str());
	//addXMP("img.jpg", gpsString);

	Vehicle v;
	v.recordGPS();
	



	//Stereo(1);

	while (!_kbhit())
	{
		v.recordCamera(m1, m2);
		v.latestGPSLocation();



		hw.Stereo(1, m1, m2);
		cout << "Running" << endl;
		Sleep(10000);
	}



	cout << "Press [ENTER] to continue." << std::endl; std::cin.get();
	return 0;
}