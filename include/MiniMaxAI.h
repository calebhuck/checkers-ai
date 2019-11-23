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
        Token* getBoardToken(Token** board, int row, int col);
    
    private: 
        int getScore(Token** board);
        int findMove(int id, int depth, PlayerType player, Token** board, AIMove** moves);
        Token** getBoardCopy(Token** old_board);
        Game* game;
        Tokens* ai_tokens;
        Tokens* player_tokens;
        std::thread workers[12];
        const wxMouseEvent evt = wxMouseEvent(wxEVT_NULL);;
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