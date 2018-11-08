// Listener.cpp: implementation of the CListener class.
//
//////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT 0x0500

#include <iostream>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>

#include <cstdio>
#include <cstdlib>
#include <cstring> // for memset
#include <limits>
#include <vector>

#include <math.h>

#include "Listener.h"




//#include "Engine.h"
//using namespace matlab::engine;

//#pragma comment (lib,"libeng");
//#pragma comment (lib,"libmat");
//#pragma comment (lib,"libmex");
//#pragma comment (lib,"");


using namespace DShowLib;
//using namespace cv::dnn;
//using namespace Eigen;

std::mutex m;
std::condition_variable conditionalVariable;
std::atomic_int num_captured;





LARGE_INTEGER trigger_time, last_trigger_time, last_frame_time, frame_time, mood;



//int mood_swich;
//////////////////////////////////////////////////////////////////////////
/*! The overlayCallback() method draws the number of the current frame. The
	frame count is a member of the tsMediaSampleDesc structure that is passed
	to overlayCallback() by the Grabber.
*/
void	CListener::overlayCallback( Grabber& caller, smart_ptr<OverlayBitmap> pBitmap, 
								   const tsMediaSampleDesc& MediaSampleDesc)
{
	char szText[25];
	if( pBitmap->getEnable() == true ) // Draw only, if the overlay bitmap is enabled.
	{
		sprintf( szText,"%04d ", MediaSampleDesc.FrameNumber);
		pBitmap->drawText( RGB(255,0,0), 0, 0, szText );
	}
}

//////////////////////////////////////////////////////////////////////////
/*! The frameReady() method calls the saveImage method to save the image buffer to disk.
*/
void	CListener::frameReady( Grabber& caller, smart_ptr<MemBuffer> pBuffer, DWORD currFrame)
{
	last_frame_time = frame_time;
	QueryPerformanceCounter(&frame_time);

	std::unique_lock<std::mutex> lck(m);
	

	mood.QuadPart = mood.QuadPart + 1;
	//saveImage(pBuffer, currFrame);
	exportImageMat(pBuffer, currFrame);
	conditionalVariable.notify_all();

}
void CListener::exportImageMat(smart_ptr<MemBuffer> pBuffer, DWORD currFrame)
{
	BYTE* pImageData;
	int iImageSize;
	//generate image from the buffer
	pImageData = pBuffer->getPtr();
	smart_ptr<BITMAPINFOHEADER> pInf = pBuffer->getBitmapInfoHeader();
	iImageSize = pInf->biWidth * pInf->biHeight * pInf->biBitCount / 8;
	// Now loop through the data and change every byte. This small sample inverts every pixel.
	/*for (int i = 0; i < iImageSize; i++)
	{
		pImageData[i] = pImageData[i];
	}*/
	/// read image into buffer using OpenCV 
	cv::Mat imgtmp(pInf->biHeight, pInf->biWidth, CV_8UC3, pImageData);
	this->img = imgtmp;
	num_captured.fetch_add(1);
}
//////////////////////////////////////////////////////////////////////////
/*! Initialize the array of bools that is used to memorize, which buffers were processed in 
	the frameReady() method. The size of the array is specified by the parameter NumBuffers.
	It should be equal to the number of buffers in the FrameHandlerSink.
	All members of m_BufferWritten are initialized to false.
	This means that no buffers have been processed.
*/
void	CListener::setBufferSize( unsigned long NumBuffers )
{
	//NumBuffers = 1000000;
	m_BufferWritten.resize(NumBuffers, false);
}

//////////////////////////////////////////////////////////////////////////
/*! The image passed by the MemBuffer pointer is saved to a BMP file.
*/
//void	CListener::saveImage(smart_ptr<MemBuffer> pBuffer, DWORD currFrame)
//{
//	int iImageSize;
//	try
//	{
//		if (mood.QuadPart % 2 > 0)
//			//if (mood_swich % 2 > 0)
//		{
//			sprintf(filename_R, "image_Right_%02i.bmp", currFrame);
//			pBuffer_R = pBuffer;
//			//pBuffer_LR[1] = pBuffer;
//
//			//generate image from the buffer
//			pImageData_Right = pBuffer_R->getPtr();
//			smart_ptr<BITMAPINFOHEADER> pInf = pBuffer_R->getBitmapInfoHeader();
//			iImageSize = pInf->biWidth * pInf->biHeight * pInf->biBitCount / 8;
//			// Now loop through the data and change every byte. This small sample inverts every pixel.
//			for (int i = 0; i < iImageSize; i++)
//			{
//				pImageData_Right[i] = pImageData_Right[i];
//			}
//			/// read image into buffer using OpenCV 
//			cv::Mat RGB_img_Right(pInf->biHeight, pInf->biWidth, CV_8UC3, pImageData_Right);
//			/*imshow("imgRight", RGB_img_Right);
//
//			cv::waitKey(1);*/
//
//			cv::Point2f src_center(RGB_img_Right.cols / 2.0F, RGB_img_Right.rows / 2.0F);
//			cv::Mat rot_mat = getRotationMatrix2D(src_center, 180, 1.0);
//			//Mat dst;
//			warpAffine(RGB_img_Right, imgRight, rot_mat, RGB_img_Right.size());
//
//			//	imwrite("RGB_img_Right.jpg", imgRight);
//
//			saveToFileBMP(*pBuffer_R, filename_R);
//			//m_BufferWritten.at(currFrame) = false; // true;
//			//pBuffer = nullptr;
//		}
//		else
//		{
//			sprintf(filename_L, "image_Left_%02i.bmp", currFrame);
//			pBuffer_L = pBuffer;
//			/*pBuffer_LR[0] = pBuffer;*/
//
//			//generate image from the buffer
//			//pImageData_Left = pBuffer_L->getPtr();
//			//smart_ptr<BITMAPINFOHEADER> pInf = pBuffer_L->getBitmapInfoHeader();
//			//iImageSize = pInf->biWidth * pInf->biHeight * pInf->biBitCount / 8;
//			//// Now loop through the data and change every byte. This small sample inverts every pixel.
//			//for (int i = 0; i < iImageSize; i++)
//			//{
//			//	pImageData_Left[i] = pImageData_Left[i];
//			//}
//			///// read image into buffer using OpenCV 
//			//cv::Mat RGB_img_Left(pInf->biHeight, pInf->biWidth, CV_8UC3, pImageData_Left);
//			//imshow("imgRight", RGB_img_Left);
//			//cv::waitKey(1);
//
//			//imwrite("RGB_img_Left.jpg", RGB_img_Left);
//
//			saveToFileBMP(*pBuffer, filename_L);
//			m_BufferWritten.at(currFrame) = true;
//
//			pBuffer = nullptr;
//
//			double A = cv::getTickCount();
//			Stereo(currFrame); // Pothole recognition and 3D localisation
//			double B = cv::getTickCount();
//			double time = (B - A) / cv::getTickFrequency();
//			std::cout << "running time = " << time << "\n"; //the time of execution in seconds
//		}
//	}
//	catch (int e)
//	{
//		std::cout << "An exception occurred. Exception Nr. " << e << '\n';
//	}
//}


// End QSF Algorithms ************
//>>