#include "video.h"
namespace video {


	GdiplusStartupInput gsi;
	ULONG_PTR           gdiToken = 0;
	Status              gdiRes = Ok;

	// call this before any GDI+ usage:
	void video::InitGdiplus()
	{
		ZeroMemory(&gsi, sizeof(gsi));
		gsi.GdiplusVersion = 1;
		gsi.DebugEventCallback = nullptr;
		gsi.SuppressBackgroundThread = FALSE;
		gsi.SuppressExternalCodecs = FALSE;

		gdiRes = GdiplusStartup(&gdiToken, &gsi, nullptr);
	}

	static Gdiplus::EncoderParameters video::getEncoderParams(ULONG* pQuality) {
		Gdiplus::EncoderParameters encoderParams;
		encoderParams.Count = 1;
		encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
		encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParams.Parameter[0].NumberOfValues = 1;
		encoderParams.Parameter[0].Value = pQuality;

		return encoderParams;
	};

	video::VideoCodec::VideoCodec() {
		this->quality = 70;
		this->encParams = getEncoderParams(&this->quality);
		this->encoder = CLSID_NULL;
	};

	void video::VideoCodec::setCodec(CLSID encoder) {
		this->encoder = encoder;
	};

	CLSID video::VideoCodec::getCodec() {
		return this->encoder;
	};

	void video::VideoCodec::setQuality(ULONG quality) {
		this->quality = quality;
	};

	ComPtr<IStream> video::VideoCodec::encodeFrame(HBITMAP& hBmp) {
		Bitmap bmp(hBmp, NULL);
		ComPtr<IStream> pStream;
		HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, pStream.GetAddressOf());
		if (FAILED(hr))
			return nullptr;

		bmp.Save(pStream.Get(), &this->encoder, &this->encParams);
		return pStream;
	};

	ComPtr<IStream> video::VideoCodec::decodeFrame() {
		return nullptr;
	};

	CLSID video::VideoCodec::getEncoder(const wchar_t encoderName[]) {
		UINT sz = 0, count = 0;
		Gdiplus::GetImageEncodersSize(&count, &sz);
		std::vector<BYTE> buf(sz);
		auto* enc = reinterpret_cast<ImageCodecInfo*>(buf.data());
		Gdiplus::GetImageEncoders(count, sz, enc);
		for (UINT i = 0; i < count; i++) {
			if (wcscmp(enc[i].MimeType, encoderName) == 0) {
				return enc[i].Clsid;
				break;
			}
		}
		return CLSID_NULL;
	};

	video::VideoCapture::VideoCapture() {

	};

	video::VideoCapture::~VideoCapture() {
		DeleteObject(this->hBmp);
		DeleteDC(this->hMem);
		ReleaseDC(NULL, this->hScreen);
	}
	void video::VideoCapture::init() {
		this->screenWidth = GetSystemMetrics(SM_CXSCREEN);
		this->screenHeight = GetSystemMetrics(SM_CYSCREEN);
		this->hScreen = GetDC(NULL);
		this->hMem = CreateCompatibleDC(this->hScreen);
		this->hBmp = CreateCompatibleBitmap(this->hScreen, this->screenWidth, this->screenHeight);
	}
	void video::VideoCapture::setCodec(CLSID codec) {
		this->codec.setCodec(codec);
	}
	bool video::VideoCapture::getCurrentFrame(frame_data_t* pFrameData) {
		HBITMAP oldhBmp = (HBITMAP)SelectObject(this->hMem, this->hBmp);
		BOOL resBlt = BitBlt(this->hMem, 0, 0, this->screenWidth, this->screenHeight, this->hScreen, 0, 0, SRCCOPY);
		if (!resBlt) {
			std::cerr << "BitBlt failed!" << std::endl;
			SelectObject(this->hMem, oldhBmp);
			return false;
		}
		ComPtr<IStream> pStream = this->codec.encodeFrame(this->hBmp);
		if (!pStream) {
			std::cerr << "Stream encoding failed!" << std::endl;
			SelectObject(this->hMem, oldhBmp);
			return false;
		}

		HGLOBAL hMemBuf = NULL;
		HRESULT res = GetHGlobalFromStream(pStream.Get(), &hMemBuf);
		if (FAILED(res)) {
			std::cerr << "Failed to get HGLOBAL from stream!" << std::endl;
			SelectObject(this->hMem, oldhBmp);
			return false;
		}

		BYTE* pFrame = (BYTE*)GlobalLock(hMemBuf);
		if (!pFrame) {
			std::cerr << "Global lock on HGLOBAL failed!" << std::endl;
			SelectObject(this->hMem, oldhBmp);
			return false;
		}

		// Set frame data fields 
		pFrameData->pFrame = pFrame;
		pFrameData->hMemBuf = hMemBuf;
		pFrameData->pStream = pStream;
		pFrameData->frameSize = GlobalSize(hMemBuf);

		SelectObject(this->hMem, oldhBmp);
		return true;
	};

	video::VideoStream::VideoStream() {
		this->videoCapture.init();
		const wchar_t* codecName = L"image/jpeg";
		this->videoCapture.setCodec(VideoCodec::getEncoder(codecName));
		this->FPS = std::chrono::milliseconds(55);
		this->continueStream();
	}

	video::VideoStream::~VideoStream() {

	}

	void video::VideoStream::initStream() {
		while (isProgramOn()) {
			while (!this->isStreamOn());// loop so it doesnt send when the receiver is overwhelmed

			auto startTime = std::chrono::system_clock::now();
			const auto pFrameData = std::make_shared<video::frame_data_t>();
			if (!this->videoCapture.getCurrentFrame(pFrameData.get())) {
				std::cerr << "Capture failed\n";
				continue;
			}

			this->streamContainer.push(VideoStreamData(pFrameData));

			auto finishTime = std::chrono::system_clock::now();
			auto totalTime = finishTime - startTime;
			std::chrono::milliseconds totalTimeInMillis = std::chrono::duration_cast<std::chrono::milliseconds>(totalTime);
			std::this_thread::sleep_for(this->FPS.load() - totalTimeInMillis);
		}
	}

	// Video Stream Data
	video::VideoStreamData::VideoStreamData(SharedFramePtr data) { this->data = data; };
	video::VideoStreamData::~VideoStreamData() {};
	const SharedFramePtr& video::VideoStreamData::getData() { return this->data; };
	const BYTE* video::VideoStreamData::getBytesDataPtr() { return this->data->pFrame; };
	SIZE_T video::VideoStreamData::getSize() { return this->data->frameSize; };

	// Video Stream Container
	video::VideoStreamContainer::VideoStreamContainer() {};
	video::VideoStreamContainer::~VideoStreamContainer() {};
	VideoStreamData video::VideoStreamContainer::pop() { return this->tsFrameQueue.Pop(); };
	VideoStreamData video::VideoStreamContainer::popFront() { return this->tsFrameQueue.Pop(); };
	VideoStreamData video::VideoStreamContainer::popBack() { return this->tsFrameQueue.Pop(); };
	void video::VideoStreamContainer::push(VideoStreamData data) { this->tsFrameQueue.Push(data); };
	void video::VideoStreamContainer::pushFront(VideoStreamData data) { this->tsFrameQueue.Push(data); };
	void video::VideoStreamContainer::pushBack(VideoStreamData data) { this->tsFrameQueue.Push(data); };
	bool video::VideoStreamContainer::isEmpty() { return this->tsFrameQueue.isEmpty(); };
	void video::VideoStreamContainer::clear() { this->tsFrameQueue.Clear(); };
}

