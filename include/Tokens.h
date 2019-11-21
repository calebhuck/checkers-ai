#if !defined TOKENS_H
#define TOKENS_H
#include <iostream>
#include "Token.h"

class Tokens
{
    public:
        Tokens(int max_num_tokens, PlayerType player);
        ~Tokens();
        void add(Token* token);
        void remove(Token* token);
        int getNumTokens();
        Token* getToken(int id);

    private:
        int num_tokens;
        PlayerType player;
        Token** tokens;

};

#endif