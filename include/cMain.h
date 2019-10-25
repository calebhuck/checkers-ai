#if !defined CMAIN_H
#define CMAIN_H

#include "BoardTile.h"

class cMain: public wxFrame
{
public:
    cMain();
    ~cMain();

private:
    //wxDECLARE_EVENT_TABLE();
    BoardTile** board;
    Game* game;
};
enum
{
    ID_Hello = 1
};

#endif