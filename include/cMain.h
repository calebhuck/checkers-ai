#if !defined CMAIN_H
#define CMAIN_H

class Board;
#include "Board.h"
#include <wx/wx.h>

class Game;
#include "Game.h"

class cMain: public wxFrame//, private wxTimer
{
public:
    cMain();
    ~cMain();
    //void updateTiles(wxTimerEvent& evt);
    //virtual void Notify();

private:
    Board* board;
    Game* game;
    //wxTimer update_timer;
    //DECLARE_EVENT_TABLE();
};
enum
{
    ID_Hello = 1
};

#endif