#include "Game.h"

Game::Game(BoardTile** board)
{
    this->board = board;
    this->is_a_tile_selected = false;
    this->num_player_tokens = 12;
    this->num_ai_tokens = 12;
}

Game::~Game()
{
    
}
//**************
//remove reference to window if not using SetStatusText()
void Game::reportClick(BoardTile* tile)
{
    std::cout << "row: " << tile->getRow() << " Col: " << tile->getCol() << std::endl;
    //no tiles are selected and the current player has a piece on the selected tile
    if (!is_a_tile_selected && tile->isTokenPresent() && tile->getToken()->getPlayer() == PLAYER)
    {
        tile->setSelected();
        is_a_tile_selected = true;
        selected = tile;
    }

    //player clicks on selected tile to unselect
    else if (is_a_tile_selected && tile == selected)
    {
        tile->setUnselected();
        selected = nullptr;
        is_a_tile_selected = false;
    }

    //a tile is selected and the player clicks on another tile to make a move
    else if (is_a_tile_selected && !tile->isTokenPresent() && selected != tile)
    {
        std::cout << "make a move\n";
        bool is_valid_move = validateMove(selected->getRow(), selected->getCol(), tile->getRow(), tile->getCol());
        move(selected, tile);
    }
}

bool Game::validateMove(int row_1, int col_1, int row_2, int col_2)
{
    return true;
}

bool Game::move(BoardTile* from, BoardTile* to)
{
    Token* token = from->getToken();
    from->removeToken();
    to->setToken(token);
    selected = nullptr;
    is_a_tile_selected = false;
}

BoardTile* Game::getTile(int row, int col)
{
    return board[row * 8 + col];
}