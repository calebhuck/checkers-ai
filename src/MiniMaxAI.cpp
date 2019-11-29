#include "MiniMaxAI.h"
#include <wx/wx.h>
#include <fstream>

#define LOG(x) std::cout << x << std::endl;
#define DEPTH 5
#define MAX 99999
#define MIN -99999

MiniMaxAI::MiniMaxAI(Game* game)
{
    this->game = game;
    ai_tokens = game->getAITokens();
    player_tokens = game->getPlayerTokens();
}

MiniMaxAI::~MiniMaxAI()
{
}


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

    auto start = std::chrono::system_clock::now();
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

            //for (int i = 0; i < 8; i++)
            //{
            //    for (int j = 0; j < 8; j++)
            //    {
            //        if (board[i * 8 + j] != nullptr)
            //        {
            //            if (board[i * 8 + j]->getPlayer() == PLAYER) std::cout << "p   ";
            //            else if (board[i * 8 + j]->getPlayer() == AI) std::cout << "a   ";
            //        }
            //        else std::cout << "x   ";
            //    }
            //    LOG("\n")
            //}
//
            //LOG("\n\n")

            //create worker to evaluate possible moves of this token with its own copy of board
            //workers[count] = std::thread(&MiniMaxAI::findMove, this, temp->getID(), DEPTH, AI, board, moves);
            findMove(temp->getID(), DEPTH, AI, board, moves);
            count++;
        }
        index++;
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::ofstream outfile;
    outfile.open(std::to_string(1) + "_thread_" + std::to_string(DEPTH) + "_depth.txt", std::ios_base::app);
    outfile << std::fixed << std::setprecision(10) << diff.count() << std::endl;
    std::cout << diff.count() << std::endl;
    //wait for each thread to join
    //count = 0;
    //while (count < num_threads)
    //{
    //    workers[count].join();
    //    count++;
    //}
    
    for (int i = 0; i < 12; i++)
    {
        if (moves[i] != nullptr)
            std::cout << "Score: " << moves[i]->score << std::endl;
    }
    LOG("")

    //find the best move out of best possible from each thread
    int max = -66666;
    AIMove* best_move = nullptr;
    for (int i = 0; i < 12; i++)
    {
        if (moves[i] != nullptr)
        {
            if (moves[i]->score > max)
            {
                best_move = moves[i]; 
                max = best_move->score;
            }
        }
    }

    std::cout << "BEST: " << max << "\n\n\n";

    
    //if a best move exists, make the move
    if (best_move != nullptr) 
    {
        if (game->getTile(best_move->from_row, best_move->from_col) != nullptr && game->getTile(best_move->to_row, best_move->to_col) != nullptr)
        {
            BoardTile* from = game->getTile(best_move->from_row, best_move->from_col);
            BoardTile* to = game->getTile(best_move->to_row, best_move->to_col);
            game->move(from, to);
        }
    }
    else
    {
        LOG("ERROR: No viable moves");
    }
   
    for (int i = 0; i < 12; i++)
    {
        if (moves[i] != nullptr)
            delete moves[i];
    }
    delete[] moves;
}

Token** MiniMaxAI::getBoardCopy(Token** old_board)
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

