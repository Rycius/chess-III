
typedef void(*buttonAction)(game_info *game, void *data);

struct gui_button
{
    Rectangle rec;
    bool hot;
    char *text;
    v2 textPos;
    buttonAction action;
    void *data;
};