#include "MiniMaxAI.h"
#include <wx/wx.h>

#define LOG(x) std::cout << x << std::endl;
#define DEPTH 4
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
    //int depth = DEPTH;
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
            //workers[count] = std::thread(&MiniMaxAI::findMove, this, temp->getID(), DEPTH, AI, board, moves);
            findMove(temp->getID(), DEPTH, AI, board, moves);
            LOG("FINISHED ONE FIND MOVE CALL")
            count++;
        }
        index++;
    }
    LOG("FINISHED ALL FINDMOVE CALLS")
    //wait for each thread to join
    //count = 0;
    //while (count < num_threads)
    //{
    //    workers[count].join();
    //    count++;
    //}
    
    //find the best move out of best possible from each thread
    int max = -9999;
    AIMove* best_move = nullptr;
    for (int i = 0; i < 12; i++)
    {
        if (moves[i] != nullptr)
        {
            LOG("Possible score = " + std::to_string(moves[i]->score))
            if (moves[i]->score > max)
            {
                LOG("setting new best.......")
                best_move = moves[i]; 
                max = best_move->score;
            }
        }
    }

    
    //if a best move exists, make the move
    if (best_move != nullptr) 
    {
        LOG("Move Taken = " + std::to_string(best_move->score) + "\n\n\n")
        if (game->getTile(best_move->from_row, best_move->from_col) != nullptr && game->getTile(best_move->to_row, best_move->to_col) != nullptr)
        {
            BoardTile* from = game->getTile(best_move->from_row, best_move->from_col);
            BoardTile* to = game->getTile(best_move->to_row, best_move->to_col);
            game->move(from, to);
        }
    }
    else
    {
        LOG("Player Won... I think... No best move\n");
    }
    LOG("deleting moves")
    for (int i = 0; i < 12; i++)
    {
        if (moves[i] != nullptr)
            delete moves[i];
    }
    delete[] moves;
    LOG("moves deleted")
}

Token** MiniMaxAI::getBoardCopy(Token** old_board)
{
    try 
    {
        Token** new_board = new Token*[64];
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (old_board[i * 8 + j] != nullptr)
                {
                    new_board[i * 8 + j] = new Token(old_board[i * 8 + j]);
                }
                else
                {
                    new_board[i * 8 + j] = nullptr;
                }
                
            }
        }
        return new_board;
    }
    catch (...)
    {
        std::cout << "MEMORY ALLOC FAILED&&&&&&&&&&&&&&&%%%%%%%%%%%%%%%%%%%%%******************************\n";
    }
}

