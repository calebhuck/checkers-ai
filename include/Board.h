#if !defined BOARD_H
#define BOARD_H
class BoardTile;
#include "BoardTile.h"

class Board
{
    public:
        Board(int rows, int cols);
        ~Board();
        void addTile(BoardTile* tile);
        BoardTile* getTile(int row, int col);
        int getRows();
        int getCols();
        
    private:
        BoardTile** board;
        int rows;
        int cols;
};

#endif