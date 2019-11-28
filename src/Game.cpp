#include "Game.h"
#include <math.h>

#define SWITCH_TURN(current) current_player = current_player ? PLAYER : AI;
#define LOG(x) std::cout << x << std::endl;

Game::Game(Board* board)
{
    player_tokens = new Tokens(12, PLAYER);  
    ai_tokens = new Tokens(12, AI);  
    current_player = PLAYER;
    game_loop = std::thread(&Game::start, this);
    this->board = board;
    this->is_a_tile_selected = false;
    this->ai = new MiniMaxAI(this);
}

Game::~Game()
{
    ai->~MiniMaxAI();
    game_loop.detach();
    game_loop.~thread();
}

Board* Game::getBoard()
{
    return board;
}

PlayerType Game::getCurrentPlayer()
{
    return current_player;
}

void Game::addToken(Token* token)
{
    if (token->getPlayer() == PLAYER)
    {
        player_tokens->add(token);
    }
    else
    {
        ai_tokens->add(token);
    }
    
}

Tokens* Game::getAITokens()
{
    return ai_tokens;
}
Tokens* Game::getPlayerTokens()
{
    return player_tokens;
}


void Game::start()
{
    //current_player = PLAYER;
    while (true)
    {
        while (current_player == PLAYER)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        //ai->get_move();
    }   
    //std::terminate();
}

void Game::check_end_game()
{
    if (ai_tokens->getNumTokens() == 0)
    {
        LOG("GAME OVER - PLAYER WINS")
        current_player = NONE;
    }
    else if (player_tokens->getNumTokens() == 0)
    {
        LOG("GAME OVER - AI WINS")
        current_player = NONE;
    }
}

void Game::reportClick(BoardTile* tile)
{
    if (current_player == PLAYER)
    {
        //no tiles are selected and the current player has a piece on the selected tile
        if (!is_a_tile_selected && tile->isTokenPresent() && tile->getToken()->getPlayer() == PLAYER)
        {
            tile->setSelected();
            tile->Refresh();
            tile->Update();
            is_a_tile_selected = true;
            selected = tile;
        }

        //player clicks on selected tile to unselect
        else if (is_a_tile_selected && tile == selected)
        {
            tile->setUnselected();
            tile->Refresh();
            tile->Update();
            selected = nullptr;
            is_a_tile_selected = false;
        }

        //a tile is selected and the player clicks on another tile to make a move
        else if (is_a_tile_selected && !tile->isTokenPresent() && selected != tile)
        {
            if(validateMove(selected->getRow(), selected->getCol(), tile->getRow(), tile->getCol()))
            {
                //LOG("move player")
                move(selected, tile);
                check_end_game();
                if (current_player == AI)
                {
                    //LOG("move ai")
                    ai->get_move();
                    check_end_game();
                }
            }
        }
    }
}