//*****************************************************FIND MOVE*****************************************************
int MiniMaxAI::findMove(int id, int depth, PlayerType turn, Token** board, AIMove** moves)
{
    if (depth == DEPTH)
    {
        LOG("STARTING MOVE SEARCH")
        int row = -1;
        int col = -1;
        int best_score = -9999;
        Token* token = nullptr;
        Token** temp_board;
        AIMove* ai_move = nullptr;

        //find the token that this thread is responsible for
        for (int i = 0; i < 64; i++)
        {
            if (board[i] != nullptr)
            {
                if (board[i]->getID() == id && board[i]->getPlayer() == AI)
                {
                    token = board[i];
                    row = token->getRow();
                    col = token->getCol();
                }
            }
        }
        if (token == nullptr) return -1;
        LOG("BREAD")

        //check each possible move for this particular token and search the resulting tree for moves recursively
        if (validateMove(board, row, col, row + 1, col + 1))
        {
            LOG("checking 1")
            temp_board = getBoardCopy(board);
            move(temp_board, row, col, row + 1, col + 1);
            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

            //for (int i = 0; i < 64; i++)
            //{
            //    if (temp_board[i] != nullptr)
            //        delete temp_board[i];
            //}
            //delete[] temp_board;

            if (score >= best_score)
            {
                if (ai_move == nullptr)
                {
                    ai_move = new AIMove();
                    ai_move->from_row = row;
                    ai_move->from_col = col;
                }
                ai_move->score = score;
                ai_move->to_row = row + 1;
                ai_move->to_col = col + 1;
                best_score = score;
            }
            LOG("After checking")
        }

        if (validateMove(board, row, col, row + 1, col - 1))
        {
            LOG("checking 2")
            temp_board = getBoardCopy(board);
            move(temp_board, row, col, row + 1, col - 1);
            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

            //for (int i = 0; i < 64; i++)
            //{
            //    if (temp_board[i] != nullptr)
            //        delete temp_board[i];
            //}
            //delete[] temp_board;

            if (score >= best_score)
            {
                if (ai_move == nullptr)
                {
                    ai_move = new AIMove();
                    ai_move->from_row = row;
                    ai_move->from_col = col;
                }
                ai_move->score = score;
                ai_move->to_row = row + 1;
                ai_move->to_col = col - 1;
                best_score = score;
            }
            LOG("After checking")
        }

        if (validateMove(board, row, col, row + 2, col + 2))
        {
            LOG("checking 3")
            temp_board = getBoardCopy(board);
            move(temp_board, row, col, row + 2, col + 2);
            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);
            //for (int i = 0; i < 64; i++)
            //{
            //    if (temp_board[i] != nullptr)
            //        delete temp_board[i];
            //}
            //delete[] temp_board;
            if (score >= best_score)
            {
                if (ai_move == nullptr)
                {
                    ai_move = new AIMove();
                    ai_move->from_row = row;
                    ai_move->from_col = col;
                }
                ai_move->score = score;
                ai_move->to_row = row + 2;
                ai_move->to_col = col + 2;
                best_score = score;
                temp_board = getBoardCopy(board);
                move(temp_board, ai_move->from_row, ai_move->from_col, ai_move->to_row, ai_move->to_col);
                if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col + 2))
                {
                    std::cout << "double jump found\n";
                    best_score += 100; 
                    ai_move->score+=100;
                }
                if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col - 2))
                {
                    std::cout << "double jump found\n";
                    best_score += 100; 
                    ai_move->score+=100;
                }
                for (int i = 0; i < 64; i++)
                {
                    if (temp_board[i] != nullptr)
                        delete temp_board[i];
                }
                delete[] temp_board;
            }
            LOG("After checking")
        }

        if (validateMove(board, row, col, row + 2, col - 2))
        {
            LOG("checking 4")
            temp_board = getBoardCopy(board);
            move(temp_board, row, col, row + 2, col - 2);
            int score =findMove(id, depth - 1, PLAYER, temp_board, nullptr);
            //for (int i = 0; i < 64; i++)
            //{
            //    if (temp_board[i] != nullptr)
            //        delete temp_board[i];
            //}
            //delete[] temp_board;
            if (score >= best_score)
            {
                if (ai_move == nullptr)
                {
                    ai_move = new AIMove();
                    ai_move->from_row = row;
                    ai_move->from_col = col;
                }
                ai_move->score = score;
                ai_move->to_row = row + 2;
                ai_move->to_col = col - 2;
                best_score = score;
                temp_board = getBoardCopy(board);
                move(temp_board, ai_move->from_row, ai_move->from_col, ai_move->to_row, ai_move->to_col);
                if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col + 2))
                {
                    std::cout << "double jump found\n";
                    best_score += 100; 
                    ai_move->score+=100;
                }
                if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col - 2))
                {
                    std::cout << "double jump found\n";
                    best_score += 100; 
                    ai_move->score+=100;
                }
                for (int i = 0; i < 64; i++)
                {
                    if (temp_board[i] != nullptr)
                        delete temp_board[i];
                }
                delete[] temp_board;
            }
            LOG("After checking")
        }

        //if the token is a king, check moves that move backward
        if (token->isKing())
        {
            if (validateMove(board, row, col, row - 1, col + 1))
            {
                LOG("checking 5")
                LOG("king move backward right")
                temp_board = getBoardCopy(board);
                move(temp_board, row, col, row - 1, col + 1);
                int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);
                //for (int i = 0; i < 64; i++)
                //{
                //    if (temp_board[i] != nullptr)
                //        delete temp_board[i];
                //}
                //delete[] temp_board;
                if (score >= best_score)
                {
                    if (ai_move == nullptr)
                    {
                        ai_move = new AIMove();
                        ai_move->from_row = row;
                        ai_move->from_col = col;
                    }
                    ai_move->score = score;
                    ai_move->to_row = row - 1;
                    ai_move->to_col = col + 1;
                    best_score = score;
                }
                LOG("After checking")
            }

            if (validateMove(board, row, col, row - 1, col - 1))
            {
                LOG("checking 6")
                LOG("king move backward left")
                temp_board = getBoardCopy(board);
                move(temp_board, row, col, row - 1, col - 1);
                int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);
                for (int i = 0; i < 64; i++)
                //{
                //    if (temp_board[i] != nullptr)
                //        delete temp_board[i];
                //}
                //delete[] temp_board;
                //if (score >= best_score)
                {
                    if (ai_move == nullptr)
                    {
                        ai_move = new AIMove();
                        ai_move->from_row = row;
                        ai_move->from_col = col;
                    }
                    ai_move->score = score;
                    ai_move->to_row = row - 1;
                    ai_move->to_col = col - 1;
                    best_score = score;
                }
                LOG("After checking")
            }

            if (validateMove(board, row, col, row - 2, col + 2))
            {
                LOG("checking 7")
                temp_board = getBoardCopy(board);
                move(temp_board, row, col, row - 2, col + 2);
                int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);
                //for (int i = 0; i < 64; i++)
                //{
                //    if (temp_board[i] != nullptr)
                //        delete temp_board[i];
                //}
                //delete[] temp_board;
                if (score >= best_score)
                {
                    if (ai_move == nullptr)
                    {
                        ai_move = new AIMove();
                        ai_move->from_row = row;
                        ai_move->from_col = col;
                    }
                    ai_move->score = score;
                    ai_move->to_row = row - 2;
                    ai_move->to_col = col + 2;
                    best_score = score;
                    temp_board = getBoardCopy(board);
                    move(temp_board, ai_move->from_row, ai_move->from_col, ai_move->to_row, ai_move->to_col);
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col + 2))
                    {
                        std::cout << "double jump found\n";
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col - 2))
                    {
                        std::cout << "double jump found\n";
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row - 2, ai_move->to_col + 2))
                    {
                        std::cout << "double jump found\n";
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row - 2, ai_move->to_col - 2))
                    {
                        std::cout << "double jump found\n";
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    for (int i = 0; i < 64; i++)
                    {
                        if (temp_board[i] != nullptr)
                            delete temp_board[i];
                    }
                    delete[] temp_board;
                }
                LOG("After checking")
            }

            if (validateMove(board, row, col, row - 2, col - 2))
            {
                LOG("checking 8")
                temp_board = getBoardCopy(board);
                move(temp_board, row, col, row - 2, col - 2);
                int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);
                //for (int i = 0; i < 64; i++)
                //{
                //    if (temp_board[i] != nullptr)
                //        delete temp_board[i];
                //}
                //delete[] temp_board;
                if (score >= best_score)
                {
                    if (ai_move == nullptr)
                    {
                        ai_move = new AIMove();
                        ai_move->from_row = row;
                        ai_move->from_col = col;
                    }
                    ai_move->score = score;
                    ai_move->to_row = row - 2;
                    ai_move->to_col = col - 2;
                    best_score = score;
                    temp_board = getBoardCopy(board);
                    move(temp_board, ai_move->from_row, ai_move->from_col, ai_move->to_row, ai_move->to_col);
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col + 2))
                    {
                        std::cout << "double jump found\n";
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col - 2))
                    {
                        std::cout << "double jump found\n";
                        best_score += 100; 
                        ai_move->score+=100;
                    }

                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row - 2, ai_move->to_col + 2))
                    {
                        std::cout << "double jump found\n";
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row - 2, ai_move->to_col - 2))
                    {
                        std::cout << "double jump found\n";
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    for (int i = 0; i < 64; i++)
                    {
                        if (temp_board[i] != nullptr)
                            delete temp_board[i];
                    }
                    delete[] temp_board;
                }
                LOG("After checking")
            }
        }

        //return the move if one was found
        if (ai_move != nullptr)
        {
            LOG("SCORE: " + std::to_string(ai_move->score))
            moves[id] = ai_move;
        }
        else
        {
            moves[id] = nullptr;
        }

        //for (int i = 0; i < 64; i++)
        //{
        //    if (board[i] != nullptr)
        //        delete board[i];
        //}
        //delete[] board;

        return 0;
    }

    //this is the last level of the tree, just return the score
    if (depth == 0)
    {
        int score = getScore(board);

        //for (int i = 0; i < 64; i++)
        //{
        //    if (board[i] != nullptr)
        //        delete board[i];
        //}
        //delete[] board;

        return score;
    }











    //this level represents the ai's turn
    if (turn == AI)
    {
        Token* token;
        Token** temp_board;
        int best_score = -9999;
        int row;
        int col;
        for (int i = 0; i < 64; i++)
        {
            token = board[i];
            if (token != nullptr)
            {
                if (token->getPlayer() == AI)
                {
                    row = token->getRow();
                    col = token->getCol();

                    if (validateMove(board, row, col, row + 1, col + 1))
                    {
                        LOG("checking 9")
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row + 1, col + 1);
                        int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        //for (int i = 0; i < 64; i++)
                        //{
                        //    if (temp_board[i] != nullptr)
                        //        delete temp_board[i];
                        //}
                        //delete[] temp_board;

                        if (score > best_score)
                        {
                            best_score = score;
                        }
                        LOG("After checking")
                    }

                    if (validateMove(board, row, col, row + 1, col - 1))
                    {
                        LOG("checking 10")
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row + 1, col - 1);
            
                        int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        //for (int i = 0; i < 64; i++)
                        //{
                        //    if (temp_board[i] != nullptr)
                        //        delete temp_board[i];
                        //}
                        //delete[] temp_board;

                        if (score >= best_score)
                        {
                            best_score = score;
                        }
                        LOG("After checking")
                    }

                    if (validateMove(board, row, col, row + 2, col + 2))
                    {
                        LOG("checking 11")
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row + 2, col + 2);
                        int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        //for (int i = 0; i < 64; i++)
                        //{
                        //    if (temp_board[i] != nullptr)
                        //        delete temp_board[i];
                        //}
                        //delete[] temp_board;

                        if (score >= best_score)
                        {
                            best_score = score;
                            temp_board = getBoardCopy(board);
                            int new_row = row + 2;
                            int new_col = col + 2;
                            move(temp_board, row, col, new_row, new_col);
                            if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                            {
                                std::cout << "double jump found\n";
                                best_score += 100; 
                            }
                            if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                            {
                                std::cout << "double jump found\n";
                                best_score += 100; 
                            }

                            for (int i = 0; i < 64; i++)
                            {
                                if (temp_board[i] != nullptr)
                                    delete temp_board[i];
                            }
                            delete[] temp_board;
                        }
                        LOG("After checking")
                    }

                    if (validateMove(board, row, col, row + 2, col - 2))
                    {
                        LOG("checking 12")
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row + 2, col - 2);
                        int score =findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        //for (int i = 0; i < 64; i++)
                        //{
                        //    if (temp_board[i] != nullptr)
                        //        delete temp_board[i];
                        //}
                        //delete[] temp_board;

                        if (score >= best_score)
                        {
                            best_score = score;
                            temp_board = getBoardCopy(board);
                            int new_row = row + 2;
                            int new_col = col - 2;
                            move(temp_board, row, col, row + 2, col - 2);
                            if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                            {
                                std::cout << "double jump found\n";
                                best_score += 100; 
                            }
                            if (validateMove(temp_board,new_row, new_col, new_row + 2, new_col - 2))
                            {
                                std::cout << "double jump found\n";
                                best_score += 100; 
                            }

                            for (int i = 0; i < 64; i++)
                            {
                                if (temp_board[i] != nullptr)
                                    delete temp_board[i];
                            }
                            delete[] temp_board;
                        }
                        LOG("After checking")
                    }

                    //if the token is a king, check moves that move backward
                    if (token->isKing())
                    {
                        if (validateMove(board, row, col, row - 1, col + 1))
                        {
                            LOG("checking 13")
                            LOG("king move backward right")
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row - 1, col + 1);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            //for (int i = 0; i < 64; i++)
                            //{
                            //    if (temp_board[i] != nullptr)
                            //        delete temp_board[i];
                            //}
                            //delete[] temp_board;

                            if (score >= best_score)
                            {
                                best_score = score;
                            }
                            LOG("After checking")
                        }

                        if (validateMove(board, row, col, row - 1, col - 1))
                        {
                            LOG("checking ")
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row - 1, col - 1);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            //for (int i = 0; i < 64; i++)
                            //{
                            //    if (temp_board[i] != nullptr)
                            //        delete temp_board[i];
                            //}
                            //delete[] temp_board;

                            if (score >= best_score)
                            {
                                best_score = score;
                            }
                            LOG("After checking")
                        }

                        if (validateMove(board, row, col, row - 2, col + 2))
                        {
                            LOG("checking 14")
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row - 2, col + 2);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            //for (int i = 0; i < 64; i++)
                            //{
                            //    if (temp_board[i] != nullptr)
                            //        delete temp_board[i];
                            //}
                            //delete[] temp_board;

                            if (score >= best_score)
                            {
                                best_score = score;
                                temp_board = getBoardCopy(board);
                                int new_row = row - 2;
                                int new_col = col + 2;
                                move(temp_board, row, col, new_row, new_col);
                                if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }

                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col + 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col - 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }

                                for (int i = 0; i < 64; i++)
                                {
                                    if (temp_board[i] != nullptr)
                                        delete temp_board[i];
                                }
                                delete[] temp_board;
                            }
                            LOG("After checking")
                        }

                        if (validateMove(board, row, col, row - 2, col - 2))
                        {
                            LOG("checking 15")
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row - 2, col - 2);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            //for (int i = 0; i < 64; i++)
                            //{
                            //    if (temp_board[i] != nullptr)
                            //        delete temp_board[i];
                            //}
                            //delete[] temp_board;

                            if (score >= best_score)
                            {
                                best_score = score;
                                temp_board = getBoardCopy(board);
                                int new_row = row - 2;
                                int new_col = col -2;
                                move(temp_board, row, col, new_row, new_col);
                                if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }

                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col + 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col - 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }

                                for (int i = 0; i < 64; i++)
                                {
                                    if (temp_board[i] != nullptr)
                                        delete temp_board[i];
                                }
                                delete[] temp_board;
                            }
                            LOG("After checking")
                        }
                    }
                }
            }
        }

        //for (int i = 0; i < 64; i++)
        //{
        //    if (board[i] != nullptr)
        //        delete board[i];
        //}
        //delete[] board;

        if (best_score == -9999)
            return 9999;
        else 
            return best_score;
    }





































    else if (turn == PLAYER)
    {
        Token* token;
        Token** temp_board;
        int best_score = 9999; //int this case, best for player, not ai
        int row;
        int col;
        for (int i = 0; i < 64; i++)
        {
            token = board[i];
            if (token != nullptr)
            {
                if (token->getPlayer() == PLAYER)
                {
                    row = token->getRow();
                    col = token->getCol();

                    if (validateMove(board, row, col, row - 1, col + 1))
                    {
                        LOG("checking 16")
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row - 1, col + 1);
                        int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        //for (int i = 0; i < 64; i++)
                        //{
                        //    if (temp_board[i] != nullptr)
                        //        delete temp_board[i];
                        //}
                        //delete[] temp_board;
                        
            
                        if (score < best_score)
                        {
                            best_score = score;
                        }
                        LOG("After checking")
                    }

                    if (validateMove(board, row, col, row - 1, col - 1))
                    {
                        LOG("checking 17")
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row + 1, col - 1);
                        int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        //for (int i = 0; i < 64; i++)
                        //{
                        //    if (temp_board[i] != nullptr)
                        //        delete temp_board[i];
                        //}
                        //delete[] temp_board;
            
                        if (score < best_score)
                        {
                            best_score = score;
                        }
                        LOG("After checking")
                    }

                    if (validateMove(board, row, col, row - 2, col + 2))
                    {
                        LOG("checking 18")
                        Token** temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row - 2, col + 2);
                        int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        //for (int i = 0; i < 64; i++)
                        //{
                        //    if (temp_board[i] != nullptr)
                        //        delete temp_board[i];
                        //}
                        //delete[] temp_board;
            
                        if (score < best_score)
                        {
                            best_score = score;

                            temp_board = getBoardCopy(board);
                            int new_row = row - 2;
                            int new_col = col + 2;
                            move(temp_board, row, col, new_row, new_col);
                            if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                            {
                                std::cout << "double jump found\n";
                                best_score -= 100; 
                            }
                            if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                            {
                                std::cout << "double jump found\n";
                                best_score -= 100; 
                            }

                            for (int i = 0; i < 64; i++)
                            {
                                if (temp_board[i] != nullptr)
                                    delete temp_board[i];
                            }
                            delete[] temp_board;
            
                        }
                        LOG("After checking")
                    }

                    if (validateMove(board, row, col, row - 2, col - 2))
                    {
                        LOG("checking 19")
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row + 2, col - 2);
                        int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        //for (int i = 0; i < 64; i++)
                        //{
                        //    if (temp_board[i] != nullptr)
                        //        delete temp_board[i];
                        //}
                        //delete[] temp_board;
            
                        if (score < best_score)
                        {
                            best_score = score;
                            temp_board = getBoardCopy(board);
                            int new_row = row + 2;
                            int new_col = col - 2;
                            move(temp_board, row, col, new_row, new_col);
                            if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                            {
                                std::cout << "double jump found\n";
                                best_score -= 100; 
                            }
                            if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                            {
                                std::cout << "double jump found\n";
                                best_score -= 100; 
                            }

                            for (int i = 0; i < 64; i++)
                            {
                                if (temp_board[i] != nullptr)
                                    delete temp_board[i];
                            }
                            delete[] temp_board;
                        }
                        LOG("After checking")
                    }

                    //if the token is a king, check moves that move backward
                    if (token->isKing())
                    {
                        if (validateMove(board, row, col, row + 1, col + 1))
                        {
                            LOG("checking 20")
                            LOG("king move backward right")
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row + 1, col + 1);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            //for (int i = 0; i < 64; i++)
                            //{
                            //    if (temp_board[i] != nullptr)
                            //        delete temp_board[i];
                            //}
                            //delete[] temp_board;
            
                            if (score < best_score)
                            {
                                best_score = score;
                            }
                            LOG("After checking")
                        }

                        if (validateMove(board, row, col, row + 1, col - 1))
                        {
                            LOG("checking ")
                            LOG("king move backward left")
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row + 1, col - 1);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            //for (int i = 0; i < 64; i++)
                            //{
                            //    if (temp_board[i] != nullptr)
                            //        delete temp_board[i];
                            //}
                            //delete[] temp_board;
            
                            if (score < best_score)
                            {
                                best_score = score;
                            }
                            LOG("After checking")
                        }

                        if (validateMove(board, row, col, row + 2, col + 2))
                        {
                            LOG("checking 21")
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row + 2, col + 2);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            //for (int i = 0; i < 64; i++)
                            //{
                            //    if (temp_board[i] != nullptr)
                            //        delete temp_board[i];
                            //}
                            //delete[] temp_board;
            
                            if (score < best_score)
                            {
                                best_score = score;
                                temp_board = getBoardCopy(board);
                                int new_row = row + 2;
                                int new_col = col + 2;
                                move(temp_board, row, col, new_row, new_col);
                                if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }

                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col + 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col - 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }

                                for (int i = 0; i < 64; i++)
                                {
                                    if (temp_board[i] != nullptr)
                                        delete temp_board[i];
                                }
                                delete[] temp_board;
                            }
                            LOG("After checking")
                        }

                        if (validateMove(board, row, col, row + 2, col - 2))
                        {
                            LOG("checking ")
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row + 2, col - 2);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            //for (int i = 0; i < 64; i++)
                            //{
                            //    if (temp_board[i] != nullptr)
                            //        delete temp_board[i];
                            //}
                            //delete[] temp_board;
            
                            if (score < best_score)
                            {
                                best_score = score;
                                temp_board = getBoardCopy(board);
                                int new_row = row + 2;
                                int new_col = col - 2;
                                move(temp_board, row, col, new_row, new_col);
                                if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }

                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col + 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col - 2))
                                {
                                    std::cout << "double jump found\n";
                                    best_score -= 100;
                                }

                                for (int i = 0; i < 64; i++)
                                {
                                    if (temp_board[i] != nullptr)
                                        delete temp_board[i];
                                }
                                delete[] temp_board;
            
                            }
                            LOG("After checking")
                        }
                    }
                }
            }
        }

        //for (int i = 0; i < 64; i++)
        //{
        //    if (board[i] != nullptr)
        //        delete board[i];
        //}
        //delete[] board;

        if (best_score == 9999)
            return -9999;
        else
            return best_score;
    }
    return -9999;
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

