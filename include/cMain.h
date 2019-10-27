#if !defined CMAIN_H
#define CMAIN_H

#include <wx/wx.h>
class Game;
#include "Game.h"
class BoardTile;
#include "BoardTile.h"

class cMain: public wxFrame
{
public:
    cMain();
    ~cMain();

private:
    BoardTile** board;
    Game* game;
};
enum
{
    ID_Hello = 1
};

#endif