bool Game::validateMove(int from_row, int from_col, int to_row, int to_col)
{
    //if to or from location is out of bounds, return false right away and print error message
    if (from_row < 0 || from_row > 7){ std::cout << "Invalid from location send to validateMove...\n"; return false;}
    if (from_col < 0 || from_col > 7){ std::cout << "Invalid from location send to validateMove...\n"; return false;}
    if (to_row < 0 || to_row > 7){ std::cout << "Invalid to location sent to validateMove...\n"; return false;}
    if (to_col < 0 || to_col > 7){ std::cout << "Invalid to location send to validateMove...\n"; return false;}
    
    Token* token = this->getTile(from_row, from_col)->getToken();
    bool is_king = token->isKing();
    
    if (token->getPlayer() == PLAYER)
    {
        //player token moves 1 space forward (can be king or regular)
        if (to_row - from_row == -1 && (abs(to_col - from_col) == 1) && !getTile(to_row, to_col)->isTokenPresent())
            return true;
        //player token moves 1 space backward (must be a king)
        else if (to_row - from_row == 1 && (abs(to_col - from_col) == 1) && !getTile(to_row, to_col)->isTokenPresent() && is_king)
            return true;

        //player token moves two spaces forward, must jump an enemy piece
        else if (to_row - from_row == -2 && (abs(to_col - from_col) == 2) && !getTile(to_row, to_col)->isTokenPresent())
        {
            //get piece to be jumped to determine if it is an enemy piece
            Token* temp;
            if (to_col > from_col)
                temp = getTile(to_row + 1, to_col - 1)->getToken();
            else
                temp = getTile(to_row + 1, to_col + 1)->getToken();
            
            if (temp != nullptr)
            {
                if (temp->getPlayer() == AI)
                {
                    return true;
                }
            }
            else
            {
                return false;
            }
            
        }
        
        //player token moves two spaces backward, must jump an enemy piece and be a king token
        else if (to_row - from_row == 2 && (abs(to_col - from_col) == 2) && !getTile(to_row, to_col)->isTokenPresent() && token->isKing())
        {
            //get piece to be jumped to determine if it is an enemy piece
            Token* temp;
            if (to_col > from_col)
                temp = getTile(to_row - 1, to_col - 1)->getToken();
            else
                temp = getTile(to_row - 1, to_col + 1)->getToken();
            
            if (temp != nullptr)
            {
                if (temp->getPlayer() == AI)
                    return true;
            }
            else
            {
                return false;
            }
            
        }
    }
   
    if (token->getPlayer() == AI)
    {
        //ai token moves 1 space forward (can be king or regular)
        if (to_row - from_row == 1 && (abs(to_col - from_col) == 1) && !getTile(to_row, to_col)->isTokenPresent())
            return true;
        //ai token moves 1 space backward (must be a king)
        else if (to_row - from_row == -1 && (abs(to_col - from_col) == 1) && !getTile(to_row, to_col)->isTokenPresent() && is_king)
            return true;

        //ai token moves two spaces forward, must jump an enemy piece
        if (to_row - from_row == 2 && (abs(to_col - from_col) == 2) && !getTile(to_row, to_col)->isTokenPresent())
        {
            //get piece to be jumped to determine if it is an enemy piece
            Token* temp;
            if (to_col > from_col)
                temp = getTile(to_row - 1, to_col - 1)->getToken();
            else
                temp = getTile(to_row - 1, to_col + 1)->getToken();
            
            if (temp != nullptr)
            {
                if (temp->getPlayer() == PLAYER)
                    return true;
            }
            else
            {
                return false;
            }
            
        }
        
        //ai token moves two spaces backward, must jump an enemy piece and be a king token
        if (to_row - from_row == -2 && (abs(to_col - from_col) == 2) && !getTile(to_row, to_col)->isTokenPresent() && token->isKing())
        {
            //get piece to be jumped to determine if it is an enemy piece
            Token* temp;
            if (to_col > from_col)
                temp = getTile(to_row + 1, to_col - 1)->getToken();
            else
                temp = getTile(to_row + 1, to_col + 1)->getToken();
            
            if (temp != nullptr)
            {
                if (temp->getPlayer() == PLAYER)
                    return true;
            }
            else
            {
                return false;
            }
            
        }
    }
    return false;
}

