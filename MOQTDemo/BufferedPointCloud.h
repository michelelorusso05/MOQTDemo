#pragma once

#include <mutex>
#include <cstdint>
#include <vector>

#include "raylib.h"

#pragma pack(push, 1)
struct RawPoint {
    uint16_t x, y, z;
    uint8_t r, g, b;
};
#pragma pack(pop)

/// <summary>
/// Classe per gestire il caricamento di point cloud e per la traduzione in matrici di trasformazione
/// compatibili con il render di instanced mesh.
/// </summary>
class BufferedPointCloud
{
public:
    /// <summary>
    /// Carica una point cloud partendo da un blocco binario in memoria.
    /// </summary>
    /// <param name="data">I dati da caricare. Ogni punto deve occupare esattamente 9 byte, nel formato XXYYZZRGB, partendo dal .ply.</param>
    /// <param name="size">Il numero di punti da caricare.</param>
    /// <param name="scale">La trasformazione sulla dimensione da applicare ai punti caricati.</param>
    void loadPointCloud(const uint8_t* data, uintptr_t size, float scale);

    /// <summary>
    /// Renderizza a schermo la point cloud, tramite istanced mesh.
    /// </summary>
    /// <param name="mesh">La mesh da utilizzare per rappresentare ogni punto.</param>
    /// <param name="material">Il materiale da utilizzare per ogni punto.</param>
    void render(const Mesh& mesh, const Material& material);

private:
    void swapBuffers();

    std::vector<Matrix> bufferA;
    std::vector<Matrix> bufferB;

    std::vector<Matrix>* frontBuffer = &bufferA;
    std::vector<Matrix>* backBuffer = &bufferB;

    std::mutex bufferMutex;
};

