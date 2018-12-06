

#include<iostream>
#include<conio.h>
#include<string>
#include <chrono>
#include <iomanip>
#include <cassert>
#include <experimental\filesystem>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Vehicle.h"
#include "AddMetadata.h"
#include "HeavyWorker.h"

#include "util.h"


#define MULTITHREAD 1

#ifdef MULTITHREAD
#include "ctpl_stl.h"
#endif // MULTITHREAD

std::vector<std::pair<string, string>> options;

std::string simulationFolder; //This folder is used as input if simulation is selected in the menu.
std::string baseUrl; // = "http://localhost:8080/Pothole_server_war_exploded/upload/";
char startTime[40]; // Application start time. Used for creating folder names.
double fps;// = 10;


int totalSimulationImages;

std::string modelTxt, modelBin, leftCameraSettingsFile, rightCameraSettingsFile;


//extern cv::String modelTxt;
//extern cv::String modelBin;
ofstream gpsFile;
std::string resultFolder; // This is where the images and GPS data is stored.

void uploadImage(const char *, std::string);



std::vector<std::string> writeData(XsDataPacket packet) {

	std::vector<std::string> gpsString;
	std::vector < std::double_t> gpsDouble;
	XsVector gps = packet.positionLLA();
	XsTimeStamp t = packet.m_toa;
	int64_t stamp = std::chrono::duration_cast<std::chrono::duration<int64_t, std::micro>>(std::chrono::steady_clock::now().time_since_epoch()).count();
	time_t tt = t.msTime() / 1000;
	//cout<<ctime(&tt)<<endl;
	gpsFile << t.msTime() << "," << stamp << "," << gps[0] << "," << gps[1] << "," << gps[2] << ";" << std::endl;

	gpsDouble.push_back(t.msTime());
	gpsDouble.push_back(stamp);
	gpsDouble.push_back(gps[0]);
	gpsDouble.push_back(gps[1]);
	gpsDouble.push_back(gps[2]);

	toString(std::begin(gpsDouble), std::end(gpsDouble), std::back_inserter(gpsString));
	return gpsString;
	//<< "," << gps[2] << ";" << std::endl;
}
void runRealtime(HeavyWorker *hw, cv::Mat leftMat, cv::Mat rightMat, std::vector<std::string> gpsString, int i)
{
	try {
		std::vector<std::pair<cv::Mat, float>> result = hw->Stereo(leftMat, rightMat);



		if (result.size() > 0)
		{
			std::cout << "There are potholes in this image. Sending them to cloud for storage." << std::endl;
			int count = 0;
			for (auto potholePatch : result)
			{
				std::ostringstream filename, ss, fileNameAndPath;
				cout << potholePatch.second;
				count = count + 1;
				filename << "potholePatch_" << i << "_" << count << ".png";
				fileNameAndPath << resultFolder << filename.str();
				cv::imwrite(fileNameAndPath.str(), potholePatch.first);
				ss << potholePatch.second;
				gpsString.push_back(ss.str());
				addXMP(fileNameAndPath.str(), gpsString);

				uploadImage(fileNameAndPath.str().c_str(), filename.str());
			}

		}
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}

}

void enumerateOptions(std::map<string, string> options)
{
	baseUrl = options.at("baseUrl");
	fps = stod(options.at("fps"));
	simulationFolder = options.at("simulationFolder");
	totalSimulationImages = stoi(options.at("totalSimulationImages"));
	/*modelTxt = cv::String(options.at("modelTxt"));
	modelBin = cv::String(options.at("modelBin"));*/
	modelTxt = options.at("modelTxt");
	modelBin = (options.at("modelBin"));
	leftCameraSettingsFile = options.at("leftCameraSettingsFile");
	rightCameraSettingsFile = options.at("rightCameraSettingsFile");

}

std::map<string, string> parseConfigurationFile(std::string configFile="config.txt")
{
	
	std::map<string, string> options;
	std::string line;
	ifstream myfile;
	myfile.open(configFile);
	while (std::getline(myfile, line))
	{
		std::istringstream is_line(line);
		std::string key;
		if (std::getline(is_line, key, '='))
		{
			std::string value;
			if (std::getline(is_line, value))

				options.insert(pair<string, string>(key, value));
		}
	}
	return options;
}

