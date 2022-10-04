#include "game.h"

sq_coord *queenPatern = 0;
sq_coord *rookPatern = 0;
sq_coord *bishopPatern = 0;
sq_coord *knightPatern = 0;



internal void SetupBoard(game_info *game)
{
    for(int32 r = 0; r < 8; ++r)
    {
        for(int32 f = 0; f < 8; ++f)
        {
            game->board[r][f].coord.file = f;
            game->board[r][f].coord.rank = r; 

            if(r == 0 || r == 1 || r == 6 || r == 7)
                game->board[r][f].piece = (piece_info *)calloc(1, sizeof(piece_info));
        }
    }

    *game->board[0][0].piece = (piece_info){.owner = PLAYER_WHITE, .type = ROOK, .coord = (sq_coord){0, 0}};
    *game->board[0][1].piece = (piece_info){.owner = PLAYER_WHITE, .type = KNIGHT, .coord = (sq_coord){0, 1}};
    *game->board[0][2].piece = (piece_info){.owner = PLAYER_WHITE, .type = BISHOP, .coord = (sq_coord){0, 2}};
    *game->board[0][3].piece = (piece_info){.owner = PLAYER_WHITE, .type = QUEEN, .coord = (sq_coord){0, 3}};
    *game->board[0][4].piece = (piece_info){.owner = PLAYER_WHITE, .type = KING, .coord = (sq_coord){0, 4}};
    *game->board[0][5].piece = (piece_info){.owner = PLAYER_WHITE, .type = BISHOP, .coord = (sq_coord){0, 5}};
    *game->board[0][6].piece = (piece_info){.owner = PLAYER_WHITE, .type = KNIGHT, .coord = (sq_coord){0, 6}};
    *game->board[0][7].piece = (piece_info){.owner = PLAYER_WHITE, .type = ROOK, .coord = (sq_coord){0, 7}};

    game->whiteKing = (sq_coord){0, 4};

    for(int32 f = 0; f < 8; ++f)
    {
        *game->board[1][f].piece = (piece_info){.owner = PLAYER_WHITE, .type = PAWN, .coord = (sq_coord){1, f}};
    }

    *game->board[7][0].piece = (piece_info){.owner = PLAYER_BLACK, .type = ROOK, .coord = (sq_coord){7, 0}};
    *game->board[7][1].piece = (piece_info){.owner = PLAYER_BLACK, .type = KNIGHT, .coord = (sq_coord){7, 1}};
    *game->board[7][2].piece = (piece_info){.owner = PLAYER_BLACK, .type = BISHOP, .coord = (sq_coord){7, 2}};
    *game->board[7][3].piece = (piece_info){.owner = PLAYER_BLACK, .type = QUEEN, .coord = (sq_coord){7, 3}};
    *game->board[7][4].piece = (piece_info){.owner = PLAYER_BLACK, .type = KING, .coord = (sq_coord){7, 4}};
    *game->board[7][5].piece = (piece_info){.owner = PLAYER_BLACK, .type = BISHOP, .coord = (sq_coord){7, 5}};
    *game->board[7][6].piece = (piece_info){.owner = PLAYER_BLACK, .type = KNIGHT, .coord = (sq_coord){7, 6}};
    *game->board[7][7].piece = (piece_info){.owner = PLAYER_BLACK, .type = ROOK, .coord = (sq_coord){7, 7}};

    game->blackKing = (sq_coord){7, 4};

    for(int32 f = 0; f < 8; ++f)
    {
        *game->board[6][f].piece = (piece_info){.owner = PLAYER_BLACK, .type = PAWN, .coord = (sq_coord){6, f}};
    }

    sq_coord qp[8] = {(sq_coord){1, 0}, (sq_coord){-1, 0}, (sq_coord){0, 1}, (sq_coord){0, -1}, (sq_coord){1, -1}, (sq_coord){1, 1}, (sq_coord){-1, -1}, (sq_coord){-1, 1}};
    for(int32 i = 0; i < ArrayCount(qp); ++i)
    {
        arrpush(queenPatern, qp[i]);
    }

    sq_coord rp[4] = {(sq_coord){1, 0}, (sq_coord){-1, 0}, (sq_coord){0, 1}, (sq_coord){0, -1}};
    for(int32 i = 0; i < ArrayCount(rp); ++i)
    {
        arrpush(rookPatern, rp[i]);
    }

    sq_coord bp[4] = {(sq_coord){1, -1}, (sq_coord){1, 1}, (sq_coord){-1, -1}, (sq_coord){-1, 1}};
    for(int32 i = 0; i < ArrayCount(bp); ++i)
    {
        arrpush(bishopPatern, bp[i]);
    }

    sq_coord knp[8] = {(sq_coord){2, -1}, (sq_coord){2, 1}, (sq_coord){-2, -1}, (sq_coord){-2, 1}, (sq_coord){1, 2}, (sq_coord){-1, 2}, (sq_coord){1, -2}, (sq_coord){-1, -2}};
    for(int32 i = 0; i < ArrayCount(knp); ++i)
    {
        arrpush(knightPatern, knp[i]);
    }

}