int MiniMaxAI::findMove(int id, int depth, PlayerType turn, Token** board, AIMove** moves)
{
    if (depth == DEPTH)
    {

        int row = -1;
        int col = -1;
        int best_score = -1000000;
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

        //check each possible move for this particular token and search the resulting tree for moves recursively
        if (validateMove(board, row, col, row + 1, col + 1))
        {
            temp_board = getBoardCopy(board);
            move(temp_board, row, col, row + 1, col + 1);
            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

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
        }

        if (validateMove(board, row, col, row + 1, col - 1))
        {
            temp_board = getBoardCopy(board);
            move(temp_board, row, col, row + 1, col - 1);
            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

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
        }

        if (validateMove(board, row, col, row + 2, col + 2))
        {
            temp_board = getBoardCopy(board);
            move(temp_board, row, col, row + 2, col + 2);
            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);
            
            if (score >= best_score)
            {
                if (ai_move == nullptr)
                {
                    ai_move = new AIMove();
                    ai_move->from_row = row;
                    ai_move->from_col = col;
                }
                ai_move->score = ++score;
                ai_move->to_row = row + 2;
                ai_move->to_col = col + 2;
                best_score = ++score;
                temp_board = getBoardCopy(board);
                move(temp_board, ai_move->from_row, ai_move->from_col, ai_move->to_row, ai_move->to_col);
                if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col + 2))
                {
                    best_score += 100; 
                    ai_move->score += 100;
                }
                if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col - 2))
                {
                    best_score += 100; 
                    ai_move->score += 100;
                }
                for (int i = 0; i < 64; i++)
                {
                    if (temp_board[i] != nullptr)
                        delete temp_board[i];
                }
                delete[] temp_board;
            }
        }

        if (validateMove(board, row, col, row + 2, col - 2))
        {
            temp_board = getBoardCopy(board);
            move(temp_board, row, col, row + 2, col - 2);
            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);
            
            if (score >= best_score)
            {
                if (ai_move == nullptr)
                {
                    ai_move = new AIMove();
                    ai_move->from_row = row;
                    ai_move->from_col = col;
                }
                ai_move->score = ++score;
                ai_move->to_row = row + 2;
                ai_move->to_col = col - 2;
                best_score = ++score;
                temp_board = getBoardCopy(board);
                move(temp_board, ai_move->from_row, ai_move->from_col, ai_move->to_row, ai_move->to_col);
                if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col + 2))
                {
                    best_score += 100; 
                    ai_move->score+=100;
                }
                if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col - 2))
                {
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
        }

        //if the token is a king, check moves that move backward
        if (token->isKing())
        {
            if (validateMove(board, row, col, row - 1, col + 1))
            {
                temp_board = getBoardCopy(board);
                move(temp_board, row, col, row - 1, col + 1);
                int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);
                
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
            }

            if (validateMove(board, row, col, row - 1, col - 1))
            {
                temp_board = getBoardCopy(board);
                move(temp_board, row, col, row - 1, col - 1);
                int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);
                
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
                    ai_move->to_col = col - 1;
                    best_score = score;
                }
            }

            if (validateMove(board, row, col, row - 2, col + 2))
            {
                temp_board = getBoardCopy(board);
                move(temp_board, row, col, row - 2, col + 2);
                int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);
                
                if (score >= best_score)
                {
                    if (ai_move == nullptr)
                    {
                        ai_move = new AIMove();
                        ai_move->from_row = row;
                        ai_move->from_col = col;
                    }
                    ai_move->score = ++score;
                    ai_move->to_row = row - 2;
                    ai_move->to_col = col + 2;
                    best_score = ++score;
                    temp_board = getBoardCopy(board);
                    move(temp_board, ai_move->from_row, ai_move->from_col, ai_move->to_row, ai_move->to_col);
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col + 2))
                    {
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col - 2))
                    {
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row - 2, ai_move->to_col + 2))
                    {
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row - 2, ai_move->to_col - 2))
                    {
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
            }

            if (validateMove(board, row, col, row - 2, col - 2))
            {
                temp_board = getBoardCopy(board);
                move(temp_board, row, col, row - 2, col - 2);
                int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);
                
                if (score >= best_score)
                {
                    if (ai_move == nullptr)
                    {
                        ai_move = new AIMove();
                        ai_move->from_row = row;
                        ai_move->from_col = col;
                    }
                    ai_move->score = ++score;
                    ai_move->to_row = row - 2;
                    ai_move->to_col = col - 2;
                    best_score = ++score;
                    temp_board = getBoardCopy(board);
                    move(temp_board, ai_move->from_row, ai_move->from_col, ai_move->to_row, ai_move->to_col);
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col + 2))
                    {
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row + 2, ai_move->to_col - 2))
                    {
                        best_score += 100; 
                        ai_move->score+=100;
                    }

                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row - 2, ai_move->to_col + 2))
                    {
                        best_score += 100; 
                        ai_move->score+=100;
                    }
                    if (validateMove(temp_board, ai_move->to_row, ai_move->to_col, ai_move->to_row - 2, ai_move->to_col - 2))
                    {
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
            }
        }

        //return the move if one was found
        if (ai_move != nullptr)
        {
            moves[id] = ai_move;
        }
        else
        {
            moves[id] = nullptr;
        }

        for (int i = 0; i < 64; i++)
        {
            if (board[i] != nullptr)
                delete board[i];
        }
        delete[] board;

        return 0;
    }

    //this is the last level of the tree, just return the score
    if (depth == 0)
    {
        int score = getScore(board);

        for (int i = 0; i < 64; i++)
        {
            if (board[i] != nullptr)
                delete board[i];
        }
        delete[] board;

        return score;
    }

    //this level represents the ai's turn
    if (turn == AI)
    {
        Token* token;
        Token** temp_board;
        int best_score = MIN;
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
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row + 1, col + 1);
                        int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        if (score >= best_score)
                        {
                            best_score = score;
                        }
                    }

                    if (validateMove(board, row, col, row + 1, col - 1))
                    {
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row + 1, col - 1);
            
                        int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        if (score >= best_score)
                        {
                            best_score = score;
                        }
                    }

                    if (validateMove(board, row, col, row + 2, col + 2))
                    {
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row + 2, col + 2);
                        int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        if (score >= best_score)
                        {
                            best_score = score;
                            temp_board = getBoardCopy(board);
                            int new_row = row + 2;
                            int new_col = col + 2;
                            move(temp_board, row, col, new_row, new_col);
                            if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                            {
                                best_score += 100; 
                            }
                            if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                            {
                                best_score += 100; 
                            }

                            for (int i = 0; i < 64; i++)
                            {
                                if (temp_board[i] != nullptr)
                                    delete temp_board[i];
                            }
                            delete[] temp_board;
                        }
                    }

                    if (validateMove(board, row, col, row + 2, col - 2))
                    {
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row + 2, col - 2);
                        int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                        if (score >= best_score)
                        {
                            best_score = score;
                            temp_board = getBoardCopy(board);
                            int new_row = row + 2;
                            int new_col = col - 2;
                            move(temp_board, row, col, row + 2, col - 2);
                            if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                            {
                                best_score += 100; 
                            }
                            if (validateMove(temp_board,new_row, new_col, new_row + 2, new_col - 2))
                            {
                                best_score += 100; 
                            }

                            for (int i = 0; i < 64; i++)
                            {
                                if (temp_board[i] != nullptr)
                                    delete temp_board[i];
                            }
                            delete[] temp_board;
                        }
                    }

                    //if the token is a king, check moves that move backward
                    if (token->isKing())
                    {
                        if (validateMove(board, row, col, row - 1, col + 1))
                        {
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row - 1, col + 1);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            if (score >= best_score)
                            {
                                best_score = score;
                            }
                        }

                        if (validateMove(board, row, col, row - 1, col - 1))
                        {
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row - 1, col - 1);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            if (score >= best_score)
                            {
                                best_score = score;
                            }
                        }

                        if (validateMove(board, row, col, row - 2, col + 2))
                        {
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row - 2, col + 2);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            if (score >= best_score)
                            {
                                best_score = score;
                                temp_board = getBoardCopy(board);
                                int new_row = row - 2;
                                int new_col = col + 2;
                                move(temp_board, row, col, new_row, new_col);
                                if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                                {
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                                {
                                    best_score += 100;
                                }

                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col + 2))
                                {
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col - 2))
                                {
                                    best_score += 100;
                                }

                                for (int i = 0; i < 64; i++)
                                {
                                    if (temp_board[i] != nullptr)
                                        delete temp_board[i];
                                }
                                delete[] temp_board;
                            }
                        }

                        if (validateMove(board, row, col, row - 2, col - 2))
                        {
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row - 2, col - 2);
                            int score = findMove(id, depth - 1, PLAYER, temp_board, nullptr);

                            if (score >= best_score)
                            {
                                best_score = score;
                                temp_board = getBoardCopy(board);
                                int new_row = row - 2;
                                int new_col = col -2;
                                move(temp_board, row, col, new_row, new_col);
                                if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                                {
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                                {
                                    best_score += 100;
                                }

                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col + 2))
                                {
                                    best_score += 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col - 2))
                                {
                                    best_score += 100;
                                }

                                for (int i = 0; i < 64; i++)
                                {
                                    if (temp_board[i] != nullptr)
                                        delete temp_board[i];
                                }
                                delete[] temp_board;
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < 64; i++)
        {
            if (board[i] != nullptr)
                delete board[i];
        }
        delete[] board;

        if (best_score == MIN)
        {
            return MIN;
        }

        return best_score;
    }

    else if (turn == PLAYER)
    {
        Token* token;
        Token** temp_board;
        int best_score = MAX; //int this case, best for player, not ai
        int row = -1;
        int col = -1;
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
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row - 1, col + 1);
                        int score = findMove(id, depth - 1, AI, temp_board, nullptr);                        
            
                        if (score <= best_score)
                        {
                            best_score = score;
                        }
                    }

                    if (validateMove(board, row, col, row - 1, col - 1))
                    {
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row - 1, col - 1);
                        int score = findMove(id, depth - 1, AI, temp_board, nullptr);
            
                        if (score <= best_score)
                        {
                            best_score = score;
                        }
                    }

                    if (validateMove(board, row, col, row - 2, col + 2))
                    {
                        Token** temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row - 2, col + 2);
                        int score = findMove(id, depth - 1, AI, temp_board, nullptr);
            
                        if (score <= best_score)
                        {
                            best_score = score;

                            temp_board = getBoardCopy(board);
                            int new_row = row - 2;
                            int new_col = col + 2;
                            move(temp_board, row, col, new_row, new_col);
                            if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col + 2))
                            {
                                best_score -= 100; 
                            }
                            if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col - 2))
                            {
                                best_score -= 100; 
                            }

                            for (int i = 0; i < 64; i++)
                            {
                                if (temp_board[i] != nullptr)
                                    delete temp_board[i];
                            }
                            delete[] temp_board;
            
                        }
                    }

                    if (validateMove(board, row, col, row - 2, col - 2))
                    {
                        temp_board = getBoardCopy(board);
                        move(temp_board, row, col, row - 2, col - 2);
                        int score = findMove(id, depth - 1, AI, temp_board, nullptr);
            
                        if (score <= best_score)
                        {
                            best_score = score;
                            temp_board = getBoardCopy(board);
                            int new_row = row - 2;
                            int new_col = col - 2;
                            move(temp_board, row, col, new_row, new_col);
                            if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col + 2))
                            {
                                best_score -= 100; 
                            }
                            if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col - 2))
                            {
                                best_score -= 100; 
                            }

                            for (int i = 0; i < 64; i++)
                            {
                                if (temp_board[i] != nullptr)
                                    delete temp_board[i];
                            }
                            delete[] temp_board;
                        }
                    }

                    //if the token is a king, check moves that move backward
                    if (token->isKing())
                    {
                        if (validateMove(board, row, col, row + 1, col + 1))
                        {
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row + 1, col + 1);
                            int score = findMove(id, depth - 1, AI, temp_board, nullptr);
            
                            if (score <= best_score)
                            {
                                best_score = score;
                            }
                        }

                        if (validateMove(board, row, col, row + 1, col - 1))
                        {
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row + 1, col - 1);
                            int score = findMove(id, depth - 1, AI, temp_board, nullptr);
            
                            if (score <= best_score)
                            {
                                best_score = score;
                            }
                        }

                        if (validateMove(board, row, col, row + 2, col + 2))
                        {
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row + 2, col + 2);
                            int score = findMove(id, depth - 1, AI, temp_board, nullptr);
            
                            if (score < best_score)
                            {
                                best_score = score;
                                temp_board = getBoardCopy(board);
                                int new_row = row + 2;
                                int new_col = col + 2;
                                move(temp_board, row, col, new_row, new_col);
                                if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                                {
                                    best_score -= 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                                {
                                    best_score -= 100;
                                }

                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col + 2))
                                {
                                    best_score -= 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col - 2))
                                {
                                    best_score -= 100;
                                }

                                for (int i = 0; i < 64; i++)
                                {
                                    if (temp_board[i] != nullptr)
                                        delete temp_board[i];
                                }
                                delete[] temp_board;
                            }
                        }

                        if (validateMove(board, row, col, row + 2, col - 2))
                        {
                            temp_board = getBoardCopy(board);
                            move(temp_board, row, col, row + 2, col - 2);
                            int score = findMove(id, depth - 1, AI, temp_board, nullptr);
            
                            if (score <= best_score)
                            {
                                best_score = score;
                                temp_board = getBoardCopy(board);
                                int new_row = row + 2;
                                int new_col = col - 2;
                                move(temp_board, row, col, new_row, new_col);
                                if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col + 2))
                                {
                                    best_score -= 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row + 2, new_col - 2))
                                {
                                    best_score -= 100;
                                }

                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col + 2))
                                {
                                    best_score -= 100;
                                }
                                    if (validateMove(temp_board, new_row, new_col, new_row - 2, new_col - 2))
                                {
                                    best_score -= 100;
                                }

                                for (int i = 0; i < 64; i++)
                                {
                                    if (temp_board[i] != nullptr)
                                        delete temp_board[i];
                                }
                                delete[] temp_board;
            
                            }
                        }
                    }
                }
            }
        }




        for (int i = 0; i < 64; i++)
        {
            if (board[i] != nullptr)
                delete board[i];
        }
        delete[] board;

        if (best_score == MAX)
        {
            return MAX;
        }
        return best_score;
    }
    return -1;
}