int main(int argc, char** argv)
{
	if (argc == 2)
	{
		std::ostringstream os;
		os << argv[1];

		enumerateOptions(parseConfigurationFile(os.str()));
	}
	else {
		enumerateOptions(parseConfigurationFile());
	}

	setOptions();

	time_t now = time(0);
	tm *ltm = localtime(&now);

	strftime(startTime, sizeof(startTime), "%F_%H_%M", ltm);

	std::ostringstream os;
	int len = 6;
	char *randomStr = NULL;
	randomStr = (char *)malloc(len * sizeof(char));
	gen_random(randomStr, len);

	os << "logs/log_" << randomStr << "_" << startTime;
	resultFolder = os.str();

	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);


	HeavyWorker hw( modelTxt, modelBin);
	
	if (run_sim)
	{
		//This is very naive approach to run pothole recognition on already gathered images in the simulationFolder. The simulationFolder should contain one csv file containing the GPS
		//log. Each line refers to a GPS sample and camera index. So line 1 = GPS of image 1 (left_1.png and right_1.png). 
		if (simulationFolder == ""|| totalSimulationImages==NULL)
		{
			std::cout << "Simulation selected but folder not specified. Please run again with folder as argument." << std::endl;
			std::cout << "Specify simulationFolder and totalSimulationImages in config.txt" << std::endl;
			std::cout << "Press return to continue.";

			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			std::cin.get();
			
			exit(0);
		}
		
		std::cout << "Running detection on images 1 to " << totalSimulationImages << " from " << simulationFolder << " folder." << std::endl;
		//std::string simulationFolder = "C:\\Users\\saurav\\Desktop\\gtagged\\images";

		cv::Mat leftMat, rightMat;
		//HeavyWorker hw;
		std::string gpsFile = simulationFolder + "/gpsFile.csv";
		
		vector<vector<double>> data = parse2DCsvFile(simulationFolder + "\\gpsFile.csv");
		cout << "GPS log found at " << gpsFile << std::endl;
		for (int i = 1; i <= totalSimulationImages; i++)
		{
			std::ostringstream osLeft, osRight, lfilename, ss;
			lfilename << "left_" << i << ".png";
			osLeft << simulationFolder << "/" << lfilename.str();
			osRight << simulationFolder << "/right_" << i << ".png";

			leftMat = cv::imread(osLeft.str());
			rightMat = cv::imread(osRight.str());


			vector<double> gpsDouble = data[i];
			std::vector<std::string> gpsString;
			gpsDouble.erase(gpsDouble.begin() + 1);
			toString(std::begin(gpsDouble), std::end(gpsDouble), std::back_inserter(gpsString));

			runRealtime(&hw, leftMat, rightMat, gpsString, i);
			/*gpsString.push_back("56");
			assert(gpsString.size() == 5);
			addXMP(osLeft.str(), gpsString);
			uploadImage(osLeft.str().c_str(), lfilename.str());*/
			std::cout << "Finished processing " << i << " image" << std::endl;
		}
	}
	else {
		namespace fs = std::experimental::filesystem;// this is c++ 14 feature
		
		
		try {
			fs::create_directories("" + resultFolder);
		}
		catch (std::exception& e) { // Not using fs::filesystem_error since std::bad_alloc can throw too.
			std::cout << e.what() << std::endl;
		}

		gpsFile = ofstream(resultFolder + "/gpsFile.csv");
		cv::Mat m1, m2;

		Vehicle v(startTime);
		v.recordGPS();
		
		double nanoSecondsBetweenFrames = (1 / fps) * 1000000000;
		int count = 0;
		while (!_kbhit())
		{
			count++;
			std::ostringstream leftStream, rightStream;

			auto start = std::chrono::high_resolution_clock::now();

			v.recordCamera(m1, m2);

			XsDataPacket gpsPacket = v.latestGPSLocation();
			leftStream << os.str() << "/left_" << count << ".png";
			rightStream << os.str() << "/right_" << count << ".png";
			//string rightImage = "right_" count<< ".png";
			cv::imwrite(leftStream.str(), m1);
			cv::imwrite(rightStream.str(), m2);

			std::vector<std::string>  gpsString = writeData(gpsPacket);


			if (run_realtime)
			{
				runRealtime(&hw, m1, m2, gpsString, count);
			}
			auto finish = std::chrono::high_resolution_clock::now();

			double timeElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();

			double sleepTimeMicro = (nanoSecondsBetweenFrames - timeElapsed) / 1000;
			if (sleepTimeMicro<0) {
				std::cout << "Overran by" << sleepTimeMicro  << " micro seconds" << endl;
				//std::cout << "Consider decreasing FPS.";
				sleepTimeMicro = 0;
			}
			cout << "Running" << endl;
			Sleep(sleepTimeMicro);
		}
	}




	//	cout << "Application for testing GPS. " << "Version:" << VERSION_MAJOR << "." << VERSION_MINOR << endl;
	unsigned int max_threads = 1;
#ifdef MULTITHREAD

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
	ctpl::thread_pool p(max_threads /* two threads in the pool */);

#endif // MULTITHREAD




	cout << "Press [ENTER] to continue." << std::endl; std::cin.get();
	curl_global_cleanup();
	return 0;
}

