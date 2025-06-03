// video.h
#pragma once

#ifndef APP_VIDEO_H 
#define APP_VIDEO_H  

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h> 
#include <Shlwapi.h>
#include <String>
#include <vector>
#include <wrl/client.h> // For Microsoft::WRL::ComPtr
#include <chrono>
#include <iostream>
#include "IHostComponent.h"
#include <thread>
#include "datastructures.h"
#include "stream.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "gdiplus.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "shlwapi.lib")

namespace video {
	using namespace Gdiplus;
	using Microsoft::WRL::ComPtr;

	extern GdiplusStartupInput gsi;
	extern ULONG_PTR gdiToken;
	extern Status gdiRes;
	extern void InitGdiplus();

	

	struct frame_data_t {
		BYTE* pFrame = nullptr;
		HGLOBAL hMemBuf = nullptr;
		ComPtr<IStream> pStream; // COM smart pointer
		SIZE_T frameSize = 0;

		~frame_data_t() {
			if (hMemBuf && pFrame) {
				GlobalUnlock(hMemBuf);
				pFrame = nullptr;
			}
			// pStream is released automatically by ComPtr
			hMemBuf = nullptr;
		}

		// Optional: Reset method to reuse this struct
		void reset() {
			if (hMemBuf && pFrame) {
				GlobalUnlock(hMemBuf);
				pFrame = nullptr;
			}
			hMemBuf = nullptr;
			pStream.Reset();
			frameSize = 0;
		}
	};
	using SharedFramePtr = std::shared_ptr<video::frame_data_t>;

	class VideoStreamData : public stream::StreamData<SharedFramePtr> {
	public:
		VideoStreamData(SharedFramePtr data);
		~VideoStreamData();
		const SharedFramePtr& getData() override;
		const BYTE* getBytesDataPtr() override;
		SIZE_T getSize() override;
	};

	class VideoCodec;
	class VideoCapture;
	class VideoStream;

	static Gdiplus::EncoderParameters getEncoderParams(ULONG* pQuality);

	class VideoCodec {
	public:
		VideoCodec();

		virtual void setCodec(CLSID encoder);

		virtual CLSID getCodec();

		virtual void setQuality(ULONG quality);

		virtual ComPtr<IStream> encodeFrame(HBITMAP& hBmp);

		virtual ComPtr<IStream> decodeFrame();

		static CLSID getEncoder(const wchar_t encoderName[]);

	private:
		CLSID encoder;
		EncoderParameters encParams;
		ULONG quality;
	};
	class VideoCapture {
	public:
		VideoCapture();
		~VideoCapture();

		virtual void init();
		virtual void setCodec(CLSID codec);
		virtual bool getCurrentFrame(frame_data_t* pFrameData);
	private:
		int screenWidth;
		int screenHeight;
		HDC hScreen;
		HDC hMem;
		HBITMAP hBmp;
		VideoCodec codec;

	};

	class VideoStreamContainer : public stream::StreamVectorContainer<VideoStreamData> {
	public:
		VideoStreamContainer();
		~VideoStreamContainer();

		VideoStreamData pop() override;
		VideoStreamData popFront() override;
		VideoStreamData popBack() override;
		void push(VideoStreamData data) override;
		void pushFront(VideoStreamData data) override;
		void pushBack(VideoStreamData data) override;
		bool isEmpty() override;
		void clear() override;
	private:
		ThreadSafeQueue<VideoStreamData> tsFrameQueue;
	};

	class VideoStream : public stream::VectorStream<VideoStreamData, VideoStreamContainer>, public IHostComponent {
	public:
		VideoStream();
		~VideoStream();

		void initStream() override;

	private:
		VideoCapture videoCapture;
		std::atomic<std::chrono::milliseconds> FPS;
	};
};
#endif //APP_VIDEO_H