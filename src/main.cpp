#define MY_DEBUG 0

#include "main.h"
#include "gui.cpp"

#include "game.cpp"

#ifndef MY_DEBUG
#include "chessPiecesImg.h"
#endif

internal void HandleScreenResize(Rectangle *screen, Camera2D *camera, draw_info *drawInfo)
{
#define BOARD_BORDER 10.0f
#define CLOCK_BORDER 5.0f
    *screen = Rec(0.0f, 0.0f, GetScreenWidth(), GetScreenHeight());

    camera->offset = Vec2(screen->width/2.0f, screen->height/2.0f),
    camera->target = Vec2(screen->width/2.0f, screen->height/2.0f),

    drawInfo->boardDim = Vec2(screen->height-BOARD_BORDER*2, screen->height-BOARD_BORDER*2);
    drawInfo->boardPos = Vec2((screen->width - drawInfo->boardDim.x)/2.0f, (screen->height - drawInfo->boardDim.y)/2.0f);
    drawInfo->squareSize = drawInfo->boardDim.x/8;

    drawInfo->boardFontSize = drawInfo->squareSize / 10.f;
    if(drawInfo->boardFontSize < drawInfo->boardMinFontSize) drawInfo->boardFontSize = drawInfo->boardMinFontSize;


    drawInfo->clockFontSize = drawInfo->squareSize/2.0f;
    v2 textDim = MeasureTextEx(drawInfo->clockFont, "00:00:00", drawInfo->clockFontSize, 1.0f);
    drawInfo->whiteClockRec.width = textDim.x + 2.0f*CLOCK_BORDER;
    drawInfo->whiteClockRec.height = textDim.y + 2.0f*CLOCK_BORDER;
    drawInfo->blackClockRec.width = textDim.x + 2.0f*CLOCK_BORDER;
    drawInfo->blackClockRec.height = textDim.y + 2.0f*CLOCK_BORDER;

    drawInfo->whiteClockRec.x = (drawInfo->boardPos.x - drawInfo->blackClockRec.width) / 2.0f - CLOCK_BORDER;
    drawInfo->whiteClockRec.y = (drawInfo->boardPos.y + drawInfo->boardDim.y) / 2.0f - drawInfo->whiteClockRec.height / 2.0f - CLOCK_BORDER;
    drawInfo->blackClockRec.x = drawInfo->boardPos.x + drawInfo->boardDim.x + (screen->width - drawInfo->boardPos.x - drawInfo->boardDim.x - drawInfo->blackClockRec.width) / 2.0f - CLOCK_BORDER;
    drawInfo->blackClockRec.y = (drawInfo->boardPos.y + drawInfo->boardDim.y) / 2.0f - drawInfo->blackClockRec.height / 2.0f - CLOCK_BORDER;

    drawInfo->whiteTimeTextPos = Vector2AddValue(Vec2(drawInfo->whiteClockRec.x, drawInfo->whiteClockRec.y), CLOCK_BORDER);
    drawInfo->blackTimeTextPos = Vector2AddValue(Vec2(drawInfo->blackClockRec.x, drawInfo->blackClockRec.y), CLOCK_BORDER);

#undef BOARD_BORDER
#undef CLOCK_BORDER
}

int main(void) 
{
    // Initialization
    //-------------------------------------------------------------------------------------
    //ExportImageAsCode(LoadImage("../resources/pieces.png"), "../chessPiecesImg.h");
    
    //--------------------------------------------------------------------------------------
    draw_info drawInfo = {0};
    drawInfo.screen = {0.0f, 0.0f, 800.0f, 450.0f};
    drawInfo.camera = {
    .offset = Vec2(drawInfo.screen.width/2.0f, drawInfo.screen.height/2.0f),
    .target = Vec2(drawInfo.screen.width/2.0f, drawInfo.screen.height/2.0f),
    .rotation = 0.0f,
    .zoom = 1.0f
    };

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(drawInfo.screen.width, drawInfo.screen.height, "Chess");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    drawInfo.clockFont = GetFontDefault();
    drawInfo.boardFont = GetFontDefault();
    drawInfo.boardMinFontSize = 12.0f;

    const int32 boardImgW = 32;
    const int32 boardImgH = 32;
    Image boardImage = GenImageChecked(boardImgW, boardImgH, boardImgW/8, boardImgH/8, WHITE, DARKGRAY);
    drawInfo.boardTex = LoadTextureFromImage(boardImage);
    UnloadImage(boardImage);

    

#ifdef MY_DEBUG
    drawInfo.piecesTex = LoadTexture("../resources/pieces.png");
#else
    Image img = {
        .data = CHESSPIECESIMG_DATA,
        .width = CHESSPIECESIMG_WIDTH,
        .height = CHESSPIECESIMG_HEIGHT,
        .mipmaps = 1,
        .format = CHESSPIECESIMG_FORMAT,
    };  
    drawInfo.piecesTex = LoadTextureFromImage(img);
#endif

    HandleScreenResize(&drawInfo.screen, &drawInfo.camera, &drawInfo);

    game_info game = {0};
    game.playersTurn = PLAYER_WHITE;
    game.dir = 1;
    game.timer[PLAYER_WHITE] = 90.0;
    game.timer[PLAYER_BLACK] = 10.0;
    game.gameState = GAME_PLAY;
    SetupBoard(&game);

    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if(IsWindowResized())
        {
            HandleScreenResize(&drawInfo.screen, &drawInfo.camera, &drawInfo);
        }


        switch(game.gameState)
        {
            case GAME_SETUP:
            {

            } break;

            case GAME_PLAY:
            {
                GameUpdate(&game, &drawInfo);

            } break;

            case GAME_END:
            {

            } break;
        }

        // Draw 90
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
        ClearBackground(RAYWHITE);

        switch(game.gameState)
        {
            case GAME_SETUP:
            {

            } break;

            case GAME_PLAY:
            {
                DrawClocks(&game, &drawInfo);
                DrawBoard(&game, &drawInfo);
                
            } break;

            case GAME_END:
            {

            } break;
        }

        DrawFPS(10, 10);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}