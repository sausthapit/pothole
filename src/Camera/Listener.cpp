// Listener.cpp: implementation of the CListener class.
//
//////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT 0x0500

#include <iostream>

#include "Listener.h"

using namespace DShowLib;

//////////////////////////////////////////////////////////////////////////
/*! The overlayCallback() method draws the number of the current frame. The
	frame count is a member of the tsMediaSampleDesc structure that is passed
	to overlayCallback() by the Grabber.
*/
void	CListener::overlayCallback( Grabber& caller, smart_ptr<OverlayBitmap> pBitmap, 
								   const tsMediaSampleDesc& MediaSampleDesc)
{
	char szText[25];
	if( pBitmap->getEnable() ) // Draw only, if the overlay bitmap is enabled.
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
	std::cout << "Buffer " << currFrame << " processed in CListener::frameReady()." << std::endl;

	saveImage( pBuffer, currFrame ); // Do the buffer processing.

	::Sleep(250); // Simulate a time expensive processing.
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
	m_BufferWritten.resize( NumBuffers, false );
}

//////////////////////////////////////////////////////////////////////////
/*! The image passed by the MemBuffer pointer is saved to a BMP file.
*/
void	CListener::saveImage( smart_ptr<MemBuffer> pBuffer, DWORD currFrame)
{
	char filename[MAX_PATH];
	if( currFrame < m_BufferWritten.size() )
	{
		sprintf( filename, "image%02i.bmp", currFrame );

		saveToFileBMP( *pBuffer, filename );

		m_BufferWritten.at( currFrame ) = true;
	}
}
//>>