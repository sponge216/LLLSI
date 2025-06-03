// video.h

#ifndef APP_VIDEO_H 
#define APP_VIDEO_H  

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h> 
#include <Shlwapi.h>
#include <String>
#include <vector>
#include <wrl/client.h>
#include <memory>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "gdiplus.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "shlwapi.lib")

namespace video {
	using namespace Gdiplus;
	using Microsoft::WRL::ComPtr;

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

	class VideoCodec;
	class VideoCapture;



	static Gdiplus::EncoderParameters getEncoderParams(ULONG* pQuality) {
		Gdiplus::EncoderParameters encoderParams;
		encoderParams.Count = 1;
		encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
		encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParams.Parameter[0].NumberOfValues = 1;
		encoderParams.Parameter[0].Value = pQuality;

		return encoderParams;
	};
	class VideoCodec {
	public:
		VideoCodec() {
			this->quality = 80;
			this->encParams = getEncoderParams(&this->quality);
			this->encoder = CLSID_NULL;
		};

		virtual void setCodec(CLSID encoder) {
			this->encoder = encoder;
		};

		virtual CLSID getCodec() {
			return this->encoder;
		};

		virtual void setQuality(ULONG quality) {
			this->quality = quality;
		};

		virtual ComPtr<IStream> encodeFrame(HBITMAP& hBmp) {
			Bitmap bmp(hBmp, NULL);
			ComPtr<IStream> pStream;
			HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, pStream.GetAddressOf());
			if (FAILED(hr))
				return nullptr;

			bmp.Save(pStream.Get(), &this->encoder, &this->encParams);
			return pStream;
		};

		virtual IStream* decodeFrame() {
			return nullptr;
		};

		static CLSID getEncoder(const wchar_t encoderName[]) {
			static CLSID cached = {};
			static bool initialized = false;

			if (!initialized) {
				UINT sz = 0, count = 0;
				GetImageEncodersSize(&count, &sz);
				std::vector<BYTE> buf(sz);
				auto* enc = reinterpret_cast<ImageCodecInfo*>(buf.data());
				GetImageEncoders(count, sz, enc);
				for (UINT i = 0; i < count; ++i) {
					if (wcscmp(enc[i].MimeType, encoderName) == 0) {
						cached = enc[i].Clsid;
						break;
					}
				}
				initialized = true;
			}
			return cached;
		};

	private:
		CLSID encoder;
		EncoderParameters encParams;
		ULONG quality;
	};
	class VideoCapture {
	public:
		VideoCodec codec;

		VideoCapture() {

		};

		virtual bool getCurrentFrame(HDC& hScreen, HDC& hMem, HBITMAP& hBmp, frame_data_t& frameData) {
			HBITMAP oldhBmp = (HBITMAP)SelectObject(hMem, hBmp);
			int w = GetSystemMetrics(SM_CXSCREEN);
			int h = GetSystemMetrics(SM_CYSCREEN);
			BitBlt(hMem, 0, 0, w, h, hScreen, 0, 0, SRCCOPY);

			// Now we receive a ComPtr<IStream>
			ComPtr<IStream> pStream = this->codec.encodeFrame(hBmp);
			if (!pStream) {
				SelectObject(hMem, oldhBmp);
				return false;
			}

			HGLOBAL hMemBuf = NULL;
			HRESULT res = GetHGlobalFromStream(pStream.Get(), &hMemBuf);
			if (FAILED(res)) {
				SelectObject(hMem, oldhBmp);
				return false;
			}

			BYTE* pFrame = (BYTE*)GlobalLock(hMemBuf);
			if (!pFrame) {
				SelectObject(hMem, oldhBmp);
				return false;
			}

			// Set frame data fields with safety
			frameData.pFrame = pFrame;
			frameData.hMemBuf = hMemBuf;
			frameData.pStream = pStream;
			frameData.frameSize = GlobalSize(hMemBuf);

			SelectObject(hMem, oldhBmp);
			return true;
		};
	};
}
#endif //APP_VIDEO_H