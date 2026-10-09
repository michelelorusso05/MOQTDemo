#include "FrameComposer.h"

FrameComposer::FrameComposer()
    : isThreadRunning(true)
{
	decompressedBuffer = new uint8_t[MAX_UNCOMPRESSED_SIZE];
    workerThread = std::thread(&FrameComposer::loop, this);
}

FrameComposer::~FrameComposer()
{
	delete[] decompressedBuffer;
    stop();
}

void FrameComposer::stop()
{
    if (isThreadRunning)
    {
        isThreadRunning = false;
        queueConditionVariable.notify_all();
        if (workerThread.joinable())
        {
            workerThread.join();
        }
    }
}

void FrameComposer::loop()
{
    while (isThreadRunning)
    {
        std::vector<Frame> batch;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueConditionVariable.wait(lock, [=] {
                return !groupQueue.empty() || !isThreadRunning;
            });

            if (!isThreadRunning && groupQueue.empty()) break;

            batch = std::move(groupQueue.front());
            groupQueue.pop();
        }

        size_t totalCompressedSize = 0;
        for (const auto& frame : batch) {
            totalCompressedSize += frame.size;
        }

        std::vector<uint8_t> assembled_compressed_data(totalCompressedSize);
        size_t offset = 0;

        for (const auto& frame : batch) {
            memcpy(assembled_compressed_data.data() + offset, frame.data, frame.size);
            offset += frame.size;

            if (onChunkUsed)
                onChunkUsed(frame.id);
        }

        int uncompressed_size = LZ4_decompress_safe(
            (const char*) assembled_compressed_data.data(),
            (char*) decompressedBuffer,
            (int)totalCompressedSize,
            MAX_UNCOMPRESSED_SIZE
        );

        if (uncompressed_size > 0 && onDataReady) {
            onDataReady(decompressedBuffer, uncompressed_size);
        }
    }
}

void FrameComposer::closeGroup()
{
    if (!currentGroup.empty()) {
        std::lock_guard<std::mutex> lock(queueMutex);
        groupQueue.push(std::move(currentGroup));
        queueConditionVariable.notify_one();
        currentGroup.clear();
    }
}

void FrameComposer::enqueueFrame(Frame frame, uint64_t timestamp)
{
    if (timestamp != currentGroupTimestamp) {
        closeGroup();
    }

    currentGroupTimestamp = timestamp;
    currentGroup.push_back(frame);
}

void FrameComposer::flush()
{
    std::lock_guard<std::mutex> lock(queueMutex);

    for (auto& job : currentGroup) {
        if (onChunkUsed)
            onChunkUsed(job.id);
    }

    currentGroup.clear();
}

void FrameComposer::setOnDataReadyCallback(OnDataReady callback)
{
    onDataReady = callback;
}

void FrameComposer::setOnChunkUsedCallback(OnChunkUsed callback)
{
    onChunkUsed = callback;
}