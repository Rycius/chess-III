struct draw_info
{
    Texture2D boardTex;
    Texture2D piecesTex;
    v2 boardPos;
    v2 boardDim;
    bool flipped;

    float squareSize;

    Font boardFont;
    float boardFontSize;
    float boardMinFontSize;

    Font clockFont;
    Rectangle whiteClockRec;
    Rectangle blackClockRec;
    v2 whiteTimeTextPos;
    v2 blackTimeTextPos;
    float clockFontSize;

    Camera2D camera;
    Rectangle screen;

    Font  guiTextFont;
    float guiTextFontSize;

    int32 guiButtonsCountHor;
    float guiSpacing;
};

enum player_color
{
    PLAYER_BLACK = -1,
    PLAYER_NONE,
    PLAYER_WHITE,
};

enum piece_type
{
    PAWN = 0,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

struct sq_coord
{
    int32 rank;
    int32 file;
};

struct piece_info
{
    sq_coord coord;
    player_color owner;
    piece_type type;
    bool isDragged;
    bool hasMoved;
};

enum move_patern_type { PATERN_CONTINUES = 0, PATERN_SINGLE };

struct move_patern
{
    sq_coord *moves;
    move_patern_type type;
};


enum game_state {GAME_SETUP = 0, GAME_PLAY, GAME_END};

struct game_time_control
{
    double start;
    double increment;
};

struct game_info
{
    piece_info *board[8][8];
    game_state state;
    player_color playersTurn;
    piece_info *selectedPiece;
    piece_info *draggedPiece;
    sq_coord *selectedPossibleMoves;
    sq_coord *kingPos;
    piece_info **piecesTaken;
    bool check;
    player_color elPeasant;
    sq_coord elPeasantSq;
    int32 dir;
    game_time_control timeControl;
    double *timer;
    bool *playerMoved;
    move_patern movePaterns[6];
    player_color promoting;
};

sq_coord Coord(int32 rank, int32 file)
{
    return (sq_coord){ rank, file };
}

sq_coord CoordAdd(sq_coord a, sq_coord b)
{
    return (sq_coord){ a.rank + b.rank, a.file + b.file };
}