bool Game::move(BoardTile* from, BoardTile* to)
{
    int from_row = from->getRow();
    int from_col = from->getCol();
    int to_row = to->getRow();
    int to_col = to->getCol();
    
    if (validateMove(from_row, from_col, to_row, to_col))
    {
        Token* token = from->getToken();
        token->setRow(to_row);
        token->setCol(to_col);
        from->removeToken();
        to->setToken(token);
        selected = nullptr;
        is_a_tile_selected = false;
        bool jump_move = false;

        from->Refresh();
        from->Update();
        to->Refresh();
        to->Update();

        //a jump was made
        if (abs(to_row - from_row) == 2)
        {  
            jump_move = true;
            BoardTile* jumped;
            //the jump was toward the right side of the board
            if (to_col > from_col)
            {
                //jump was made the player
                if (token->getPlayer() == PLAYER)
                {
                    //case where token jumps forward
                    if (to_row < from_row)
                    {
                        jumped = getTile(to_row + 1, to_col - 1);
                        if (jumped->isTokenPresent() && jumped->getToken()->getPlayer() == AI)
                        {
                            ai_tokens->remove(jumped->getToken());
                            jumped->removeToken();
                        }
                        else 
                        {
                            std::cout << "ERROR: Invalid move made, validateMove function should not allow this...\n";
                            //throw -1;

                        }
                    }
                    //case where jump is made backward, the token must be a king to do this
                    else if (to_row > from_row && token->isKing())
                    {
                        jumped = getTile(to_row - 1, to_col - 1);
                        if (jumped->isTokenPresent() && jumped->getToken()->getPlayer() == AI)
                        {
                            ai_tokens->remove(jumped->getToken());
                            jumped->removeToken();
                        }
                        else 
                        {
                            std::cout << "ERROR: Invalid move made, validateMove function should not allow this...\n";
                            //throw -1;
                        }
                    }

                }
                //jump was made by the AI
                else if (token->getPlayer() == AI)
                {
                    //case where token jumps forward
                    if (to_row > from_row)
                    {
                        jumped = getTile(to_row - 1, to_col - 1);
                        if (jumped->isTokenPresent() && jumped->getToken()->getPlayer() == PLAYER)
                        {
                            player_tokens->remove(jumped->getToken());
                            jumped->removeToken();
                            
                        }
                        else 
                        {
                            std::cout << "ERROR: Invalid move made,  validateMove function should not allow this...\n";
                            //throw -1;
                        }
                    }
                    //case where jump is made backward, the token must be a king to do this
                    else if (to_row < from_row && token->isKing())
                    {

                        jumped = getTile(to_row + 1, to_col - 1);
                        if (jumped->isTokenPresent() && jumped->getToken()->getPlayer() == PLAYER)
                        {
                            player_tokens->remove(jumped->getToken());
                            jumped->removeToken();
                        }
                        else 
                        {
                            std::cout << "ERROR: Invalid move made, validateMove function should not allow this...\n";
                            //throw -1;
                        }
                    }
                }
            }

            //the jump was made toward the left side of the board
            if (to_col < from_col)
            {
                //jump was made the player
                if (token->getPlayer() == PLAYER)
                {
                    //case where token jumps forward
                    if (to_row < from_row)
                    {
                        jumped = getTile(to_row + 1, to_col + 1);
                        if (jumped->isTokenPresent() && jumped->getToken()->getPlayer() == AI)
                        {
                            ai_tokens->remove(jumped->getToken());
                            jumped->removeToken();
                        }
                        else 
                        {
                            std::cout << "ERROR: Invalid move made, validateMove function should not allow this...\n";
                            //throw -1;

                        }
                    }
                    //case where jump is made backward, the token must be a king to do this
                    else if (to_row > from_row && token->isKing())
                    {
                        jumped = getTile(to_row - 1, to_col + 1);
                        if (jumped->isTokenPresent() && jumped->getToken()->getPlayer() == AI)
                        {
                            ai_tokens->remove(jumped->getToken());
                            jumped->removeToken();
                        }
                        else 
                        {
                            std::cout << "ERROR: Invalid move made, validateMove function should not allow this...\n";
                            //throw -1;
                        }
                    }

                }
                //jump was made by the AI
                else if (token->getPlayer() == AI)
                {
                    //case where token jumps forward
                    if (to_row > from_row)
                    {
                        jumped = getTile(to_row - 1, to_col + 1);
                        if (jumped->isTokenPresent() && jumped->getToken()->getPlayer() == PLAYER)
                        {
                            player_tokens->remove(jumped->getToken());
                            jumped->removeToken();
                        }
                        else 
                        {
                            std::cout << "ERROR: Invalid move made,  validateMove function should not allow this...\n";
                            //throw -1;
                        }
                    }
                    //case where jump is made backward, the token must be a king to do this
                    else if (to_row < from_row && token->isKing())
                    {

                        jumped = getTile(to_row + 1, to_col + 1);
                        if (jumped->isTokenPresent() && jumped->getToken()->getPlayer() == PLAYER)
                        {
                            player_tokens->remove(jumped->getToken());
                            jumped->removeToken();
                        }
                        else 
                        {
                            std::cout << "ERROR: Invalid move made, validateMove function should not allow this...\n";
                            //throw -1;
                        }
                    }
                }  
            }

        }
        
        //if a token made it to the end, change it to a king and end the turn
        if (to_row == 0 && token->getPlayer() == PLAYER && !token->isKing())
        {
            token->setKing();
            SWITCH_TURN(current_player);
            return true;
        }
        if (to_row == 7 && token->getPlayer() == AI && !token->isKing())
        {
            token->setKing();
            SWITCH_TURN(current_player);
            return true;
        }

        //check for additional jumps, all available jumps must be made before the turn ends
        if (jump_move)
        {
            int row = to_row;
            int col = to_col;

            if (token->getPlayer() == PLAYER)
            {
                if (row - 2 >= 0)
                {
                    //check for a jump to the left
                    if (col - 2 >= 0)
                    {
                        if (!getTile(row - 2, col - 2)->isTokenPresent() && getTile(row - 1, col - 1)->isTokenPresent())
                        {
                            if (getTile(row - 1, col - 1)->getToken()->getPlayer() == AI)
                            {
                                return false;
                            }
                        }
                    }
                    //check for a jump to the right
                    if (col + 2 <= 7)
                    {
                        if (!getTile(row - 2, col + 2)->isTokenPresent() && getTile(row - 1, col + 1)->isTokenPresent())
                        {
                            if (getTile(row - 1, col + 1)->getToken()->getPlayer() == AI)
                            {
                                return false;
                            }
                        }
                    }
                }
            
                //check for backward jumps
                if (token->isKing())
                {
                    if (row + 2 <= 7)
                    {
                        //check for a jump to the back left
                        if (col - 2 >= 0)
                        {
                            if (!getTile(row + 2, col - 2)->isTokenPresent() && getTile(row + 1, col - 1)->isTokenPresent())
                            {
                                if (getTile(row + 1, col - 1)->getToken()->getPlayer() == AI)
                                {
                                    return false;
                                }
                            }
                        }

                        //check for a jump to the right
                        if (col + 2 <= 7)
                        {
                            if (!getTile(row + 2, col + 2)->isTokenPresent() && getTile(row + 1, col + 1)->isTokenPresent())
                            {
                                if (getTile(row + 1, col + 1)->getToken()->getPlayer() == AI)
                                {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
            //if AI's move, check for possible jumps before turn ends
            if (token->getPlayer() == AI)
            {
                //check for jumps forward (regular token or king)
                if (row + 2 <= 7)
                {
                    //check for a jump to the left
                    if (col - 2 >= 0)
                    {
                        if (!getTile(row + 2, col - 2)->isTokenPresent() && getTile(row + 1, col - 1)->isTokenPresent())
                        {
                            if (getTile(row + 1, col - 1)->getToken()->getPlayer() == PLAYER)
                            {
                                token = getTile(row, col)->getToken();
                                token->setRow(row + 2);
                                token->setCol(col - 2);
                                if (token->getRow() == 7)
                                {
                                    token->setKing();
                                }
                                getTile(row, col)->removeToken();

                                getTile(row + 2, col - 2)->Refresh();
                                getTile(row + 2, col - 2)->Update();

                                getTile(row + 2, col - 2)->setToken(token);

                                token = getTile(row + 1, col - 1)->getToken();
                                player_tokens->remove(token);
                                getTile(row + 1, col - 1)->removeToken();

                                SWITCH_TURN(current_player)
                                return true;
                            }
                        }
                    }
                    //check for a jump to the right
                    if (col + 2 <= 7)
                    {
                        if (!getTile(row + 2, col + 2)->isTokenPresent() && getTile(row + 1, col + 1)->isTokenPresent())
                        {
                            if (getTile(row + 1, col + 1)->getToken()->getPlayer() == PLAYER)
                            {
                                
                                token = getTile(row, col)->getToken();
                                token->setRow(row + 2);
                                token->setCol(col + 2);
                                if (token->getRow() == 7)
                                {
                                    token->setKing();
                                }
                                getTile(row, col)->removeToken();

                                getTile(row + 2, col + 2)->Refresh();
                                getTile(row + 2, col + 2)->Update();

                                getTile(row + 2, col + 2)->setToken(token);

                                token = getTile(row + 1, col + 1)->getToken();
                                player_tokens->remove(token);
                                getTile(row + 1, col + 1)->removeToken();

                                SWITCH_TURN(current_player)
                                return true;
                            }
                        }
                    }
                }
            
                //check for backward jumps (king only)
                if (token->isKing())
                {
                    if (row - 2 >= 0)
                    {
                        //check for a jump to the back left
                        if (col - 2 >= 0)
                        {
                            if (!getTile(row - 2, col - 2)->isTokenPresent() && getTile(row - 1, col - 1)->isTokenPresent())
                            {
                                if (getTile(row - 1, col - 1)->getToken()->getPlayer() == PLAYER)
                                {
                                    token = getTile(row, col)->getToken();
                                    token->setRow(row - 2);
                                    token->setCol(col - 2);
                                    if (token->getRow() == 7)
                                    {
                                        token->setKing();
                                    }
                                    getTile(row, col)->removeToken();

                                    getTile(row - 2, col - 2)->Refresh();
                                    getTile(row - 2, col - 2)->Update();

                                    getTile(row - 2, col - 2)->setToken(token);

                                    token = getTile(row - 1, col - 1)->getToken();
                                    player_tokens->remove(token);
                                    getTile(row - 1, col - 1)->removeToken();

                                    SWITCH_TURN(current_player)
                                    return true;
                                }
                            }
                        }

                        //check for a jump to the right
                        if (col + 2 <= 7)
                        {
                            if (!getTile(row - 2, col + 2)->isTokenPresent() && getTile(row - 1, col + 1)->isTokenPresent())
                            {
                                if (getTile(row - 1, col + 1)->getToken()->getPlayer() == PLAYER)
                                {
                                    token = getTile(row, col)->getToken();
                                    token->setRow(row - 2);
                                    token->setCol(col + 2);
                                    if (token->getRow() == 7)
                                    {
                                        token->setKing();
                                    }
                                    getTile(row, col)->removeToken();

                                    getTile(row - 2, col + 2)->Refresh();
                                    getTile(row - 2, col + 2)->Update();

                                    getTile(row - 2, col + 2)->setToken(token);

                                    token = getTile(row - 1, col + 1)->getToken();
                                    player_tokens->remove(token);
                                    getTile(row - 1, col + 1)->removeToken();

                                    SWITCH_TURN(current_player)
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        return false;
    }

    SWITCH_TURN(current_player);
    return true;
}

BoardTile* Game::getTile(int row, int col)
{
    return board->getTile(row, col);
}