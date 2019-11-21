#include "MiniMaxAI.h"

#define LOG(x) std::cout << x << std::endl;
//*****************************************************Contructors/Destructors*****************************************************
MiniMaxAI::MiniMaxAI(Game* game)
{
    this->game = game;
    ai_tokens = game->getAITokens();
    player_tokens = game->getPlayerTokens();
}

MiniMaxAI::~MiniMaxAI()
{
}
//*****************************************************GET MOVE*****************************************************
void MiniMaxAI::get_move()
{
    //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    /*
    //print the board
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i * 8 + j] == nullptr)
                std:: cout << "x ";
            else
            {
                if (board[i * 8 + j]->isKing())
                    std::cout << "k ";
                else
                    std::cout << board[i * 8 + j]->getPlayer() << " ";
            }
            
        }
        std::cout << std::endl;
    }
    std::cout << "\n\n";
    */



    //create the array of AIMove structures so each thread can return best move with no race conditions
    AIMove** moves;
    moves = new AIMove*[12];
    for (int i = 0; i < 12; i++)
        moves[i] = nullptr;


    //determine how many threads to make (equal to the number of ai tokens on the board) and start the threads
    int num_threads = ai_tokens->getNumTokens();
    int count = 0;
    int index = 0;
    Token* temp;
    Token** board;
    while (count < num_threads)
    {
        temp = ai_tokens->getToken(index);
        if (temp != nullptr)
        {
            //create a board for this worker and initialize all to nullptr
            board = new Token*[64];
            for (int i = 0; i < 64; i++)
            {
                board[i] = nullptr;
            }

            //copy all tokens into board so that originals will be unaffected
            Token* temp2;
            for (int i = 0; i < 12; i++)
            {
                temp2 = ai_tokens->getToken(i);
                if (temp2 != nullptr)
                {
                    board[temp2->getRow() * 8 + temp2->getCol()] = new Token(temp2);
                }
                temp2 = player_tokens->getToken(i);
                if (temp2 != nullptr)
                {
                    board[temp2->getRow() * 8 + temp2->getCol()] = new Token(temp2);
                }
            }

            //create worker to evaluate possible moves of this token with its own copy of board
            LOG("WORKER " + std::to_string(count) + " CREATED")
            workers[count] = std::thread(&MiniMaxAI::findMove, this, temp->getID(), board, moves);
            count++;
        }
        index++;
    }

    //wait for each thread to join
    count = 0;
    while (count < num_threads)
    {
        workers[count].join();
        count++;
    }
    
    //find the best move out of best possible from each thread
    int max = -1;
    AIMove* best_move = nullptr;
    LOG("Before find best move")
    for (int i = 0; i < 12; i++)
    {
        if (moves[i] != nullptr)
        {
            
            if (moves[i]->score > max)
            {
                LOG("before set best")
                best_move = moves[i]; 
            }
        }
    }

    
    //if a best move exists, make the move
    if (best_move != nullptr) 
    {
        LOG("MAKINGG BEST MOVE")
        BoardTile* from = game->getTile(best_move->from_row, best_move->from_col);
        BoardTile* to = game->getTile(best_move->to_row, best_move->to_col);
        game->move(from, to);
    }
    else
    {
        LOG("Player Won... I think... No best move\n");
    }
}

//*****************************************************GET SCORE*****************************************************
int MiniMaxAI::getScore(Token** board)
{
    int ai_tokens = 0;
    int player_tokens = 0;
    for (int i = 0; i < 64; i++)
    {
        if (board[i] != nullptr)
        {
            if (board[i]->getPlayer() == AI)
            {
                if (board[i]->isKing())
                    ai_tokens += 2;
                else
                    ai_tokens++;
            }
            else
            {
                if (board[i]->isKing())
                    player_tokens += 2;
                else
                    player_tokens++;
            }
            
        }
    }
    if (player_tokens == 0)
        return 100;
    else
        return ai_tokens - player_tokens;
}

