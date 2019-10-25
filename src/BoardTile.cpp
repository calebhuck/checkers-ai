#include <wx/wx.h>
//#include "Game.h"
#include "BoardTile.h"

BoardTile::BoardTile(BoardTile** board, Game* game, bool player1_present, bool player2_present, wxFrame* parent, wxWindowID id, wxPoint pos, wxSize size) : wxPanel(parent, id, pos, size)
{
    this->player1_present = player1_present;
    this->player2_present = player2_present;
    this->board = board;
    this->game = game;
    this->selected = false;
}

 BoardTile::~BoardTile()
 {
 }

BEGIN_EVENT_TABLE(BoardTile, wxPanel)
    EVT_LEFT_DOWN(BoardTile::onClick)
    EVT_PAINT(BoardTile::paintEvent)
END_EVENT_TABLE()

void BoardTile::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
    evt.Skip();
}

void BoardTile::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void BoardTile::render(wxDC&  dc)
{
    if (selected)
    {
        dc.SetPen(wxPen( wxColor(5,199,0), 5) );
        dc.DrawLine(wxPoint(0, 0), wxPoint(0, 120));
        dc.DrawLine(wxPoint(0, 0), wxPoint(120, 0));
        dc.DrawLine(wxPoint(120, 0), wxPoint(120, 120));
        dc.DrawLine(wxPoint(0, 120), wxPoint(120, 120));
    }
    else
    {
        dc.Clear();
    }
    


    if (player1_present)
    {
        dc.DrawIcon(wxIcon("./img/game_piece.png", wxBITMAP_TYPE_PNG, 130, 130), wxPoint(25, 25));
    }

    else if (player2_present)
    {
        dc.DrawIcon(wxIcon("./img/game_piece3.png", wxBITMAP_TYPE_PNG, 130, 130), wxPoint(25, 25));
    }
}

void BoardTile::onClick(wxMouseEvent& event)
{
    if ((player1_present || player2_present) && this->selected)
    {
        this->selected = !this->selected;
    }

    else if ((player1_present || player2_present) && !this->selected)
    {
        this->selected = !this->selected;
    }
        
    this->paintNow();
}