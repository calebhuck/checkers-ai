#include "Token.h"
#include <iostream>

Token::Token(PlayerType player, int row, int col)
{
    this->player = player;
    this->row = row;
    this->col = col;
    this->is_king = false;
}

Token::Token(Token* old_token)
{
    this->row = old_token->getRow();
    this->col = old_token->getCol();
    this->id = old_token->getID();
    this->player = old_token->getPlayer();
    this->is_king = old_token->isKing();
}

Token::~Token()
{

}

bool Token::isKing()
{
    return this->is_king;
}

void Token::setKing()
{
    this->is_king = true;
}

PlayerType Token::getPlayer()
{
    return this->player;
}

void Token::setID(int id)
{
    this->id = id;
}

int Token::getID()
{
    return this->id;
}

void Token::setRow(int row)
{
    this->row = row;
}

void Token::setCol(int col)
{
    this->col = col;
}

int Token::getRow()
{
    return this->row;
}

int Token::getCol()
{
    return this->col;
}