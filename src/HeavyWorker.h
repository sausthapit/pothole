#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <Eigen/QR>
#include <Eigen/Dense>
#include <opencv2/core.hpp>
#include <opencv/cv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/trace.hpp>
#include <opencv2/core/eigen.hpp>
#include <thread>

#include <tisudshl.h>


class HeavyWorker 
{
public:
	HeavyWorker();
	~HeavyWorker();
	std::vector<std::pair<cv::Mat, float>> Stereo(cv::Mat,cv::Mat);
	 std::vector<cv::String> readClassNames(const char *);
	int Pothole_Recognition();
	void MvStereoRectify(cv::Mat _rmap[2][2], cv::Rect vRoi[2]);
	 void getMaxClass(const cv::Mat &probBlob, int *classId, double *classProb);
	float Depth_Estimation(cv::Mat imgDisparity8U);
private:
	int SADWindowSize = 5;
	int numberOfDisparities = 192;
	int preFilterCap = 31;
	int minDisparity = 0;
	int uniquenessRatio = 15;
	int speckleWindowSize = 0;
	int speckleRange = 0;
	int disp12MaxDiff = -1;
	int fullDP = false;
	int P1 = 200;
	int P2 = 800;

	cv::Mat Q;

	cv::String modelTxt = "aaaa.prototxt";
	cv::String modelBin = "alaa_project_train_iter_100.caffemodel";
	cv::dnn::Net net = cv::dnn::readNetFromCaffe(modelTxt, modelBin);
	cv::Rect patch;
	cv::Mat inputBlob;
	cv::Mat imgDisparity8U_Patch;
	const char *windowDisparity = "Disparity";
	int array2D[5][2];

	BYTE* pImageData_Left;
	BYTE* pImageData_Right;

	std::string imageName;
	std::string FullPath; // save to the current directory
	cv::Mat prob;
	int classId;
	double classProb;

	smart_ptr<DShowLib::MemBuffer> pBuffer_L, pBuffer_R;
	smart_ptr<DShowLib::MemBuffer> pBuffer_LR[2];
	
	BYTE*  pImageData_L;
	smart_ptr<BITMAPINFOHEADER> pInf, pInf_L;
	// Calculate the size of the image.
	
	cv::Mat RGB_img_L, RGB_img_R;
	cv::Mat imgLeft, imgRight, greyMat_L, greyMat_R;

	char filename_L[MAX_PATH];
	char filename_R[MAX_PATH];
	std::string class_name;
};

