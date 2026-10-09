#include "BufferedPointCloud.h"

void BufferedPointCloud::loadPointCloud(const uint8_t* data, uintptr_t size, float scale)
{
    size_t pointCount = size / sizeof(RawPoint);

    backBuffer->clear();
    backBuffer->reserve(pointCount);

    for (size_t ptr = 0; ptr < size; ptr += sizeof(RawPoint))
    {
        RawPoint* pt = (RawPoint*)(data + ptr);
        Matrix m = { 0 };
        m.m0 = 1.0f; m.m5 = 1.0f; m.m10 = 1.0f; m.m15 = 1.0f;

        // Y e Z sono invertite
        m.m12 = (float)pt->x * scale;
        m.m13 = (float)pt->z * scale;
        m.m14 = (float)pt->y * scale;

        m.m3 = pt->r / 255.0f;
        m.m7 = pt->g / 255.0f;
        m.m11 = pt->b / 255.0f;

        backBuffer->push_back(m);
    }

    BufferedPointCloud::swapBuffers();
}

void BufferedPointCloud::swapBuffers()
{
    std::lock_guard<std::mutex> lock(bufferMutex);
    std::swap(frontBuffer, backBuffer);
}

void BufferedPointCloud::render(const Mesh& mesh, const Material& material)
{
    std::lock_guard<std::mutex> lock(bufferMutex);
    DrawMeshInstanced(mesh, material, frontBuffer->data(), frontBuffer->size());
}