//*****************************************************FIND MOVE*****************************************************
void MiniMaxAI::findMove(int id, Token** board, AIMove** moves)
{
    double rand = 0;
    while(rand < 10000000)
    {
        rand+=.05;
    }
    int row;
    int col;
    AIMove* move = new AIMove;
    for (int i = 0; i < 64; i++)
    {
        if (board[i] != nullptr)
        {
            if (board[i]->getID() == id && board[i]->getPlayer() == AI)
            {
                row = board[i]->getRow();
                col = board[i]->getCol();
            }
        }
    }

    if (validateMove(board, row, col, row + 1, col + 1))
    {
        //LOG("Found move + col")
        move->score = getScore(board) + 1;
        move->from_row = row;
        move->from_col = col;
        move->to_row = row + 1;
        move->to_col = col + 1;
        moves[id] = move;
    }
    if (validateMove(board, row, col, row + 1, col - 1))
    {
        //LOG("Found move - col")
        move->score = getScore(board);
        move->from_row = row;
        move->from_col = col;
        move->to_row = row + 1;
        move->to_col = col - 1;
        moves[id] = move;
    }
}

//*****************************************************GET BOARD TOKEN*****************************************************
Token* getBoardToken(int row, int col)
{
    return nullptr;
}


//*****************************************************VALIDATE MOVE*****************************************************
bool MiniMaxAI::validateMove(Token** board, int from_row, int from_col, int to_row, int to_col)
{
    //if to or from location is out of bounds, return false right away and print error message
    if (from_row < 0 || from_row > 7){ std::cout << "Invalid from_row location sent to validateMove...\n"; return false;}
    if (from_col < 0 || from_col > 7){ std::cout << "Invalid from_col location sent to validateMove...\n"; return false;}
    if (to_row < 0 || to_row > 7){ std::cout << "Invalid to_row location sent to validateMove...\n"; return false;}
    if (to_col < 0 || to_col > 7){ std::cout << "Invalid to_col location sent to validateMove...\n"; return false;}
    //LOG("VALID Parameters")
    Token* token = board[from_row * 8 + from_col];
    if (token == nullptr) return false;

    bool is_king = token->isKing();
    
    if (token->getPlayer() == PLAYER)
    {
        //player token moves 1 space forward (can be king or regular)
        if (to_row - from_row == -1 && (abs(to_col - from_col) == 1) && board[to_row * 8 + to_col]  == nullptr)
            return true;
        //player token moves 1 space backward (must be a king)
        else if (to_row - from_row == 1 && (abs(to_col - from_col) == 1) && board[to_row * 8 + to_col]  == nullptr && is_king)
            return true;

        //player token moves two spaces forward, must jump an enemy piece
        else if (to_row - from_row == -2 && (abs(to_col - from_col) == 2) && board[to_row * 8 + to_col]  == nullptr)
        {
            //get piece to be jumped to determine if it is an enemy piece
            Token* temp;
            if (to_col > from_col)
                temp = board[(to_row + 1) * 8 + (to_col - 1)];
            else
                temp = board[(to_row + 1) * 8 + (to_col + 1)];
            
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
        else if (to_row - from_row == 2 && (abs(to_col - from_col) == 2) && board[to_row * 8 + to_col]  == nullptr && token->isKing())
        {
            //get piece to be jumped to determine if it is an enemy piece
            Token* temp;
            if (to_col > from_col)
                temp = board[(to_row - 1) * 8 + (to_col - 1)];
            else
                temp = board[(to_row - 1) * 8 + (to_col + 1)];
            
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
        if (to_row - from_row == 1 && (abs(to_col - from_col) == 1) && board[to_row * 8 + to_col]  == nullptr)
            return true;
        //ai token moves 1 space backward (must be a king)
        else if (to_row - from_row == -1 && (abs(to_col - from_col) == 1) && board[to_row * 8 + to_col]  == nullptr && is_king)
            return true;

        //ai token moves two spaces forward, must jump an enemy piece
        if (to_row - from_row == 2 && (abs(to_col - from_col) == 2) && board[to_row * 8 + to_col]  == nullptr)
        {
            //get piece to be jumped to determine if it is an enemy piece
            Token* temp;
            if (to_col > from_col)
                temp = board[(to_row - 1) * 8 + (to_col - 1)];
            else
                temp = board[(to_row - 1) * 8 + (to_col + 1)];
            
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
        if (to_row - from_row == -2 && (abs(to_col - from_col) == 2) && board[to_row * 8 + to_col]  == nullptr && token->isKing())
        {
            //get piece to be jumped to determine if it is an enemy piece
            Token* temp;
            if (to_col > from_col)
                temp = board[(to_row + 1) * 8 + (to_col - 1)];
            else
                temp = board[(to_row + 1) * 8 + (to_col + 1)];
            
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








//P)(*&)(*&)(*&)(*&)(*&)(*&)(*&*&^(*&^)(&)*(^&*(^&&*&*))) MAY HAVE MEMORY LEAK__________DELETE OBJECTS WHEN JUMP MOVE IS MADE
//***********IF SECOND JUMP POSSIBLE JUST MAKE IT... I THINK, MAKE SURE THAT WOULD WORK WITH RETURNED MOVE

void MiniMaxAI::move(Token** board, int from_row, int from_col, int to_row, int to_col)
{
    //if (validateMove(from_row, from_col, to_row, to_col))
   // {
        Token* token = board[from_row * 8 + from_col];
        token->setRow(to_row);
        token->setCol(to_col);
        board[from_row * 8 + from_col] = nullptr;
        board[to_row * 8 + to_col] = token;
        bool jump_move = false;
        //a jump was made
        if (abs(to_row - from_row) == 2)
        {  
            jump_move = true;





            //CHANGEDD THIS
            Token* jumped;








            //the jump was toward the right side of the board
            if (to_col > from_col)
            {
                //jump was made the player
                if (token->getPlayer() == PLAYER)
                {
                    //case where token jumps forward
                    if (to_row < from_row)
                    {
                        jumped = board[(to_row + 1) * 8 + (to_col - 1)];
                        if (jumped != nullptr)
                        {
                            if (jumped->getPlayer() == AI)
                            {
                                board[(to_row + 1) * 8 + (to_col - 1)] = nullptr;
                            }
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
                        jumped = board[(to_row - 1) * 8 + (to_col - 1)];
                        if (jumped != nullptr)
                        {
                            if (jumped->getPlayer() == AI)
                            {
                                board[(to_row - 1) * 8 + (to_col - 1)] = nullptr;
                            }
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
                        jumped = board[(to_row - 1) * 8 + (to_col - 1)];
                        if (jumped != nullptr)
                        {
                            if (jumped->getPlayer() == PLAYER)
                            {
                                board[(to_row - 1) * 8 + (to_col - 1)] = nullptr;
                            }
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

                        jumped = board[(to_row + 1) * 8 + (to_col - 1)];
                        if (jumped != nullptr)
                        {
                            if (jumped->getPlayer() == PLAYER)
                            {
                                board[(to_row + 1) * 8 + (to_col - 1)] = nullptr;
                            }
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
                        jumped = board[(to_row + 1) * 8 + (to_col + 1)];
                        if (jumped != nullptr)
                        {
                            if (jumped->getPlayer() == AI)
                            {
                                board[(to_row + 1) * 8 + (to_col + 1)] = nullptr;
                            }
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
                        jumped = board[(to_row - 1) * 8 + (to_col + 1)];
                        if (jumped != nullptr)
                        {
                            if (jumped->getPlayer() == AI)
                            {
                                board[(to_row - 1) * 8 + (to_col + 1)] = nullptr;
                            }
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
                        jumped = board[(to_row - 1) * 8 + (to_col + 1)];
                        if (jumped != nullptr && jumped->getPlayer() == PLAYER)
                        {
                            board[(to_row - 1) * 8 + (to_col + 1)] = nullptr;
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

                        jumped = board[(to_row + 1) * 8 + (to_col + 1)];
                        if (jumped != nullptr && jumped->getPlayer() == PLAYER)
                        {
                            board[(to_row + 1) * 8 + (to_col + 1)] = nullptr;
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
            //return true;
        }
        else if (to_row == 7 && token->getPlayer() == AI && !token->isKing())
        {
            token->setKing();
            //return true;
        }
/*
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
                                std::cout << "JUMP available to PLAYER regular token the left\n";
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
                                std::cout << "JUMP available to PLAYER regular token on the right\n";
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
                                    std::cout << "JUMP available to PLAYER king token to the back left\n";
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
                                    std::cout << "JUMP available to PLAYER king token on to the back right\n";
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
                                std::cout << "JUMP available to PLAYER regular token the left\n";
                                return false;
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
                                std::cout << "JUMP available to PLAYER regular token on the right\n";
                                return false;
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
                                    std::cout << "JUMP available to PLAYER king token to the back left\n";
                                    return false;
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
                                    std::cout << "JUMP available to PLAYER king token on to the back right\n";
                                    return false;
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
    */
}