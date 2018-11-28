

#include<iostream>
#include<conio.h>
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
#include <curl/curl.h>


#define MULTITHREAD 1

#ifdef MULTITHREAD
#include "ctpl_stl.h"
#endif // MULTITHREAD
void uploadImage(const char *file);
//using namespace std;
//static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
//{
//	size_t retcode;
//	curl_off_t nread;
//
//	/* in real-world cases, this would probably get this data differently
//	as this fread() stuff is exactly what the library already would do
//	by default internally */
//	retcode = fread(ptr, size, nmemb, stream);
//
//	nread = (curl_off_t)retcode;
//
//	fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
//		" bytes from file\n", nread);
//
//	return retcode;
//}
bool log_data = false;
bool run_realtime = false;
bool run_sim = false;

CURL *curl;
CURLcode res;
FILE * hd_src;
struct stat file_info;
curl_mime *mime;
curl_mimepart *part;
char *file;
char *url;
ofstream gpsFile;
std::string resultFolder;
/**
* Reads csv file into table, exported as a vector of vector of doubles.
* @param inputFileName input file name (full path).
* @return data as vector of vector of doubles.
*/
vector<vector<double>> parse2DCsvFile(string inputFileName) {

	vector<vector<double> > data;
	ifstream inputFile(inputFileName);
	int l = 0;

	while (inputFile) {
		l++;
		string s;
		if (!getline(inputFile, s)) break;
		if (s[0] != '#') {
			istringstream ss(s);
			vector<double> record;

			while (ss) {
				string line;
				if (!getline(ss, line, ','))
					break;
				try {
					record.push_back(stof(line));
				}
				catch (const std::invalid_argument e) {
					cout << "NaN found in file " << inputFileName << " line " << l
						<< endl;
					e.what();
				}
			}

			data.push_back(record);
		}
	}

	if (!inputFile.eof()) {
		cerr << "Could not read file " << inputFileName << "\n";
		/*__throw_invalid_argument("File not found.");*/
	}

	return data;
}
template<class IteratorIn, class IteratorOut>
void toString(IteratorIn first, IteratorIn last, IteratorOut out)
{
	std::transform(first, last, out, [](auto d) { return std::to_string(d); });
}
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
			std::cout << "There are potholes in this image. Sending them to cloud for storage" << std::endl;
			int count = 0;
			for (auto potholePatch : result)
			{
				std::ostringstream filename, ss;
				cout << potholePatch.second;
				count = count + 1;
				filename << resultFolder << "potholePatch_" << i << "_" << count << ".png";
				cv::imwrite(filename.str(), potholePatch.first);
				ss << potholePatch.second;
				gpsString.push_back(ss.str());
				addXMP(filename.str(), gpsString);
				
				uploadImage(filename.str().c_str());
			}

		}
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}

}
int main(int argc, char** argv)
{
	//Get a starting time.
	std::string simulationFolder;
	
	time_t now = time(0);
	tm *ltm = localtime(&now);
	char startTime[40];
	strftime(startTime, sizeof(startTime), "%F_%H_%M", ltm);
	HeavyWorker hw;
	std::cout << "Pothole Detection and Localisation tool" << std::endl;
	std::cout << "======================================= " << std::endl;
	std::cout << "Please select one of the option below" << std::endl;
	std::cout << "======================================= " << std::endl;
	std::cout << "[1] Collect Data Only" << std::endl;
	//std::cout << "[2] Run realtime " << std::endl;
	std::cout << "[2] Run realtime and collect data" << std::endl;
	std::cout << "[3] Run realtime on previously collected data" << std::endl;
	std::cout << "[4] Exit" << std::endl;



	char option = std::cin.get();
	if (option == '1') {
		std::cout << "Only logging data." << std::endl;
		log_data = true;
	}
	//else if (option == '2') {
	//	std::cout << "Detecting pothole realtime" << std::endl;
	//	run_realtime = true;
	//}
	else if (option == '2') {
		std::cout << "Running logger and detector" << std::endl;
		log_data = true;
		run_realtime = true;
	}
	else if (option == '3') {
		if (argc ==2)
		{
			simulationFolder = argv[1];
		}
		else {
			std::cout << "Please specify the folder as argument.";
			exit(1);
			
		}	
		std::cout << "Running detector on previously captured data." << std::endl;
		log_data = false;
		run_realtime = false;
		run_sim = true;
		
	}
	else {
		std::cout << "Exiting" << std::endl;
		return 0;
	}

	if (run_sim)
	{
		//This is very naive approach to run pothole recognition on already gathered images in the simulationFolder. The simulationFolder should contain one csv file containing the GPS
		//log. Each line refers to a GPS sample and camera index. So line 1 = GPS of image 1 (left_1.png and right_1.png). 
		int totalImages = 1;
		//std::string simulationFolder = "C:\\Users\\saurav\\Desktop\\gtagged\\images";

		cv::Mat leftMat, rightMat;
		//HeavyWorker hw;
		vector<vector<double>> data = parse2DCsvFile("C:\\dev\\PotholeProject\\build_vs2015\\logs\\log_2018-11-26_17_41\\gpsFile.csv");
		for (int i = 1; i <= totalImages; i++)
		{
			std::ostringstream osLeft, osRight, filename, ss;
			osLeft << simulationFolder << "/left_" << i << ".jpg";
			osRight << simulationFolder << "/right_" << i << ".jpg";

			leftMat = cv::imread(osLeft.str());
			rightMat = cv::imread(osRight.str());


			vector<double> gpsDouble = data[i];
			std::vector<std::string> gpsString;
			gpsDouble.erase(gpsDouble.begin() + 1);
			toString(std::begin(gpsDouble), std::end(gpsDouble), std::back_inserter(gpsString));

			runRealtime(&hw, leftMat, rightMat, gpsString, i);
			std::cout << "Finished processing " << i << " image" << std::endl;
		}
	}
	else {
		namespace fs = std::experimental::filesystem;// this is c++ 14 feature
		std::string fname;
		std::ostringstream os;


		os << "logs/log_" << startTime;
		resultFolder = os.str();
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
		double fps = 20;
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
			if (sleepTimeMicro) {
				std::cout << "Overran by" << sleepTimeMicro / 1000000 << "seconds" << endl;
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
	return 0;
}


void uploadImage(const char *file)
{
	//Uses libcurl to upload image to the server. The server is a REST api accepting HTTP PUT
	url = "http://localhost:8080/Pothole_server_war_exploded/upload";
	//file = "img.jpg";

	/* get the file size of the local file */
	stat(file, &file_info);

	/* get a FILE * of the same file, could also be made with
	fdopen() from the previous descriptor, but hey this is just
	an example! */
	hd_src = fopen(file, "rb");

	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) {
		/* we want to use our own read function */
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, fread);

		/* enable uploading */
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		/* HTTP PUT please */
		//curl_easy_setopt(curl, CURLOPT_PUT, 1L);

		/* specify target URL, and note that this URL should include a file
		name, not only a directory */
		curl_easy_setopt(curl, CURLOPT_URL, url);

		/* now specify which file to upload */
		curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		/* provide the size of the upload, we specicially typecast the value
		to curl_off_t since we must be sure to use the correct data size */
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
			(curl_off_t)file_info.st_size);

		///* Build an HTTP form with a single field named "file", */
		//mime = curl_mime_init(curl);
		//part = curl_mime_addpart(mime);
		//curl_mime_data(part, "This is the field data", CURL_ZERO_TERMINATED);
		//curl_mime_name(part, "file");

		//curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

		/* Now run off and do what you've been told! */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	fclose(hd_src); /* close the local file */

	curl_global_cleanup();

}