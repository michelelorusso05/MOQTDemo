#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

#include "raylib.h"
#include "rlgl.h"

#include <cstdint>
#include <string>
#include <iostream>

#include "BufferedPointCloud.h"
#include "MOQTManager.h"
#include "FrameComposer.h"

int main(void)
{
    BufferedPointCloud bufferedPointCloud;
    MOQTManager networkManager;
    FrameComposer frameComposer;

    std::ios::sync_with_stdio(0);

    networkManager.setTrack("frames");
    networkManager.connect("https://127.0.0.1:4443");
    networkManager.waitForPublisher("moqtest/1");

    networkManager.setOnTrackFrameCallback([&](int32_t code, const uint8_t* data, size_t size, uint64_t timestamp) {
        frameComposer.enqueueFrame({ code, data, size }, timestamp);
    });

    networkManager.setOnTrackClosedCallback([&]() {
        frameComposer.closeGroup();
    });

    networkManager.setOnTrackErrorCallback([&]() {
        frameComposer.flush();
    });

    frameComposer.setOnDataReadyCallback([&](uint8_t* data, size_t size) {
        bufferedPointCloud.loadPointCloud(data, size, 0.1f);
    });

    frameComposer.setOnChunkUsedCallback([&](int32_t id) {
        networkManager.freeFrame(id);
    });

    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    SetTraceLogLevel(LOG_NONE);

    InitWindow(screenWidth, screenHeight, "Point Cloud test");

    Camera3D camera = { 0 };
    camera.position = { -30.0f, 20.0f, -30.0f };
    camera.target = { 20.0f, 20.0f, 20.0f };     
    camera.up = { 0.0f, 1.0f, 0.0f };         
    camera.fovy = 45.0f;                               
    camera.projection = CAMERA_PERSPECTIVE;            

    bool hasFocus = false;

    float scale = 0.1f;
    Mesh cubeMesh = GenMeshCube(scale, scale, scale);

    Material material = LoadMaterialDefault();
    material.shader = LoadShader("./shaders/instanced.vs", "./shaders/instanced.fs");
    material.shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(material.shader, "instanceTransform");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (networkManager.shouldResubscribe())
        {
            networkManager.attemptResubscribe();
        }

        if (!networkManager.subscribeRequested())
        {
            networkManager.waitForPublisher("moqtest/1");
        }

        if (hasFocus)
        {
            UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            DisableCursor();
            hasFocus = true;
        }

        if (IsKeyPressed(KEY_LEFT_ALT))
        {
            EnableCursor();
            hasFocus = false;
        }

        if (IsKeyPressed(KEY_Z)) camera.target = { 0.0f, 0.0f, 0.0f };
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                
                bufferedPointCloud.render(cubeMesh, material);

            EndMode3D();

        EndDrawing();
    }

    CloseWindow();

    UnloadMesh(cubeMesh);
    UnloadMaterial(material);

    return 0;
}