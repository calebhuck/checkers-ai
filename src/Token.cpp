#include "Token.h"

Token::Token(int id, PlayerType player, int row, int col)
{
    this->id = id;
    this->player = player;
    this->row = row;
    this->col = col;
    this->is_king = false;
}

Token::~Token()
{

}

bool Token::isKing()
{
    return is_king;
}

void Token::setKing()
{
    is_king = true;
}

PlayerType Token::getPlayer()
{
    return player;
}