internal bool CoordEqual(sq_coord a, sq_coord b)
{
    return (a.rank == b.rank && a.file == b.file);
}

internal bool CoordInArr(sq_coord *arr, sq_coord coord)
{
    bool result = false;

    for(int32 i = 0; i < arrlen(arr); ++i)
    {
        if(arr[i].rank == coord.rank && arr[i].file == coord.file)
        {
            result = true;
            break;
        }
    }

    return result;
}

internal inline bool CoordIsValid(sq_coord coord)
{
    return (coord.file >= 0 && coord.file < 8 && coord.rank >= 0 && coord.rank < 8);
}

internal bool KingIsChecked(square_info board[8][8], sq_coord kingCoord)
{
    Assert(board[kingCoord.rank][kingCoord.file].piece->type == KING);

    bool result = false;


    player_color kingColor = board[kingCoord.rank][kingCoord.file].piece->owner;
    int32 dir = kingColor == PLAYER_WHITE ? 1 : -1;
    sq_coord *patern = queenPatern;

    sq_coord pawns[2] = {(sq_coord){kingCoord.rank+1*dir, kingCoord.file-1}, (sq_coord){kingCoord.rank+1*dir, kingCoord.file+1}};
    for(int32 i = 0; i < 2; ++i)
    {
        if(CoordIsValid(pawns[i]))
        {
            piece_info *piece = board[pawns[i].rank][pawns[i].file].piece;
            if(piece != 0 && piece->type == PAWN && piece->owner != kingColor)
            {
                result = true;
                goto END;
            }
        }
    }

    for(int32 i = 0; i < arrlen(patern); ++i)
    {
        sq_coord coord = (sq_coord){kingCoord.rank+patern[i].rank, kingCoord.file+patern[i].file};
        if(CoordIsValid(coord) == false) continue;

        piece_info *piece = board[coord.rank][coord.file].piece;
        if(piece == 0) continue;

        if(piece->type == KING)
        {
            result = true;
            goto END;
        }
    }

    patern = bishopPatern;
    for(int32 i = 0; i < arrlen(patern); ++i)
    {
        int32 sqCount = 1;
        sq_coord coord = (sq_coord){kingCoord.rank+patern[i].rank*sqCount, kingCoord.file+patern[i].file*sqCount};

        while(CoordIsValid(coord))
        {
            piece_info *piece = board[coord.rank][coord.file].piece;
            if(piece != 0)
            {
                if(piece->owner != kingColor)
                {
                    if(piece->type == BISHOP || piece->type == QUEEN)
                    {
                        result = true;
                        goto END;
                    }
                    else break;
                }
                else break;
            }

            sqCount++;
            coord = (sq_coord){kingCoord.rank+patern[i].rank*sqCount, kingCoord.file+patern[i].file*sqCount};
        }
    }

    patern = rookPatern;
    for(int32 i = 0; i < arrlen(patern); ++i)
    {
        int32 sqCount = 1;
        sq_coord coord = (sq_coord){kingCoord.rank+patern[i].rank*sqCount, kingCoord.file+patern[i].file*sqCount};

        while(CoordIsValid(coord))
        {
            piece_info *piece = board[coord.rank][coord.file].piece;
            if(piece != 0)
            {
                if(piece->owner != kingColor)
                {
                    if(piece->type == ROOK || piece->type == QUEEN)
                    {
                        result = true;
                        goto END;
                    }
                    else break;
                }
                else break;
            }

            sqCount++;
            coord = (sq_coord){kingCoord.rank+patern[i].rank*sqCount, kingCoord.file+patern[i].file*sqCount};
        }
    }

    patern = knightPatern;
    for(int32 i = 0; i < arrlen(patern); ++i)
    {
        sq_coord coord = (sq_coord){kingCoord.rank+patern[i].rank, kingCoord.file+patern[i].file};
        if(CoordIsValid(coord) == false) continue;

        piece_info *piece = board[coord.rank][coord.file].piece;
        if(piece == 0) continue;

        if(piece->type == KNIGHT && piece->owner != kingColor)
        {
            result = true;
            goto END;
        }
    }

    END:
    return result;
}

