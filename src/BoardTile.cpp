#include "BoardTile.h"

BoardTile::BoardTile(Game* game, wxFrame* parent, wxWindowID id, wxPoint pos, wxSize size) : wxPanel(parent, id, pos, size)
{
    this->game = game;
    this->selected = false;
    this->token_present = false;
    token = nullptr;
}

BoardTile::BoardTile(BoardTile* old_tile)
{
    std::cout << "BOARDTILE CONSTRUCTOR\n";
    this->game = nullptr;
    this->token_present = old_tile->isTokenPresent();
    std::cout << "made it here.........\n";
    if (token_present)
    {
        //std::cout << "creating copy token\n";
        //this->token = new Token(old_tile->getToken());
    }
    else
    {
        //std::cout << "token is not present, set nullptr\n";
        this->token = nullptr;
    } 
    std::cout << "exiting boardTIle constructor\n";
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
    


    if (token_present)
    {
        if (token->getPlayer() == PLAYER)
        {
            if (!token->isKing())
                dc.DrawIcon(wxIcon("./img/token_red.png", wxBITMAP_TYPE_PNG, 170, 170), wxPoint(0, 0));
            else                
                dc.DrawIcon(wxIcon("./img/token_red_king.png", wxBITMAP_TYPE_PNG, 170, 170), wxPoint(0, 0));
        }

        else if (token->getPlayer() == AI)
        { 
            if (!token->isKing())
                dc.DrawIcon(wxIcon("./img/token_white.png", wxBITMAP_TYPE_PNG, 170, 170), wxPoint(0, 0));
            else                
                dc.DrawIcon(wxIcon("./img/token_white_king.png", wxBITMAP_TYPE_PNG, 170, 170), wxPoint(0, 0));
        }
    }
}

void BoardTile::onClick(wxMouseEvent& event)
{    
    game->reportClick(this);
    this->paintNow();
    event.Skip();
}

void BoardTile::setSelected()
{
    this->selected = true;
}

void BoardTile::setUnselected()
{
    this->selected = false;
}

bool BoardTile::isSelected()
{
    return  selected;
}

void BoardTile::setRow(int row)
{
    this->row = row;
}

void BoardTile::setCol(int col)
{
    this->col = col;
}

int BoardTile::getRow()
{
    return this->row;
}

int BoardTile::getCol()
{
    return this->col;
}


void BoardTile::setToken(Token* token)
{
    this->token = token;
    token_present = true;
}

bool BoardTile::isTokenPresent()
{
    return token_present;
}

Token* BoardTile::getToken()
{
    if (token_present)
        return token;
    else 
        return nullptr;
}

void BoardTile::removeToken()
{
    token = nullptr;
    token_present = false;
    selected = false;
    paintNow();
}