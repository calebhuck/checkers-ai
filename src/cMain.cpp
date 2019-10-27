#include "cMain.h"
//#include "Board.h"
#include "Token.h"

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Checkers App", wxPoint(50, 50), wxSize(800, 800),wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))// & wxFRAME_NO_TASKBAR
{
    //wxImage::AddHandler(new wxICOHandler);
    wxImage::AddHandler(new wxPNGHandler);
    board = new BoardTile*[64];
    this->game = new Game(board);
    Token* token;
    BoardTile* tile;
    for (int col = 0; col < 8; col++)
    {
        for (int row = 0; row < 8; row++)
        {
            //add the tiles with a starting gamepiece
            if (row < 3 && ((row % 2 == 0 && (row * 8 + col) % 2 == 0) || (row % 2 == 1 && (row * 8 + col) % 2 == 1)))
            {
                tile = new BoardTile(board, game, this, 1000 + (row * 8 + col), wxPoint(col * 120, row * 120), wxSize(120, 120));
                tile->setRow(row);
                tile->setCol(col);
                token = new Token(row * 8 + col, AI, row, col);
                tile->setToken(token);
                board[row * 8 + col] = tile;
            }
            else if (row >= 5 && ((row % 2 == 0 && (row * 8 + col) % 2 == 0) || (row % 2 == 1 && (row * 8 + col) % 2 == 1)))
            {
                tile = new BoardTile(board, game, this, 1000 + (row * 8 + col), wxPoint(col * 120, row * 120), wxSize(120, 120));
                tile->setRow(row);
                tile->setCol(col);
                token = new Token(row * 8 + col, PLAYER, row, col);
                tile->setToken(token);
                board[row * 8 + col] = tile;
            }
            else
            {
                tile = new BoardTile(board, game, this, 1000 + (row * 8 + col), wxPoint(col * 120, row * 120), wxSize(120, 120));
                tile->setRow(row);
                tile->setCol(col);
                board[row * 8 + col] = tile;
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
                board[row * 8 + col]->SetBackgroundColour(wxColour(160, 0, 0));
            }   
        }
    }
    //set the usable area to fit the board
    this->SetSize(960, 960);
    CreateStatusBar();
    SetStatusText( "Welcome to wxWidgets!" );
}
cMain::~cMain()
{
    delete[] board;
    delete game;
}