internal bool MoveIsLegal(game_info *game, sq_coord from, sq_coord to)
{
    Assert(game->board[from.rank][from.file].piece != 0);

    bool result = false;

    square_info board[8][8] = {0};
    memcpy(board, game->board, sizeof(game->board));

    sq_coord kingsCoord = board[from.rank][from.file].piece->owner == PLAYER_WHITE ? game->whiteKing : game->blackKing;
    if(board[from.rank][from.file].piece->type == KING) kingsCoord = board[to.rank][to.file].coord;

    board[to.rank][to.file].piece = board[from.rank][from.file].piece;
    board[from.rank][from.file].piece = 0;


    result = !KingIsChecked(board, kingsCoord);

    return result;
}

internal sq_coord *CalcPossibleMoves(game_info *game, piece_info *piece)
{
    Assert(piece != 0);

    sq_coord *result = 0;

    switch(piece->type)
    {
        case PAWN:
        {
            //////// MOVING
            sq_coord one = (sq_coord){piece->coord.rank+1*game->dir, piece->coord.file};
            if(game->board[one.rank][one.file].piece == 0 && MoveIsLegal(game, piece->coord, one))
            {
                arrpush(result, one);

                int32 startRank = game->playersTurn == PLAYER_WHITE ? 1 : 6;
                sq_coord two = (sq_coord){piece->coord.rank+2*game->dir, piece->coord.file};
                if(piece->coord.rank == startRank && game->board[two.rank][two.file].piece == 0 && MoveIsLegal(game, piece->coord, two)) arrpush(result, two);
            }

            //////// TAKING
            sq_coord left = (sq_coord){piece->coord.rank+1*game->dir, piece->coord.file-1};
            if(CoordIsValid(left))
            {
                piece_info *pl = game->board[left.rank][left.file].piece;
                if(MoveIsLegal(game, piece->coord, left))
                {
                    if((pl != 0 && pl->owner != game->playersTurn) || (game->elPeasant && CoordEqual(game->elPeasantSq, left))) arrpush(result, left);
                }
            }

            sq_coord right = (sq_coord){piece->coord.rank+1*game->dir, piece->coord.file+1};
            if(CoordIsValid(right))
            {
                piece_info *pr = game->board[right.rank][right.file].piece;
                if(MoveIsLegal(game, piece->coord, right))
                {
                    if((pr != 0 && pr->owner != game->playersTurn) || (game->elPeasant && CoordEqual(game->elPeasantSq, right))) arrpush(result, right);
                }
            }

        }   break;

        case QUEEN:
        case BISHOP:
        case ROOK:
        {
            sq_coord *patern = queenPatern;
            if(piece->type == BISHOP) patern = bishopPatern;
            else if(piece->type == ROOK) patern = rookPatern;

            for(int32 i = 0; i < arrlen(patern); ++i)
            {
                int32 sqCount = 1;
                sq_coord coord = (sq_coord){piece->coord.rank+patern[i].rank*sqCount, piece->coord.file+patern[i].file*sqCount};
                while(CoordIsValid(coord))
                {
                    square_info sq = game->board[coord.rank][coord.file];
                    if(sq.piece != 0)
                    {
                        if(sq.piece->owner == game->playersTurn) break;

                        if(MoveIsLegal(game, piece->coord, coord)) arrpush(result, coord);
                        break;
                    }
                    else if(MoveIsLegal(game, piece->coord, coord))
                    {
                        arrpush(result, coord);
                    }
                    sqCount++;
                    coord = (sq_coord){piece->coord.rank+patern[i].rank*sqCount, piece->coord.file+patern[i].file*sqCount};
                }
            }

        }   break;

        case KNIGHT:
        {
            for(int32 i = 0; i < arrlen(knightPatern); ++i)
            {
                sq_coord coord = (sq_coord){piece->coord.rank+knightPatern[i].rank, piece->coord.file+knightPatern[i].file};
                if(CoordIsValid(coord) == false) continue;

                square_info sq = game->board[coord.rank][coord.file];

                if(sq.piece == 0 && MoveIsLegal(game, piece->coord, coord)) arrpush(result, coord);
                else if(sq.piece != 0 && sq.piece->owner != game->playersTurn && MoveIsLegal(game, piece->coord, coord)) arrpush(result, coord);
            }

        }   break;

        case KING:
        {
            for(int32 i = 0; i < arrlen(queenPatern); ++i)
            {
                sq_coord coord = (sq_coord){piece->coord.rank+queenPatern[i].rank, piece->coord.file+queenPatern[i].file};
                if(CoordIsValid(coord) == false) continue;

                square_info sq = game->board[coord.rank][coord.file];

                if(sq.piece == 0 && MoveIsLegal(game, piece->coord, coord)) arrpush(result, coord);
                else if(sq.piece != 0 && sq.piece->owner != game->playersTurn && MoveIsLegal(game, piece->coord, coord)) arrpush(result, coord);
            }

            if(piece->hasMoved == false)
            {
                // castiling short
                bool bishopEmpty = game->board[piece->coord.rank][piece->coord.file+1].piece == 0 ? true : false;
                bool knightEmpty = game->board[piece->coord.rank][piece->coord.file+2].piece == 0 ? true : false;
                piece_info *rook = game->board[piece->coord.rank][piece->coord.file+3].piece;
                bool rookCheck = (rook != 0 && rook->hasMoved == false) ? true : false;
                

                if(bishopEmpty && knightEmpty && rookCheck && MoveIsLegal(game, piece->coord, (sq_coord){piece->coord.rank, piece->coord.file+1}) && MoveIsLegal(game, piece->coord, (sq_coord){piece->coord.rank, piece->coord.file+2})) 
                    arrpush(result, ((sq_coord){piece->coord.rank, piece->coord.file+2}));

                // castiling long
                bool queenEmpty = game->board[piece->coord.rank][piece->coord.file-1].piece == 0 ? true : false;
                bishopEmpty = game->board[piece->coord.rank][piece->coord.file-2].piece == 0 ? true : false;
                knightEmpty = game->board[piece->coord.rank][piece->coord.file-3].piece == 0 ? true : false;
                rook = game->board[piece->coord.rank][piece->coord.file-4].piece;
                rookCheck = (rook != 0 && rook->hasMoved == false) ? true : false;

                if(queenEmpty && bishopEmpty && knightEmpty && rookCheck && MoveIsLegal(game, piece->coord, (sq_coord){piece->coord.rank, piece->coord.file-1}) && MoveIsLegal(game, piece->coord, (sq_coord){piece->coord.rank, piece->coord.file-2})) 
                    arrpush(result, ((sq_coord){piece->coord.rank, piece->coord.file-2}));
            }

        }   break;

        default: 
            break;
    }

    return result;
}


