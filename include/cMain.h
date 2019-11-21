#if !defined CMAIN_H
#define CMAIN_H

class Board;
#include "Board.h"
#include <wx/wx.h>
class Game;
#include "Game.h"
//class BoardTile;
//#include "BoardTile.h"

class cMain: public wxFrame
{
public:
    cMain();
    ~cMain();

private:
    //BoardTile** board;
    Board* board;
    Game* game;
};
enum
{
    ID_Hello = 1
};

#endif