#include "BoardTile.h"
#include <wx/gdicmn.h>

BoardTile::BoardTile(Game *game, wxFrame *parent, wxWindowID id, wxPoint pos,
                     wxSize size)
    : wxPanel(parent, id, pos, size) {
  // red_token = wxIcon("./img/token_red.png", wxBITMAP_TYPE_PNG, 170, 170);
  // red_king = wxIcon("./img/token_red_king.png", wxBITMAP_TYPE_PNG, 170, 170);
  // white_token = wxIcon("./img/token_white.png", wxBITMAP_TYPE_PNG, 170, 170);
  // white_king = wxIcon("./img/token_white_king.png", wxBITMAP_TYPE_PNG, 170,
  // 170);
  this->game = game;
  this->selected = false;
  this->token_present = false;
  token = nullptr;
}

BoardTile::~BoardTile() {}

BEGIN_EVENT_TABLE(BoardTile, wxPanel)
EVT_LEFT_DOWN(BoardTile::onClick)
EVT_PAINT(BoardTile::paintEvent)
END_EVENT_TABLE()

void BoardTile::paintEvent(wxPaintEvent &evt) {
  wxPaintDC dc(this);
  render(dc);
}

void BoardTile::paintNow() {
  wxClientDC dc(this);
  render(dc);
}

void BoardTile::render(wxDC &dc) {
  if (selected) {
    dc.SetPen(wxPen(wxColor(5, 199, 0), 5));
    dc.DrawLine(wxPoint(0, 0), wxPoint(0, 120));
    dc.DrawLine(wxPoint(0, 0), wxPoint(120, 0));
    dc.DrawLine(wxPoint(120, 0), wxPoint(120, 120));
    dc.DrawLine(wxPoint(0, 120), wxPoint(120, 120));
  } else {
    dc.Clear();
  }

  if (token_present) {
    if (token->getPlayer() == PLAYER) {
      if (!token->isKing()) {
        dc.DrawIcon(wxIcon("/home/caleb/workspace/checkers-ai/"
                           "resources/test.png",
                           wxBITMAP_TYPE_PNG),
                    wxPoint(0, 0));
      } else {
        dc.DrawIcon(wxIcon("/home/caleb/workspace/checkers-ai/"
                           "resources/token_red_king.png",
                           wxBITMAP_TYPE_ANY, 170, 170),
                    wxPoint(0, 0));
      }
    }

    else if (token->getPlayer() == AI) {
      wxIcon icon("/home/caleb/workspace/checkers-ai/"
                  "resources/token_white.png",
                  wxBITMAP_TYPE_PNG);
      icon.SetWidth(17);
      icon.SetHeight(17);
      dc.DrawIcon(icon, wxPoint(0, 0));
      if (!token->isKing()) {
      } else {
        dc.DrawIcon(wxIcon("/home/caleb/workspace/checkers-ai/"
                           "resources/token_white_king.png",
                           wxBITMAP_TYPE_PNG, 50, 50),
                    wxPoint(0, 0));
      }
    }
  }
}

void BoardTile::onClick(wxMouseEvent &event) {
  game->reportClick(this);
  event.Skip();
}

void BoardTile::setSelected() { this->selected = true; }

void BoardTile::setUnselected() { this->selected = false; }

bool BoardTile::isSelected() { return selected; }

void BoardTile::setRow(int row) { this->row = row; }

void BoardTile::setCol(int col) { this->col = col; }

int BoardTile::getRow() { return this->row; }

int BoardTile::getCol() { return this->col; }

void BoardTile::setToken(Token *token) {
  this->token = token;
  token_present = true;
}

bool BoardTile::isTokenPresent() { return token_present; }

Token *BoardTile::getToken() {
  if (token_present)
    return token;
  else
    return nullptr;
}

void BoardTile::removeToken() {
  token = nullptr;
  token_present = false;
  selected = false;
  paintNow();
}
