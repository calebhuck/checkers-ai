#if !defined BOARDTILE_H
#define BOARDTILE_H
#include "Game.h"
#include <wx/wx.h>

class BoardTile: public wxPanel
{
    public:
        BoardTile(BoardTile** board, Game* game, bool player1_present, bool player2_present, wxFrame* parent, wxWindowID id, wxPoint pos, wxSize size);
        ~BoardTile();

        void paintEvent(wxPaintEvent & evt);
        void paintNow();
        void render(wxDC& dc);
        void onClick(wxMouseEvent& event);

        DECLARE_EVENT_TABLE();

    private:
        bool selected;
        //cannot both be true
        bool player1_present;
        bool player2_present;
        //change to Board**
        BoardTile** board;
        Game* game;
};

#endif