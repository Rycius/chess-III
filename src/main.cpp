#define MY_DEBUG 1

#include "main.h"

#include "game.cpp"
#include "gui.cpp"

#ifndef MY_DEBUG
#include "chessPiecesImg.h"
#endif

internal void HandleScreenResize(draw_info *drawInfo)
{
#define BOARD_BORDER 10.0f
#define CLOCK_BORDER 5.0f
    drawInfo->screen = Rec(0.0f, 0.0f, GetScreenWidth(), GetScreenHeight());

    drawInfo->camera.offset = Vec2(drawInfo->screen.width/2.0f, drawInfo->screen.height/2.0f),
    drawInfo->camera.target = Vec2(drawInfo->screen.width/2.0f, drawInfo->screen.height/2.0f),

    drawInfo->boardDim = Vec2(drawInfo->screen.height-BOARD_BORDER*2, drawInfo->screen.height-BOARD_BORDER*2);
    drawInfo->boardPos = Vec2((drawInfo->screen.width - drawInfo->boardDim.x)/2.0f, (drawInfo->screen.height - drawInfo->boardDim.y)/2.0f);
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
    drawInfo->blackClockRec.x = drawInfo->boardPos.x + drawInfo->boardDim.x + (drawInfo->screen.width - drawInfo->boardPos.x - drawInfo->boardDim.x - drawInfo->blackClockRec.width) / 2.0f - CLOCK_BORDER;
    drawInfo->blackClockRec.y = (drawInfo->boardPos.y + drawInfo->boardDim.y) / 2.0f - drawInfo->blackClockRec.height / 2.0f - CLOCK_BORDER;

    drawInfo->whiteTimeTextPos = Vector2AddValue(Vec2(drawInfo->whiteClockRec.x, drawInfo->whiteClockRec.y), CLOCK_BORDER);
    drawInfo->blackTimeTextPos = Vector2AddValue(Vec2(drawInfo->blackClockRec.x, drawInfo->blackClockRec.y), CLOCK_BORDER);

#undef BOARD_BORDER
#undef CLOCK_BORDER

    float buttonsAreaW = drawInfo->screen.width * 0.7f;
    float buttonsAreaH = drawInfo->screen.height * 0.4f;
    float buttonsAreaX = (drawInfo->screen.width - buttonsAreaW) / 2.0f;
    float buttonsAreaY = (drawInfo->screen.height - buttonsAreaH) / 2.0f;

    float buttonX = buttonsAreaX;
    float buttonY = buttonsAreaY;
    float buttonW = (buttonsAreaW / drawInfo->guiButtonsCountHor) - ((drawInfo->guiButtonsCountHor - 1) * drawInfo->guiSpacing);
    int32 verCount = (arrlen(_buttons) / drawInfo->guiButtonsCountHor);
    float buttonH = (buttonsAreaH / verCount) - ((verCount - 1) * drawInfo->guiSpacing);

    for(int32 i = 0; i < arrlen(_buttons); ++i)
    {
        drawInfo->guiTextFontSize = buttonH / 2.0f;

        _buttons[i].rec = Rec(buttonX, buttonY, buttonW, buttonH);

        v2 textDim = MeasureTextEx(drawInfo->guiTextFont, _buttons[i].text, drawInfo->guiTextFontSize, 1.0f);
        _buttons[i].textPos = Vec2(buttonX + (buttonW - textDim.x) / 2.0f, buttonY + (buttonH - textDim.y) / 2.0f);

        buttonX += drawInfo->guiSpacing + buttonW;
        if(((i + 1) % drawInfo->guiButtonsCountHor) == 0)
        {
            buttonX = buttonsAreaX;
            buttonY += drawInfo->guiSpacing + buttonH;
        }
    }
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

    drawInfo.guiTextFont = GetFontDefault();
    drawInfo.guiButtonsCountHor = 4;
    drawInfo.guiSpacing = 5;

#ifdef MY_DEBUG
    drawInfo.piecesTex = LoadTexture("../resources/pieces.png");
#else
    MaximizeWindow();

    Image img = {
        .data = CHESSPIECESIMG_DATA,
        .width = CHESSPIECESIMG_WIDTH,
        .height = CHESSPIECESIMG_HEIGHT,
        .mipmaps = 1,
        .format = CHESSPIECESIMG_FORMAT,
    };  
    drawInfo.piecesTex = LoadTextureFromImage(img);
#endif


    game_info game = {0};
    game.playersTurn = PLAYER_WHITE;
    game.dir = 1;
    game.timer[PLAYER_WHITE] = 90.0;
    game.timer[PLAYER_BLACK] = 10.0;
    game.state = GAME_SETUP;

    GUISetup(&game, &drawInfo);
    SetupBoard(&game);

    HandleScreenResize(&drawInfo);
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if(IsWindowResized())
        {
            HandleScreenResize(&drawInfo);
        }


        switch(game.state)
        {
            case GAME_SETUP:
            {
                GUIUpdate(&game);
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

        switch(game.state)
        {
            case GAME_SETUP:
            {
                DrawGUI(&drawInfo);
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

        //DrawFPS(10, 10);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}