#define MY_DEBUG 1

#include "main.h"
#include "gui.cpp"

int main() 
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int32 screenWidth = 800;
    const int32 screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib");

    Camera camera = { 0 };
    camera.position = Vec3(10.0f, 10.0f, 8.0f);
    camera.target = Vec3(0.0f, 0.0f, 0.0f);
    camera.up = Vec3(0.0f, 1.0f, 0.0f);
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    SetCameraMode(camera, CAMERA_FREE);

    v3 cubePosition = { 0 };

    int32 spinnerVal = 5;
    bool spinnerEditMode = false;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, BLUE);
                DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);
                DrawGrid(10, 1.0f);

            EndMode3D();
            
            DrawFPS(10, 10);

            if(GUISpinner(Rec(10, 20, 100, 30), 0, &spinnerVal, -5, 5, &spinnerEditMode))
            {
                TraceLog(LOG_INFO, "Spinner Value: %i", spinnerVal);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}