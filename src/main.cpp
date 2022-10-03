#define MY_DEBUG 1

#include "main.h"
#include "gui.cpp"

#include "game.cpp"


internal void HandleScreenResize(Rectangle *screen, Camera2D *camera, board_info *board)
{
#define BORDER 10 
    *screen = Rec(0.0f, 0.0f, GetScreenWidth(), GetScreenHeight());

    camera->offset = Vec2(screen->width/2.0f, screen->height/2.0f),
    camera->target = Vec2(screen->width/2.0f, screen->height/2.0f),

    board->dim = Vec2(screen->height-BORDER*2, screen->height-BORDER*2);
    board->pos = Vec2((screen->width - board->dim.x)/2.0f, (screen->height - board->dim.y)/2.0f);
    board->squareSize = board->dim.x/8;

#undef BORDER
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

Rectangle RecForSquare(board_info board, sq_coord coord, bool center = false)
{
    Rectangle result = {0};
    
    int32 offset = center ? board.squareSize/2 : 0;

    result = Rec(board.pos.x+board.squareSize*coord.file+offset, board.pos.y+board.dim.y-board.squareSize-board.squareSize*coord.rank+offset, board.squareSize, board.squareSize);

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
    

    board_info board = {0};
    board.fontSize = 10;
    HandleScreenResize(&screen, &camera, &board);

    game_info game = {0};
    game.playersTurn = PLAYER_WHITE;
    game.dir = 1;
    game.playerTimer[PLAYER_WHITE] = 10.0;
    game.playerTimer[PLAYER_BLACK] = 10.0;
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
            HandleScreenResize(&screen, &camera, &board);
        }

        if(IsKeyPressed(KEY_R))
        {
            board.flipped = !board.flipped;
        }

        Update(&game, &board);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            float whiteClockW = board.squareSize * 2.0f;
            float whiteClockH = board.squareSize;
            float whiteClockX = (board.pos.x - whiteClockW) / 2.0f;
            float whiteClockY = (board.pos.y + board.dim.y) / 2.0f - whiteClockH / 2.0f;
            DrawRectangleLinesEx(Rec(whiteClockX, whiteClockY, whiteClockW, whiteClockH), 2.0f, BLACK);
            DrawTextPro(GetFontDefault(), TextFormat("%d", (int32)game.playerTimer[PLAYER_WHITE]), Vec2(whiteClockX, whiteClockY), Vec2(), 0.0f, whiteClockH, 1.0f, BLACK);

            float blackClockW = board.squareSize * 2.0f;
            float blackClockH = board.squareSize;
            float blackClockX = board.pos.x + board.dim.x + (screen.width - board.pos.x - board.dim.x - blackClockW) / 2.0f;
            float blackClockY = (board.pos.y + board.dim.y) / 2.0f - blackClockH / 2.0f;
            DrawRectangleLinesEx(Rec(blackClockX, blackClockY, blackClockW, blackClockH), 2.0f, BLACK);

            camera.rotation = board.flipped ? 180.0f : 0.0f;

            BeginMode2D(camera);

            DrawTexturePro(boardTex, Rec(0, 0, boardImage.width, boardImage.height), Rec(board.pos, board.dim), Vec2(), 0.0f, WHITE);
            DrawRectangleLinesEx(Rec(board.pos, board.dim), 1.0f, BLACK);

            if(game.draggedPiece == 0) deferedPiece = 0;

            if(game.selectedPossibleMoves != 0)
            {
                for(int32 i = 0; i < arrlen(game.selectedPossibleMoves); ++i)
                {
                    sq_coord sq = game.selectedPossibleMoves[i];
                    Rectangle sqRec = RecForSquare(board, sq);
                    DrawRectanglePro(sqRec, Vec2(), 0.0f, Fade(GREEN, 0.3f));
                }
            }

            if(game.check)
            {
                sq_coord kingSq = game.playersTurn == PLAYER_WHITE ? game.whiteKing : game.blackKing;
                Rectangle sqRec = RecForSquare(board, kingSq);
                DrawRectanglePro(sqRec, Vec2(), 0.0f, Fade(RED, 0.5f));
            }

            if(game.selectedPiece != 0)
            {
                Rectangle sqRec = RecForSquare(board, game.selectedPiece->coord);
                DrawRectangleLinesEx(sqRec, 2.0f, GREEN);
            }

            for(int32 r = 0; r < 8; ++r)
            {
                for(int32 f = 0; f < 8; ++f)
                {
                    square_info sq = game.board[r][f]; 

                    if(sq.piece == 0) continue;

                    Rectangle pieceRec = RecForSquare(board, sq.coord, true);
                    
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
                        DrawTexturePro(piecesTex, TexRecForPiece(*sq.piece, piecesTex), pieceRec, Vec2(board.squareSize/2, board.squareSize/2), board.flipped ? 180.0f : 0.0f, WHITE);
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
                        Rectangle pieceBordRec = (player_color)p == PLAYER_WHITE ? RecForSquare(board, (sq_coord){0, 8}) : RecForSquare(board, (sq_coord){7, -1});
                        if((player_color)p == PLAYER_BLACK) 
                        {
                            pieceBordRec.x += board.squareSize;
                            pieceBordRec.y += board.squareSize;
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
                        DrawTexturePro(piecesTex, pieceTexRec, pieceBordRec, Vec2(pieceBordRec.width/2.0f, pieceBordRec.height/2.0f), board.flipped ? 180.0f : 0.0f, WHITE);
                    }
                }
            }

            if(deferedPiece != 0)
            {
                TraceLog(LOG_INFO, "Defered piece is");
                DrawTexturePro(piecesTex, TexRecForPiece(*deferedPiece, piecesTex), deferedRec, Vec2(board.squareSize/2, board.squareSize/2), board.flipped ? 180.0f : 0.0f, WHITE);
                deferedPiece = 0;
            }

            EndMode2D();
            
            ///////////// DRAW FILE LETTERS AND RANK NUMBERS ON THE BOARD ///////////////////////
            const char* fileLetters[] = {"a", "b", "c", "d", "e", "f", "g", "h"};
            v2 fStart = Vector2Add(board.pos, Vec2(5.0f, board.dim.y-board.fontSize));
            int32 fIndex = 0;
            for(int32 i = board.flipped ? 7 : 0; board.flipped ? i >= 0 : i < 8; board.flipped ? --i : ++i)
            {
                DrawText(fileLetters[i], fStart.x + board.squareSize*fIndex++, fStart.y, board.fontSize, RED);
            }

            const char* rankNumbers[] = {"1", "2", "3", "4", "5", "6", "7", "8"};
            v2 rStart = Vector2Add(board.pos, Vec2(5.0f, board.dim.y-board.squareSize+1));
            int32 rlIndex = 0;
            for(int32 i = board.flipped ? 7 : 0; board.flipped ? i >= 0 : i < 8; board.flipped ? --i : ++i)
            {
                DrawText(rankNumbers[i], rStart.x, rStart.y-board.squareSize*rlIndex++, board.fontSize, RED);
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