//*****************************************************GET BOARD TOKEN*****************************************************
Token* MiniMaxAI::getBoardToken(Token** board, int row, int col)
{
    return board[row * 8 + col];
}


//*****************************************************VALIDATE MOVE*****************************************************
bool MiniMaxAI::validateMove(Token** board, int from_row, int from_col, int to_row, int to_col)
{
    //if to or from location is out of bounds, return false right away and print error message
    if (from_row < 0 || from_row > 7){ /*std::cout << "Invalid from_row location sent to validateMove...\n";*/ return false;}
    if (from_col < 0 || from_col > 7){ /*std::cout << "Invalid from_col location sent to validateMove...\n";*/ return false;}
    if (to_row < 0 || to_row > 7){ /*std::cout << "Invalid to_row location sent to validateMove...\n";*/ return false;}
    if (to_col < 0 || to_col > 7){ /*std::cout << "Invalid to_col location sent to validateMove...\n";*/ return false;}
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








void MiniMaxAI::move(Token** board, int from_row, int from_col, int to_row, int to_col)
{
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
                                delete board[(to_row + 1) * 8 + (to_col - 1)];
                                board[(to_row + 1) * 8 + (to_col - 1)] = nullptr;
                            }
                        }
                        else 
                        {
                            std::cout << "ERROR1: Invalid move made, validateMove function should not allow this...\n";
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
                                delete board[(to_row - 1) * 8 + (to_col - 1)];
                                board[(to_row - 1) * 8 + (to_col - 1)] = nullptr;
                            }
                        }
                        else 
                        {
                            std::cout << "ERROR2: Invalid move made, validateMove function should not allow this...\n";
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
                                delete board[(to_row - 1) * 8 + (to_col - 1)];
                                board[(to_row - 1) * 8 + (to_col - 1)] = nullptr;
                            }
                        }
                        else 
                        {
                            std::cout << "ERROR3: Invalid move made,  validateMove function should not allow this...\n";
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
                                delete board[(to_row + 1) * 8 + (to_col - 1)];
                                board[(to_row + 1) * 8 + (to_col - 1)] = nullptr;
                            }
                        }
                        else 
                        {
                            std::cout << "ERROR4: Invalid move made, validateMove function should not allow this...\n";
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
                                delete board[(to_row + 1) * 8 + (to_col + 1)];
                                board[(to_row + 1) * 8 + (to_col + 1)] = nullptr;
                            }
                        }
                        else 
                        {
                            std::cout << "ERROR5: Invalid move made, validateMove function should not allow this...\n";
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
                                delete board[(to_row - 1) * 8 + (to_col + 1)];
                                board[(to_row - 1) * 8 + (to_col + 1)] = nullptr;
                            }
                        }
                        else 
                        {
                            std::cout << "ERROR6: Invalid move made, validateMove function should not allow this...\n";
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
                            delete board[(to_row - 1) * 8 + (to_col + 1)];
                            board[(to_row - 1) * 8 + (to_col + 1)] = nullptr;
                        }
                        else 
                        {
                            std::cout << "ERROR7: Invalid move made,  validateMove function should not allow this...\n";
                            //throw -1;
                        }
                    }
                    //case where jump is made backward, the token must be a king to do this
                    else if (to_row < from_row && token->isKing())
                    {

                        jumped = board[(to_row + 1) * 8 + (to_col + 1)];
                        if (jumped != nullptr && jumped->getPlayer() == PLAYER)
                        {
                            delete board[(to_row + 1) * 8 + (to_col + 1)];
                            board[(to_row + 1) * 8 + (to_col + 1)] = nullptr;
                        }
                        else 
                        {
                            std::cout << "ERROR8: Invalid move made, validateMove function should not allow this...\n";
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

}