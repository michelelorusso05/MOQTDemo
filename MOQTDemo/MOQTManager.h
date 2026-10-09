#pragma once

#include "moq.h"
#include <cstdint>
#include <string>
#include <atomic>
#include <functional>

class MOQTManager
{
public:
	using FrameReceivedCallback = std::function<void(int32_t code, const uint8_t* data, size_t size, uint64_t timestamp)>;

	MOQTManager();
	~MOQTManager();

	/// <summary>
	/// Connetti ad un relay con url specificato.
	/// </summary>
	/// <param name="url">L'url del relay MOQ.</param>
	/// <returns>true se la richiesta di connessione ha avuto successo, false se si è verificato un errore MOQ.</returns>
	bool connect(std::string url);

	/// <summary>
	/// Metti in attesa di un publisher sul path specificato. Una volta arrivato, tenta in automatico l'iscrizione alla track scelta.
	/// </summary>
	/// <param name="path">Il path sul quale mettersi in ascolto.</param>
	/// <returns>true se la richiesta ha avuto successo, false se si è verificato un errore MOQ.</returns>
	bool waitForPublisher(std::string path);

	/// <summary>
	/// Imposta la track alla quale iscriversi una volta che è stata stabilita una connessione.
	/// </summary>
	/// <param name="track">La track a cui iscriversi.</param>
	void setTrack(std::string track);

	/// <summary>
	/// Imposta il callback da invocare quando viene ricevuto un frame.
	/// </summary>
	/// <param name="callback">Il callback da invocare.</param>
	void setOnTrackFrameCallback(FrameReceivedCallback callback);

	/// <summary>
	/// Imposta il callback da invocare quando il publisher chiude normalmente la track.
	/// </summary>
	/// <param name="callback">Il callback da invocare.</param>
	void setOnTrackClosedCallback(std::function<void(void)> callback);

	/// <summary>
	/// Imposta il callback da invocare quando si verifica un errore nel leggere la track.
	/// </summary>
	/// <param name="callback">Il callback da invocare.</param>
	void setOnTrackErrorCallback(std::function<void(void)> callback);

	/// <summary>
	/// Libera esplicitamente un frame.
	/// </summary>
	/// <param name="frame">L'handle del frame da liberare</param>
	void freeFrame(uint32_t frame);

	/// <summary>
	/// Controlla se è necessario riconnettersi alla track in seguito ad un errore irreversibile.
	/// </summary>
	/// <returns>true se è necessario riconnettersi, false se è tutto ok.</returns>
	bool shouldResubscribe();

	/// <summary>
	/// Prova a riconnettersi alla track specificata.
	/// </summary>
	void attemptResubscribe();

	bool subscribeRequested();

private:
	static void onSessionStatusWrapper(void* userData, int32_t code);
	void onSessionStatus(int32_t code);

	static void onBroadcastReadyWrapper(void* userData, int32_t code);
	bool onBroadcastReady(int32_t code);

	static void onTrackFrameReadyWrapper(void* userData, int32_t code);
	void onTrackFrameReady(int32_t code);

	FrameReceivedCallback onTrackFrameCallback;
	std::function<void(void)> onTrackClosedCallback;
	std::function<void(void)> onTrackErrorCallback;

	std::string targetTrack;

	uint32_t origin;
	uint32_t session;
	uint32_t broadcast;

	std::atomic_bool needsResubscribe;
	std::atomic_bool isSubscribed;
};

