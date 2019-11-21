#if !defined BOARDTILE_H
#define BOARDTILE_H
#include "Token.h"
#include "Game.h"
#include <wx/wx.h>

//fixes mutual inclusion problem 
class Game;

class BoardTile: public wxPanel
{
    public:
        BoardTile(Game* game, wxFrame* parent, wxWindowID id, wxPoint pos, wxSize size);
        BoardTile(BoardTile* old_tile);
        ~BoardTile();

        void paintEvent(wxPaintEvent & evt);
        void paintNow();
        void render(wxDC& dc);
        void onClick(wxMouseEvent& event);
        void setSelected();
        void setUnselected();
        bool isSelected();
        void setRow(int row);
        void setCol(int col);
        int getRow();
        int getCol();
        void setToken(Token* token);
        bool isTokenPresent();
        Token* getToken();
        void removeToken();

        DECLARE_EVENT_TABLE();

    private:
        bool selected;
        bool token_present;
        Token* token;
        Game* game;
        int row;
        int col;
};

#endif