int MiniMaxAI::getScore(Token** board)
{
    int ai_score = 0;
    int player_score = 0;
    for (int i = 0; i < 64; i++)
    {
        if (board[i] != nullptr)
        {
            if (board[i]->getPlayer() == AI)
            {
                if (board[i]->isKing())
                    ai_score += 2;
                else
                    ai_score++;
            }
            else
            {
                if (board[i]->isKing())
                    player_score += 2;
                else
                    player_score++;
            }
        }
    }
    if (player_score == 0)
        return MAX;
    else if (ai_score == 0)
        return MIN;
    else
        return ai_score - player_score;
}


bool MiniMaxAI::validateMove(Token** board, int from_row, int from_col, int to_row, int to_col)
{
    //if to or from location is out of bounds, return false
    if (from_row < 0 || from_row > 7) return false;
    if (from_col < 0 || from_col > 7) return false;
    if (to_row < 0 || to_row > 7) return false;
    if (to_col < 0 || to_col > 7) return false;
    
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
                                board[(to_row + 1) * 8 + (to_col - 1)]->~Token();
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
                                board[(to_row - 1) * 8 + (to_col - 1)]->~Token();
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
                                board[(to_row - 1) * 8 + (to_col - 1)]->~Token();
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
                                board[(to_row + 1) * 8 + (to_col - 1)]->~Token();
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
                                board[(to_row + 1) * 8 + (to_col + 1)]->~Token();
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
                                board[(to_row - 1) * 8 + (to_col + 1)]->~Token();
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
                        if (jumped != nullptr)
                        {
                            if (jumped->getPlayer() == PLAYER)
                            {
                                board[(to_row - 1) * 8 + (to_col + 1)]->~Token();
                                board[(to_row - 1) * 8 + (to_col + 1)] = nullptr;
                            }
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
                            board[(to_row + 1) * 8 + (to_col + 1)]->~Token();
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
        }
        else if (to_row == 7 && token->getPlayer() == AI && !token->isKing())
        {
            token->setKing();
        }

}