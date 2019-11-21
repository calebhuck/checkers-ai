#if !defined GAME_H
#define GAME_H

#include <chrono>
#include <thread>
#include "Tokens.h"
class MiniMaxAI;
#include "MiniMaxAI.h"
class cMain;
#include "cMain.h"
class Board;
#include "Board.h"
//fixes mutual inclusion problem TRY REMOVING***********************
class BoardTile;
#include "BoardTile.h"

class Game
{
    public:
        Game(Board* board);
        //Game(Game* game);
        ~Game();
        void reportClick(BoardTile* tile);
        BoardTile* getTile(int row, int col);
        bool validateMove(int row_1, int col_1, int row_2, int col_2);
        bool move(BoardTile* from, BoardTile* to);
        void start();
        Tokens* getAITokens();
        Tokens* getPlayerTokens();
        PlayerType getCurrentPlayer();
        Board* getBoard();
        void addToken(Token* token);

    private:
        Board* board;
        bool is_a_tile_selected;
        BoardTile* selected;
        PlayerType current_player;
        std::thread game_loop;
        MiniMaxAI* ai;
        Tokens* ai_tokens;
        Tokens* player_tokens;
};

#endif