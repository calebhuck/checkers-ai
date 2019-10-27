#if !defined TOKEN_H
#define TOKEN_H

enum PlayerType
{
    PLAYER = 0,
    AI = 1
};

class Token
{
    public:
        Token(int id, PlayerType player, int row, int col);
        ~Token();
        void setKing();
        bool isKing();
        PlayerType getPlayer();

    private:
        int row;
        int col;
        bool is_king;
        int id;
        PlayerType player;

};

#endif