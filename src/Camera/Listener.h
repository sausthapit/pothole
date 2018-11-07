// Listener.h: interface for the CListener class.
// 
// The CListener class is derived from GrabberListener. It overwrites
// the "frameReady()" method. In the frameReady method the member method
// "saveImage()" is called.
// "saveImage()" saves the specified buffer to a BMP file and calls a "Sleep(250)" 
// to simulate a time expensive image processing. "saveImage()" is also called
// by the main() function of this example to save all buffers, that have
// not been processed in the frameReady method.#
//
// This class also overwrites the overlayCallback method to draw a 
// frame counter.
//
// The CListener object is registered to a Grabber with the parameter 
// eFRAMEREADY|eOVERLAYCALLBACK . 
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTENER_H__3E017E1D_6B0A_472C_9F9C_0C5F9A8DFB23__INCLUDED_)
#define AFX_LISTENER_H__3E017E1D_6B0A_472C_9F9C_0C5F9A8DFB23__INCLUDED_

#pragma once

#include <tisudshl.h>

#include <stdlib.h>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv/cv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/trace.hpp>

#include <fstream>
#include <iostream>
#include <cstdlib>



//#include <opencv2/core/eigen.hpp>
//using namespace std;
//using namespace cv;
//using namespace cv::dnn;

extern std::mutex m;
extern std::condition_variable conditionalVariable;
extern bool frame_ready;
extern LARGE_INTEGER trigger_time, last_trigger_time, frame_time, last_frame_time, mood;
// Global variable declaration:
//int mood_swich;

class CListener : public DShowLib::GrabberListener
{
private:
public:
	cv::Mat img;
	// Overwrite the GrabberListener methods we need
	virtual void overlayCallback(DShowLib::Grabber& caller, smart_ptr<DShowLib::OverlayBitmap> pBitmap, const DShowLib::tsMediaSampleDesc& MediaSampleDesc);
	virtual void frameReady(DShowLib::Grabber& caller, smart_ptr<DShowLib::MemBuffer> pBuffer, DWORD FrameNumber);

	void exportImageMat(smart_ptr<DShowLib::MemBuffer> pBuffer, DWORD currFrame);

	// Save one image and mark it as saved
	void		saveImage(smart_ptr<DShowLib::MemBuffer> pBuffer, DWORD currFrame);
	// Setup the buffersize. 
	void		setBufferSize(unsigned long NumBuffers);

	std::vector<bool>	m_BufferWritten;	// array of flags which buffers have been saved.

};
bool Stereo(int);
void MvStereoRectify(cv::Mat _rmap[2][2], cv::Rect vRoi[2]);
void Pothole_Recognition();
void Depth_Estimation(cv::Mat imgDisparity8U);
#endif // !defined(AFX_LISTENER_H__3E017E1D_6B0A_472C_9F9C_0C5F9A8DFB23__INCLUDED_)