internal void GameUpdate(game_info *game, draw_info *drawInfo)
{

    if(IsKeyPressed(KEY_R))
    {
        drawInfo->flipped = !drawInfo->flipped;
    }
    drawInfo->camera.rotation = drawInfo->flipped ? 180.0f : 0.0f;

    square_info *squareUnderMouse = 0;

    {
        v2 boardStart = Vec2(drawInfo->flipped ? drawInfo->boardPos.x+drawInfo->boardDim.x : drawInfo->boardPos.x, drawInfo->flipped ? drawInfo->boardPos.y : drawInfo->boardPos.y + drawInfo->boardDim.y);
        v2 delta = Vector2Subtract(GetMousePosition(), boardStart);
        int32 file = floor(abs(delta.x) / drawInfo->squareSize);
        int32 rank = floor(abs(delta.y) / drawInfo->squareSize);

        if(file >= 0 && file < 8 && rank >= 0 && rank < 8) squareUnderMouse = &game->board[rank][file];
    }

    game->timer[game->playersTurn] -= GetFrameTime();
    if(game->timer[game->playersTurn] <= 0.0)
    {
        game->timer[game->playersTurn] = 0.0;
        // GAME OVER
    }
    game->check = KingIsChecked(game->board, game->playersTurn == PLAYER_WHITE ? game->whiteKing : game->blackKing);
    if(game->elPeasant == game->playersTurn) game->elPeasant = PLAYER_NONE;
    ///////////////////// PIECE SELECTION AND DRAGING ///////////////////////
    
    if(game->selectedPiece != 0 && ((game->draggedPiece != 0 && IsMouseButtonReleased(0)) || IsMouseButtonPressed(0)))
    {
        if(squareUnderMouse != 0)
        {
            if(CoordInArr(game->selectedPossibleMoves, squareUnderMouse->coord))
            {
                if(game->selectedPiece->type == PAWN && (abs(game->selectedPiece->coord.rank - squareUnderMouse->coord.rank) == 2))
                {
                    game->elPeasant = game->playersTurn;
                    game->elPeasantSq = game->selectedPiece->coord;
                    game->elPeasantSq.rank += game->dir;
                }

                if(game->selectedPiece->type == KING && (abs(game->selectedPiece->coord.file - squareUnderMouse->coord.file) == 2))
                {
                    int32 castilingSide = squareUnderMouse->coord.file - game->selectedPiece->coord.file;
                    piece_info *rook = castilingSide > 0 ? game->board[game->selectedPiece->coord.rank][game->selectedPiece->coord.file+3].piece : game->board[game->selectedPiece->coord.rank][game->selectedPiece->coord.file-4].piece;
                    game->board[rook->coord.rank][rook->coord.file].piece = 0;
                    rook->coord.file = castilingSide > 0 ? 5 : 3;
                    game->board[rook->coord.rank][rook->coord.file].piece = rook;

                }
                game->board[game->selectedPiece->coord.rank][game->selectedPiece->coord.file].piece = 0;
                piece_info *pieceTaken = game->board[squareUnderMouse->coord.rank][squareUnderMouse->coord.file].piece;
                if(game->elPeasant && CoordEqual(squareUnderMouse->coord, game->elPeasantSq))
                {
                    sq_coord take = game->elPeasantSq;
                    take.rank += game->dir*-1;
                    pieceTaken = game->board[take.rank][take.file].piece;
                }

                if(pieceTaken != 0)
                {
                    arrpush(game->piecesTaken[game->playersTurn], *pieceTaken);
                    sq_coord clean = pieceTaken->coord;
                    free(pieceTaken);
                    game->board[clean.rank][clean.file].piece = 0;
                }
                game->board[squareUnderMouse->coord.rank][squareUnderMouse->coord.file].piece = game->selectedPiece;
                game->selectedPiece->coord = squareUnderMouse->coord;
                game->selectedPiece->hasMoved = true;
                game->selectedPiece->isDragged = false;

                if(game->selectedPiece->type == KING)
                {
                    game->playersTurn == PLAYER_WHITE ? (game->whiteKing = squareUnderMouse->coord) : (game->blackKing = squareUnderMouse->coord);
                }

                game->draggedPiece = 0;
                game->selectedPiece = 0;
                game->playersTurn = game->playersTurn == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
                //bi->flipped = game->playersTurn == PLAYER_WHITE ? false : true;
                game->dir *= -1;
                
                arrfree(game->selectedPossibleMoves);
            }
            else //if(squareUnderMouse->coord.rank == game->selectedPiece->coord.rank && squareUnderMouse->coord.file == game->selectedPiece->coord.file)
            {
                game->selectedPiece->isDragged = false;
                game->draggedPiece = 0;
            }

        }
        else
        {
            game->selectedPiece->isDragged = false;
            game->draggedPiece = 0;
        }
    }

    if(squareUnderMouse != 0 && squareUnderMouse->piece != 0 && squareUnderMouse->piece->owner == game->playersTurn)
    {
        if(IsMouseButtonPressed(0))
        {
            game->selectedPiece = squareUnderMouse->piece;
            if(game->selectedPossibleMoves != 0) arrfree(game->selectedPossibleMoves);
            game->selectedPossibleMoves = CalcPossibleMoves(game, game->selectedPiece);
        }

        if(IsMouseButtonDown(0) && game->selectedPiece != 0 && game->draggedPiece == 0)
        {
            squareUnderMouse->piece->isDragged = true;
            game->draggedPiece = game->selectedPiece;
        }
    }
    
#if MY_DEBUG
    if(IsKeyPressed(KEY_S)) game->playersTurn = game->playersTurn == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
#endif 
}

