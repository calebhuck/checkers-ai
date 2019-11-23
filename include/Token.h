#if !defined TOKEN_H
#define TOKEN_H

enum PlayerType
{
    PLAYER = 0,
    AI = 1,
    NONE = -1
};

class Token
{
    public:
        Token(PlayerType player, int row, int col);
        Token(Token* old_token);
        ~Token();
        void setKing();
        bool isKing();
        PlayerType getPlayer();
        int getID();
        void setID(int id);
        void setRow(int row);
        void setCol(int col);
        int getRow();
        int getCol();

    private:
        int row;
        int col;
        bool is_king;
        int id;
        PlayerType player;

};

#endif