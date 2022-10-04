#define MY_DEBUG 1

#include "main.h"
#include "gui.cpp"

#include "game.cpp"


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


internal Rectangle TexRecForPiece(piece_info piece, Texture2D tex)
{
    Rectangle result = {0};

    result.width = tex.width / 6;
    result.height = tex.height / 2;

    result.y = (piece.owner == PLAYER_WHITE) ? tex.height / 2 : 0;

    switch (piece.type)
    {
    case ROOK:
    {
        result.x = 0;    
    } break;
    case BISHOP:
    {
        result.x = result.width;    
    } break;
    case QUEEN:
    {
        result.x = result.width * 2;    
    } break;
    case KING:
    {
        result.x = result.width * 3;    
    } break;
    case KNIGHT:
    {
        result.x = result.width * 4;    
    } break;
    case PAWN:
    {
        result.x = result.width * 5;  
    } break;
    
    default:
        result = Rec(0, 0, 0, 0);
        break;
    }

    return result;
}

Rectangle RecForSquare(draw_info board, sq_coord coord, bool center = false)
{
    Rectangle result = {0};
    
    int32 offset = center ? board.squareSize/2 : 0;

    result = Rec(board.boardPos.x+board.squareSize*coord.file+offset, board.boardPos.y+board.boardDim.y-board.squareSize-board.squareSize*coord.rank+offset, board.squareSize, board.squareSize);

    return result;
}