void DrawClocks(game_info *game, draw_info *drawInfo)
{
    DrawRectangleLinesEx(drawInfo->whiteClockRec, 2.0f, BLACK);
    int32 whiteHours = (int32)floor(game->timer[PLAYER_WHITE]/60.0f/60.0f);
    int32 whiteMinutes = (int32)floor(game->timer[PLAYER_WHITE]/60.0f);
    int32 whiteSeconds = (int32)game->timer[PLAYER_WHITE] % 60;
    const char *whiteTimeText = TextFormat("%.2d:%.2d:%.2d", whiteHours, whiteMinutes, whiteSeconds);
    DrawTextPro(drawInfo->clockFont, whiteTimeText, drawInfo->whiteTimeTextPos, Vec2(), 0.0f, drawInfo->clockFontSize, 1.0f, BLACK);

    DrawRectanglePro(drawInfo->blackClockRec, Vec2(), 0.0f, BLACK);
    int32 blackHours = (int32)floor(game->timer[PLAYER_BLACK]/60.0f/60.0f);
    int32 blackMinutes = (int32)floor(game->timer[PLAYER_BLACK]/60.0f);
    int32 blackSeconds = (int32)game->timer[PLAYER_BLACK] % 60;
    const char *blackTimeText = TextFormat("%.2d:%.2d:%.2d", blackHours, blackMinutes, blackSeconds);
    DrawTextPro(drawInfo->clockFont, blackTimeText, drawInfo->blackTimeTextPos, Vec2(), 0.0f, drawInfo->clockFontSize, 1.0f, WHITE);
}

