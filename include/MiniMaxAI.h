#if !defined MINIMAXAI_H
#define MINIMAXAI_H

struct AIMove;
class Game;
#include "Game.h"
#include <thread>
#include <chrono>

class MiniMaxAI
{
    public:
        MiniMaxAI(Game* game);
        ~MiniMaxAI();
        void get_move();
        bool validateMove(Token** board, int from_row, int from_col, int to_row, int to_col);
        void move(Token** board, int from_row, int from_col, int to_row, int to_col);

    
    private: 
        int getScore(Token** board);
        void findMove(int id, Token** board, AIMove** moves);
        Game* game;
        Tokens* ai_tokens;
        Tokens* player_tokens;
        //Token** board;
        std::thread workers[12];
};

struct AIMove
{
    int from_row;
    int from_col;
    int to_row;
    int to_col;
    int score;
};

#endif