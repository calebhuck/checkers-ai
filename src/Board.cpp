#include "Board.h"

Board::Board(int rows, int cols)
{
    this->rows = rows;
    this->cols = cols;
    board = new BoardTile*[rows * cols];
}

Board::Board(Board* old_board)
{
    this->rows = old_board->getRows();
    this->cols = old_board->getCols();
    this->board = new BoardTile*[rows * cols];
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            BoardTile* temp = old_board->getTile(i, j);
            std::cout << "Temp Row " << temp->getRow() << " temp col " << temp->getCol() << std::endl;
            this->addTile(new BoardTile(temp));
        }
    }
}

Board::~Board()
{
    for (int i = 0; i < rows; i++)
    {
        board[i]->~BoardTile();
        delete[] board[i];
    }
    delete[] board;
}

int Board::getRows()
{
    return this->rows;
}

int Board::getCols()
{
    return this->cols;
}

void Board::addTile(BoardTile* tile)
{
    board[tile->getRow() * cols + tile->getCol()] = tile;
}

BoardTile* Board::getTile(int row, int col)
{
    return board[row * this->cols + col];
}