Rectangle RecForSquare(draw_info *drawInfo, sq_coord coord, bool center = false)
{
    Rectangle result = {0};
    
    int32 offset = center ? drawInfo->squareSize/2 : 0;

    result = Rec(drawInfo->boardPos.x+drawInfo->squareSize*coord.file+offset, drawInfo->boardPos.y+drawInfo->boardDim.y-drawInfo->squareSize-drawInfo->squareSize*coord.rank+offset, drawInfo->squareSize, drawInfo->squareSize);

    return result;
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

Rectangle _deferedRec;
piece_info *_deferedPiece;

void DrawBoard(game_info *game, draw_info *drawInfo)
{
    BeginMode2D(drawInfo->camera);

    DrawTexturePro(drawInfo->boardTex, Rec(0, 0, drawInfo->boardTex.width, drawInfo->boardTex.height), Rec(drawInfo->boardPos, drawInfo->boardDim), Vec2(), 0.0f, WHITE);
    DrawRectangleLinesEx(Rec(drawInfo->boardPos, drawInfo->boardDim), 1.0f, BLACK);

    if(game->draggedPiece == 0) _deferedPiece = 0;

    if(game->selectedPossibleMoves != 0)
    {
        for(int32 i = 0; i < arrlen(game->selectedPossibleMoves); ++i)
        {
            sq_coord sq = game->selectedPossibleMoves[i];
            Rectangle sqRec = RecForSquare(drawInfo, sq);
            DrawRectanglePro(sqRec, Vec2(), 0.0f, Fade(GREEN, 0.3f));
        }
    }

    if(game->check)
    {
        sq_coord kingSq = game->playersTurn == PLAYER_WHITE ? game->whiteKing : game->blackKing;
        Rectangle sqRec = RecForSquare(drawInfo, kingSq);
        DrawRectanglePro(sqRec, Vec2(), 0.0f, Fade(RED, 0.5f));
    }

    if(game->selectedPiece != 0)
    {
        Rectangle sqRec = RecForSquare(drawInfo, game->selectedPiece->coord);
        DrawRectangleLinesEx(sqRec, 2.0f, GREEN);
    }

    for(int32 r = 0; r < 8; ++r)
    {
        for(int32 f = 0; f < 8; ++f)
        {
            square_info sq = game->board[r][f]; 

            if(sq.piece == 0) continue;

            Rectangle pieceRec = RecForSquare(drawInfo, sq.coord, true);
            
            if(sq.piece->isDragged)
            {
                v2 mw = GetScreenToWorld2D(GetMousePosition(), drawInfo->camera);
                pieceRec.x = mw.x;
                pieceRec.y = mw.y;

                _deferedRec = pieceRec;
                _deferedPiece = sq.piece;
            }
            else
            {
                DrawTexturePro(drawInfo->piecesTex, TexRecForPiece(*sq.piece, drawInfo->piecesTex), pieceRec, Vec2(drawInfo->squareSize/2, drawInfo->squareSize/2), drawInfo->flipped ? 180.0f : 0.0f, WHITE);
            }
        }
    }

    for(int32 p = 1; p < 3; ++p)
    {
        piece_info *takenPieces = game->piecesTaken[p];

        if(arrlen(takenPieces) > 0)
        {
            int32 row = 0;
            int32 col = 0;
            for(int32 i = 0; i < arrlen(takenPieces); ++i)
            {
                int32 dir = (player_color)p == PLAYER_WHITE ? 1 : -1;
                Rectangle pieceTexRec = TexRecForPiece(takenPieces[i], drawInfo->piecesTex);
                Rectangle pieceBordRec = (player_color)p == PLAYER_WHITE ? RecForSquare(drawInfo, (sq_coord){0, 8}) : RecForSquare(drawInfo, (sq_coord){7, -1});
                if((player_color)p == PLAYER_BLACK) 
                {
                    pieceBordRec.x += drawInfo->squareSize;
                    pieceBordRec.y += drawInfo->squareSize;
                }
                pieceBordRec.width /= 3.0f;
                pieceBordRec.height /= 3.0f;
                pieceBordRec.x += dir * pieceBordRec.width/2.0f;
                if(pieceBordRec.x + dir * col * pieceBordRec.width > drawInfo->screen.width || pieceBordRec.x + dir * col * pieceBordRec.width < 0) 
                {
                    row++;
                    col = 0;
                }
                pieceBordRec.x += dir * col * pieceBordRec.width;
                pieceBordRec.y += dir * row * pieceBordRec.height + dir * pieceBordRec.height/2.0f;
                
                col++;
                DrawTexturePro(drawInfo->piecesTex, pieceTexRec, pieceBordRec, Vec2(pieceBordRec.width/2.0f, pieceBordRec.height/2.0f), drawInfo->flipped ? 180.0f : 0.0f, WHITE);
            }
        }
    }

    if(_deferedPiece != 0)
    {
        DrawTexturePro(drawInfo->piecesTex, TexRecForPiece(*_deferedPiece, drawInfo->piecesTex), _deferedRec, Vec2(drawInfo->squareSize/2, drawInfo->squareSize/2), drawInfo->flipped ? 180.0f : 0.0f, WHITE);
        _deferedPiece = 0;
    }

    EndMode2D();
    
    ///////////// DRAW FILE LETTERS AND RANK NUMBERS ON THE BOARD ///////////////////////
    const char* fileLetters[] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    v2 fStart = Vector2Add(drawInfo->boardPos, Vec2(5.0f, drawInfo->boardDim.y-drawInfo->boardFontSize));
    int32 fIndex = 0;
    for(int32 i = drawInfo->flipped ? 7 : 0; drawInfo->flipped ? i >= 0 : i < 8; drawInfo->flipped ? --i : ++i)
    {
        DrawTextPro(drawInfo->boardFont, fileLetters[i], Vec2(fStart.x + drawInfo->squareSize*fIndex++, fStart.y), Vec2(), 0.0f, drawInfo->boardFontSize, 1.0f, RED);
    }

    const char* rankNumbers[] = {"1", "2", "3", "4", "5", "6", "7", "8"};
    v2 rStart = Vector2Add(drawInfo->boardPos, Vec2(5.0f, drawInfo->boardDim.y-drawInfo->squareSize+1));
    int32 rlIndex = 0;
    for(int32 i = drawInfo->flipped ? 7 : 0; drawInfo->flipped ? i >= 0 : i < 8; drawInfo->flipped ? --i : ++i)
    {
        DrawTextPro(drawInfo->boardFont, rankNumbers[i], Vec2(rStart.x, rStart.y-drawInfo->squareSize*rlIndex++), Vec2(), 0.0f, drawInfo->boardFontSize, 1.0f, RED);
    }
}