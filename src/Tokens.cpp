#include "Tokens.h"

Tokens::Tokens(int max_num_tokens, PlayerType player)
{
    this->num_tokens = 0;
    this->player = player;
    this->tokens = new Token*[max_num_tokens];
    for (int i = 0; i < num_tokens; i++)
    {
        this->tokens[i] = nullptr;
    }
}
Tokens::~Tokens()
{
    delete[] tokens;
}
void Tokens::add(Token* token)
{
    token->setID(num_tokens);
    tokens[num_tokens] = token;
    num_tokens++;
}
void Tokens::remove(Token* token)
{
    tokens[token->getID()] = nullptr;
    num_tokens--;
}
int Tokens::getNumTokens()
{
    return num_tokens;
}

Token* Tokens::getToken(int id)
{
    if (id <= 12 && id >= 0)
        return tokens[id];
    else
    {
        return nullptr;
    }
}