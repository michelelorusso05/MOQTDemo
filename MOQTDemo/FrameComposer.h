#pragma once

#include <cstdint>
#include <queue>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <functional>

#include "lz4.h"

struct Frame {
    int32_t id;
    const uint8_t* data;
    size_t size;
};

class FrameComposer
{
public:
    using OnDataReady = std::function<void(uint8_t* data, size_t size)>;
    using OnChunkUsed = std::function<void(int32_t handle)>;

    FrameComposer();
    ~FrameComposer();

    /// <summary>
    /// Imposta il callback da chiamare quando un intero frame è stato ricevuto, ricomposto e decompresso.
    /// </summary>
    /// <param name="callback">Il callback da invocare, di tipo OnDataReady</param>
    void setOnDataReadyCallback(OnDataReady callback);

    /// <summary>
    /// Imposta il callback da chiamare quando una sequenza di dati in ingresso è stata consumata.
    /// Se i dati inoltrati hanno bisogno di essere liberati dopo l'utilizzo, fai qui pulizia.
    /// </summary>
    /// <param name="callback">Il callback da invocare, di tipo OnChunkUsed.</param>
    void setOnChunkUsedCallback(OnChunkUsed callback);

    /// <summary>
    /// Inoltra un nuovo frame MOQ.
    /// </summary>
    /// <param name="frame">Il frame, di tipo Frame (id, data, size).</param>
    /// <param name="timestamp">Il timestamp, in microsecondi, in cui è stato ricevuto il frame.</param>
    void enqueueFrame(Frame frame, uint64_t timestamp);

    /// <summary>
    /// Termina esplicitamente il thread di elaborazione frame. Chiamato normalmente dal distruttore.
    /// </summary>
    void stop();

    /// <summary>
    /// Svuota i buffer e sblocca il thread di elaborazione frame.
    /// Da chiamare se il fornitore dati ha riscontrato un errore irreversibile durante la ricezione di un frame,
    /// e ha lasciato i dati a metà.
    /// </summary>
    void flush();

    /// <summary>
    /// Invoca la chiusura esplicita di un frame, e invia i dati parziali fin'ora ricevuti al consumatore.
    /// Da chiamare se gli ultimi dati sono stati ricevuti e il fornitore dati ha terminato l'esecuzione.
    /// </summary>
    void closeGroup();
private:
    void loop();

    const size_t MAX_QUEUE_SIZE = 64;
    std::queue<std::vector<Frame>> groupQueue;

    std::vector<Frame> currentGroup;
    uint64_t currentGroupTimestamp = 0;

    std::mutex queueMutex;
    std::condition_variable queueConditionVariable;
    std::atomic<bool> isThreadRunning;

    // Buffer di appoggio per la decompressione
    const int MAX_UNCOMPRESSED_SIZE = 3 * 1024 * 1024;
    uint8_t* decompressedBuffer;

    OnDataReady onDataReady;
    OnChunkUsed onChunkUsed;

    std::thread workerThread;
};

