#include "MOQTManager.h"
#include <iostream>

MOQTManager::MOQTManager()
{
	origin = moq_origin_create();
	broadcast = -1;
	session = -1;
}

MOQTManager::~MOQTManager()
{
	moq_consume_close(broadcast);
	moq_session_close(session);
	moq_origin_close(origin);
}

bool MOQTManager::connect(std::string url)
{
	moq_client_config config = { 0 };

	// Disabilita la verifica dei certificati a scopo di debug
	config.tls_disable_verify = true;

	session = moq_session_connect(url.c_str(), url.length(), &config, 0, origin, onSessionStatusWrapper, this);
	
	return session > 0;
}

bool MOQTManager::waitForPublisher(std::string path)
{
	isSubscribed = true;
	int32_t broadcastRequest = moq_origin_announced_broadcast(origin, path.c_str(), path.length(), onBroadcastReadyWrapper, this);

	return broadcastRequest > 0;
}

void MOQTManager::setTrack(std::string track)
{
	this->targetTrack = track;
}

void MOQTManager::onSessionStatusWrapper(void* userData, int32_t code)
{
	MOQTManager* self = static_cast<MOQTManager*>(userData);
	self->onSessionStatus(code);
}

void MOQTManager::onSessionStatus(int32_t code)
{

}

void MOQTManager::onBroadcastReadyWrapper(void* userData, int32_t code)
{
	MOQTManager* self = static_cast<MOQTManager*>(userData);
	self->onBroadcastReady(code);
}

bool MOQTManager::onBroadcastReady(int32_t code)
{
	if (code > 0)
	{
		broadcast = code;

		struct moq_subscription sub = { 0 };
		sub.max_age_us = 0;

		int32_t track = moq_consume_track(broadcast, targetTrack.c_str(), targetTrack.length(), &sub, onTrackFrameReadyWrapper, this);
		return track > 0;
	}

	return false;
}

void MOQTManager::onTrackFrameReadyWrapper(void* userData, int32_t code)
{
	MOQTManager* self = static_cast<MOQTManager*>(userData);
	self->onTrackFrameReady(code);
}

void MOQTManager::onTrackFrameReady(int32_t code)
{
	if (code > 0)
	{
		struct moq_frame frame;
		int32_t result = moq_consume_track_frame(code, &frame);
		if (result == 0)
		{
			if (onTrackFrameCallback)
				onTrackFrameCallback(code, frame.payload, frame.payload_size, frame.timestamp_us);
			else
				freeFrame(code);
		}
	}
	else if (code == 0)
	{
		if (onTrackClosedCallback)
			onTrackClosedCallback();

		isSubscribed = false;
		broadcast = -1;
	}
	else
	{
		if (onTrackErrorCallback)
			onTrackErrorCallback();

		// MOQ: old
		if (code == -18)
			needsResubscribe = true;
		else
		{
			isSubscribed = false;
			broadcast = -1;
		}
	}
}

void MOQTManager::setOnTrackFrameCallback(FrameReceivedCallback callback)
{
	onTrackFrameCallback = callback;
}

void MOQTManager::setOnTrackClosedCallback(std::function<void(void)> callback)
{
	onTrackClosedCallback = callback;
}

void MOQTManager::setOnTrackErrorCallback(std::function<void(void)> callback)
{
	onTrackErrorCallback = callback;
}

void MOQTManager::freeFrame(uint32_t frame)
{
	moq_consume_track_frame_free(frame);
}

bool MOQTManager::shouldResubscribe()
{
	return needsResubscribe && broadcast > 0;
}

bool MOQTManager::subscribeRequested()
{
	return isSubscribed;
}

void MOQTManager::attemptResubscribe()
{
	needsResubscribe = !onBroadcastReady(broadcast);
}