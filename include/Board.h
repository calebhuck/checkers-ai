#if !defined BOARD_H
#define BOARD_H
#include "BoardTile.h"

class Board
{
    public:
        Board();
        ~Board();
        
    private:
        BoardTile** board;
};

#endif