int main(void) 
{
    // Initialization
    //-------------------------------------------------------------------------------------
    Rectangle screen = {0.0f, 0.0f, 800.0f, 450.0f};

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen.width, screen.height, "raylib");

    Camera2D camera = {
        .offset = Vec2(screen.width/2.0f, screen.height/2.0f),
        .target = Vec2(screen.width/2.0f, screen.height/2.0f),
        .rotation = 0.0f,
        .zoom = 1.0f
    };

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    const int32 boardImgW = 32;
    const int32 boardImgH = 32;
    Image boardImage = GenImageChecked(boardImgW, boardImgH, boardImgW/8, boardImgH/8, WHITE, DARKGRAY);
    Texture2D boardTex = LoadTextureFromImage(boardImage);
    UnloadImage(boardImage);

    Texture2D piecesTex = LoadTexture("../resources/pieces.png");
    

    draw_info drawInfo = {0};
    drawInfo.boardFontSize = 10;
    drawInfo.clockFont = GetFontDefault();
    HandleScreenResize(&screen, &camera, &drawInfo);

    game_info game = {0};
    game.playersTurn = PLAYER_WHITE;
    game.dir = 1;
    game.timer[PLAYER_WHITE] = 90.0;
    game.timer[PLAYER_BLACK] = 10.0;
    SetupBoard(&game);

    Rectangle deferedRec;
    piece_info *deferedPiece;


    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if(IsWindowResized())
        {
            HandleScreenResize(&screen, &camera, &drawInfo);
        }

        if(IsKeyPressed(KEY_R))
        {
            drawInfo.flipped = !drawInfo.flipped;
        }

        GameUpdate(&game, &drawInfo);

        // Draw 90
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawRectangleLinesEx(drawInfo.whiteClockRec, 2.0f, BLACK);
            int32 whiteHours = (int32)floor(game.timer[PLAYER_WHITE]/60.0f/60.0f);
            int32 whiteMinutes = (int32)floor(game.timer[PLAYER_WHITE]/60.0f);
            int32 whiteSeconds = (int32)game.timer[PLAYER_WHITE] % 60;
            const char *whiteTimeText = TextFormat("%.2d:%.2d:%.2d", whiteHours, whiteMinutes, whiteSeconds);
            DrawTextPro(drawInfo.clockFont, whiteTimeText, drawInfo.whiteTimeTextPos, Vec2(), 0.0f, drawInfo.clockFontSize, 1.0f, BLACK);

            DrawRectanglePro(drawInfo.blackClockRec, Vec2(), 0.0f, BLACK);
            int32 blackHours = (int32)floor(game.timer[PLAYER_BLACK]/60.0f/60.0f);
            int32 blackMinutes = (int32)floor(game.timer[PLAYER_BLACK]/60.0f);
            int32 blackSeconds = (int32)game.timer[PLAYER_BLACK] % 60;
            const char *blackTimeText = TextFormat("%.2d:%.2d:%.2d", blackHours, blackMinutes, blackSeconds);
            DrawTextPro(drawInfo.clockFont, blackTimeText, drawInfo.blackTimeTextPos, Vec2(), 0.0f, drawInfo.clockFontSize, 1.0f, WHITE);

            
            camera.rotation = drawInfo.flipped ? 180.0f : 0.0f;

            BeginMode2D(camera);

            DrawTexturePro(boardTex, Rec(0, 0, boardImage.width, boardImage.height), Rec(drawInfo.boardPos, drawInfo.boardDim), Vec2(), 0.0f, WHITE);
            DrawRectangleLinesEx(Rec(drawInfo.boardPos, drawInfo.boardDim), 1.0f, BLACK);

            if(game.draggedPiece == 0) deferedPiece = 0;

            if(game.selectedPossibleMoves != 0)
            {
                for(int32 i = 0; i < arrlen(game.selectedPossibleMoves); ++i)
                {
                    sq_coord sq = game.selectedPossibleMoves[i];
                    Rectangle sqRec = RecForSquare(drawInfo, sq);
                    DrawRectanglePro(sqRec, Vec2(), 0.0f, Fade(GREEN, 0.3f));
                }
            }

            if(game.check)
            {
                sq_coord kingSq = game.playersTurn == PLAYER_WHITE ? game.whiteKing : game.blackKing;
                Rectangle sqRec = RecForSquare(drawInfo, kingSq);
                DrawRectanglePro(sqRec, Vec2(), 0.0f, Fade(RED, 0.5f));
            }

            if(game.selectedPiece != 0)
            {
                Rectangle sqRec = RecForSquare(drawInfo, game.selectedPiece->coord);
                DrawRectangleLinesEx(sqRec, 2.0f, GREEN);
            }

            for(int32 r = 0; r < 8; ++r)
            {
                for(int32 f = 0; f < 8; ++f)
                {
                    square_info sq = game.board[r][f]; 

                    if(sq.piece == 0) continue;

                    Rectangle pieceRec = RecForSquare(drawInfo, sq.coord, true);
                    
                    if(sq.piece->isDragged)
                    {
                        v2 mw = GetScreenToWorld2D(GetMousePosition(), camera);
                        pieceRec.x = mw.x;
                        pieceRec.y = mw.y;

                        deferedRec = pieceRec;
                        deferedPiece = sq.piece;
                    }
                    else
                    {
                        DrawTexturePro(piecesTex, TexRecForPiece(*sq.piece, piecesTex), pieceRec, Vec2(drawInfo.squareSize/2, drawInfo.squareSize/2), drawInfo.flipped ? 180.0f : 0.0f, WHITE);
                    }
                }
            }

            for(int32 p = 1; p < 3; ++p)
            {
                piece_info *takenPieces = game.piecesTaken[p];

                if(arrlen(takenPieces) > 0)
                {
                    int32 row = 0;
                    int32 col = 0;
                    for(int32 i = 0; i < arrlen(takenPieces); ++i)
                    {
                        int32 dir = (player_color)p == PLAYER_WHITE ? 1 : -1;
                        Rectangle pieceTexRec = TexRecForPiece(takenPieces[i], piecesTex);
                        Rectangle pieceBordRec = (player_color)p == PLAYER_WHITE ? RecForSquare(drawInfo, (sq_coord){0, 8}) : RecForSquare(drawInfo, (sq_coord){7, -1});
                        if((player_color)p == PLAYER_BLACK) 
                        {
                            pieceBordRec.x += drawInfo.squareSize;
                            pieceBordRec.y += drawInfo.squareSize;
                        }
                        pieceBordRec.width /= 3.0f;
                        pieceBordRec.height /= 3.0f;
                        pieceBordRec.x += dir * pieceBordRec.width/2.0f;
                        if(pieceBordRec.x + dir * col * pieceBordRec.width > screen.width || pieceBordRec.x + dir * col * pieceBordRec.width < 0) 
                        {
                            row++;
                            col = 0;
                        }
                        pieceBordRec.x += dir * col * pieceBordRec.width;
                        pieceBordRec.y += dir * row * pieceBordRec.height + dir * pieceBordRec.height/2.0f;
                        
                        col++;
                        DrawTexturePro(piecesTex, pieceTexRec, pieceBordRec, Vec2(pieceBordRec.width/2.0f, pieceBordRec.height/2.0f), drawInfo.flipped ? 180.0f : 0.0f, WHITE);
                    }
                }
            }

            if(deferedPiece != 0)
            {
                TraceLog(LOG_INFO, "Defered piece is");
                DrawTexturePro(piecesTex, TexRecForPiece(*deferedPiece, piecesTex), deferedRec, Vec2(drawInfo.squareSize/2, drawInfo.squareSize/2), drawInfo.flipped ? 180.0f : 0.0f, WHITE);
                deferedPiece = 0;
            }

            EndMode2D();
            
            ///////////// DRAW FILE LETTERS AND RANK NUMBERS ON THE BOARD ///////////////////////
            const char* fileLetters[] = {"a", "b", "c", "d", "e", "f", "g", "h"};
            v2 fStart = Vector2Add(drawInfo.boardPos, Vec2(5.0f, drawInfo.boardDim.y-drawInfo.boardFontSize));
            int32 fIndex = 0;
            for(int32 i = drawInfo.flipped ? 7 : 0; drawInfo.flipped ? i >= 0 : i < 8; drawInfo.flipped ? --i : ++i)
            {
                DrawText(fileLetters[i], fStart.x + drawInfo.squareSize*fIndex++, fStart.y, drawInfo.boardFontSize, RED);
            }

            const char* rankNumbers[] = {"1", "2", "3", "4", "5", "6", "7", "8"};
            v2 rStart = Vector2Add(drawInfo.boardPos, Vec2(5.0f, drawInfo.boardDim.y-drawInfo.squareSize+1));
            int32 rlIndex = 0;
            for(int32 i = drawInfo.flipped ? 7 : 0; drawInfo.flipped ? i >= 0 : i < 8; drawInfo.flipped ? --i : ++i)
            {
                DrawText(rankNumbers[i], rStart.x, rStart.y-drawInfo.squareSize*rlIndex++, drawInfo.boardFontSize, RED);
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