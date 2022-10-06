#include "gui.h"

gui_button *_buttons;


void SetGameTimeControl(game_info *game, void *data)
{
    game_time_control *timeControl = (game_time_control *)data;
    game->timeControl = *timeControl;

    game->timer[PLAYER_WHITE] = timeControl->start;
    game->timer[PLAYER_BLACK] = timeControl->start;
}

game_time_control timeControls[] = {
    (game_time_control){.start = 10.0*60.0, .increment = 0.0},
    (game_time_control){.start = 5.0*60.0, .increment = 0.0},
    (game_time_control){.start = 3.0*60.0, .increment = 0.0},
    (game_time_control){.start = 1.0*60.0, .increment = 0.0},
    (game_time_control){.start = 15.0*60.0, .increment = 10.0},
    (game_time_control){.start = 3.0*60.0, .increment = 2.0},
    (game_time_control){.start = 2.0*60.0, .increment = 1.0},
    (game_time_control){.start = 1.0*60.0, .increment = 1.0},
    (game_time_control){.start = 10.0*60.0, .increment = 1.0},
    (game_time_control){.start = 10.0*60.0, .increment = 2.0},
    (game_time_control){.start = 10.0*60.0, .increment = 5.0},
    (game_time_control){.start = 10.0*60.0, .increment = 10.0},
};

void GUISetup(game_info *game, draw_info *drawInfo)
{
    
    
    for(int32 i = 0; i < ArrayCount(timeControls); ++i)
    {
        const char *text = TextFormat("%d | %d", (int32)(timeControls[i].start / 60.0), (int32)timeControls[i].increment);
        

        gui_button button = {
            .rec = Rec(0, 0, 0, 0),
            .hot = false,
            .text = (char *)calloc(TextLength(text), sizeof(char)),
            .textPos = Vec2(),
            .action = SetGameTimeControl,
            .data = (void *)&timeControls[i],
        };

        TextCopy(button.text, text);
        arrpush(_buttons, button);
    }
}

void GUIUpdate(game_info *game)
{
    for(int32 i = 0; i < arrlen(_buttons); ++i)
    {
        _buttons[i].hot = CheckCollisionPointRec(GetMousePosition(), _buttons[i].rec);

        if(_buttons[i].hot && IsMouseButtonPressed(0))
        {
            _buttons[i].action(game, _buttons[i].data);
            game->state = GAME_PLAY;
        }
    }
}

void DrawGUI(draw_info *drawInfo)
{
    for(int32 i = 0; i < arrlen(_buttons); ++i)
    {
        Color color = _buttons[i].hot ? RED : BLACK;
        DrawRectangleLinesEx(_buttons[i].rec, 2.0f, color);
        DrawTextEx(drawInfo->guiTextFont, _buttons[i].text, _buttons[i].textPos, drawInfo->guiTextFontSize, 1.0f, color);
    }
}