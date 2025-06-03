#pragma once
#ifndef APP_STREAM_H
#define APP_STREAM_H

#include <atomic>
#include <WTypesbase.h>
namespace stream {

	template<typename Data>
	class StreamData {
	public:
		virtual const Data& getData() = 0;
		virtual const BYTE* getBytesDataPtr() = 0;
		virtual SIZE_T getSize() = 0;
	protected:
		Data data;
	};

	template <typename StreamData>
	class StreamContainer {
	public:
		virtual bool isEmpty() = 0;
		virtual void clear() = 0;
	};

	template <typename StreamKey, typename StreamValue>
	class StreamMapContainer : public StreamContainer<StreamValue> {
	public:
		virtual StreamValue pop(StreamKey key) = 0;
		virtual void push(StreamKey key, StreamValue value) = 0;
	};

	template <typename StreamData>
	class StreamVectorContainer : public StreamContainer<StreamData> {
	public:
		virtual StreamData pop() = 0;
		virtual void push(StreamData data) = 0;
		virtual StreamData popFront() = 0;
		virtual StreamData popBack() = 0;
		virtual void pushFront(StreamData data) = 0;
		virtual void pushBack(StreamData data) = 0;
	};

	template <typename StreamData, typename Container, typename = std::enable_if_t<std::is_base_of_v<stream::StreamContainer<StreamData>, Container>>>
	class Stream {
	public:
		Stream() {};

		Stream(CHAR streamType, WORD frameSize) {
			this->streamType = streamType;
			this->frameSize = frameSize;
		};

		~Stream() {};

		virtual const Container& getContainer() { return this->streamContainer; }

		virtual void setStreamType(CHAR streamType) { this->streamType = streamType; };

		virtual CHAR getStreamType() { return this->streamType; };

		virtual void setFrameSize(WORD frameSize) { this->frameSize = frameSize; };

		virtual WORD getFrameSize() { return this->frameSize; };

		virtual void stopStream() { this->streamStatus = false; };

		virtual void continueStream() { this->streamStatus = true; };

		virtual bool isStreamOn() { return this->streamStatus; }

		virtual void initStream() = 0;

		virtual void clearStream() { this->streamContainer.clear(); };

	protected:
		Container streamContainer;

	private:
		std::atomic<bool> streamStatus = false;
		CHAR streamType = -1;
		WORD frameSize = -1;
	};

	template <typename StreamData, typename Container, typename = std::enable_if_t<std::is_base_of_v<stream::StreamVectorContainer<StreamData>, Container>>>
	class VectorStream : public Stream<StreamData, Container> {
	public:
		VectorStream() {};

		VectorStream(CHAR streamType, WORD frameSize) : Stream<StreamData, Container>(streamType, frameSize) {};

		~VectorStream() {};

		virtual StreamData frontFrame() { return this->streamContainer.popFront(); };

		virtual StreamData backFrame() { return this->streamContainer.popBack(); };
	};

	template <typename StreamKey, typename StreamValue, typename Container, typename = std::enable_if_t<std::is_base_of_v<stream::StreamMapContainer<StreamKey, StreamValue>, Container>>>
	class MapStream : public Stream<StreamValue, Container> {
	public:
		MapStream() {};

		MapStream(CHAR streamType, WORD frameSize) : Stream<StreamData, Container>(streamType, frameSize) {};

		~MapStream() {};

		virtual StreamValue pop(StreamKey key) = 0;

		virtual void push(StreamKey key, StreamValue value) = 0;
	};
};
#endif // APP_STREAM_H