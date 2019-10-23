#include <wx/wx.h>
#include "Game.h"
#include "BoardTile.h"
#include "Board.h"
#include "cMain.h"

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Checkers App", wxPoint(50, 50), wxSize(800, 800),wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))// & wxFRAME_NO_TASKBAR
{
    //wxImage::AddHandler(new wxICOHandler);
    wxImage::AddHandler(new wxPNGHandler);
    this->game = new Game();
    board = new BoardTile*[64];
    for (int col = 0; col < 8; col++)
    {
        for (int row = 0; row < 8; row++)
        {
            //add the tiles with a starting gamepiece
            if (row < 3 && ((row % 2 == 0 && (row * 8 + col) % 2 == 0) || (row % 2 == 1 && (row * 8 + col) % 2 == 1)))
            {
                board[row * 8 + col] = new BoardTile(board, game, true, false, this, 1000 + (row * 8 + col), wxPoint(col * 120, row * 120), wxSize(120, 120));
            }
            else if (row >= 5 && ((row % 2 == 0 && (row * 8 + col) % 2 == 0) || (row % 2 == 1 && (row * 8 + col) % 2 == 1)))
            {
                board[row * 8 + col] = new BoardTile(board, game, false, true, this, 1000 + (row * 8 + col), wxPoint(col * 120, row * 120), wxSize(120, 120));

            }
            else
            {
                board[row * 8 + col] = new BoardTile(board, game, false, false, this, 1000 + (row * 8 + col), wxPoint(col * 120, row * 120), wxSize(120, 120));
            }

            //set background for tiles in checkered pattern
            if ((row % 2 == 0 && (row * 8 + col) % 2 == 0) || (row % 2 == 1 && (row * 8 + col) % 2 == 1))
            {
                //set back groundblack
                board[row * 8 + col]->SetBackgroundColour(wxColour(0, 0, 0));
            }
            else
            {
                //set background color red
                board[row * 8 + col]->SetBackgroundColour(wxColour(255, 0, 0));
            }   
        }
    }
    //set the usable area to fit the board
    this->SetSize(960, 960);
}
cMain::~cMain()
{
    delete[] board;
    delete game;
}