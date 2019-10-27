#if !defined GAME_H
#define GAME_H

class cMain;
#include "cMain.h"
//fixes mutual inclusion problem
class BoardTile;

#include "BoardTile.h"

class Game
{
    public:
        Game(BoardTile** board);
        ~Game();
        void reportClick(BoardTile* tile);
        BoardTile* getTile(int row, int col);
        bool validateMove(int row_1, int col_1, int row_2, int col_2);
        bool move(BoardTile* from, BoardTile* to);

    private:
        BoardTile** board;
        bool is_a_tile_selected;
        BoardTile* selected;
        int num_player_tokens;
        int num_ai_tokens;
};

#endif