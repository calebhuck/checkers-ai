#include "cMain.h"
#include "Token.h"

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Checkers App", wxPoint(0, 19), wxSize(800, 800),wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))// & wxFRAME_NO_TASKBAR
{
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxXPMHandler);

    board = new Board(8, 8);
    
    this->game = new Game(board);
    Token* token;
    BoardTile* tile;
    for (int col = 0; col < 8; col++)
    {
        for (int row = 0; row < 8; row++)
        {
            //add the tiles with a starting gamepiece
            if (row < 3 && ((row % 2 == 0 && (row * 8 + col) % 2 == 1) || (row % 2 == 1 && (row * 8 + col) % 2 == 0)))
            {
                tile = new BoardTile(game, this, 1000 + (row * 8 + col), wxPoint(col * 120, row * 120), wxSize(120, 120));
                tile->setRow(row);
                tile->setCol(col);
                token = new Token(AI, row, col);
                tile->setToken(token);
                game->addToken(token);
                board->addTile(tile);
            }
            else if (row >= 5 && ((row % 2 == 0 && (row * 8 + col) % 2 == 1) || (row % 2 == 1 && (row * 8 + col) % 2 == 0)))
            {
                tile = new BoardTile(game, this, 1000 + (row * 8 + col), wxPoint(col * 120, row * 120), wxSize(120, 120));
                tile->setRow(row);
                tile->setCol(col);
                token = new Token(PLAYER, row, col);
                tile->setToken(token);
                game->addToken(token);
                board->addTile(tile);
            }
            else
            {
                tile = new BoardTile(game, this, 1000 + (row * 8 + col), wxPoint(col * 120, row * 120), wxSize(120, 120));
                tile->setRow(row);
                tile->setCol(col);
                //board[row * 8 + col] = tile;
                board->addTile(tile);
            }

            //set background for tiles in checkered pattern
            if ((row % 2 == 0 && (row * 8 + col) % 2 == 0) || (row % 2 == 1 && (row * 8 + col) % 2 == 1))
            {
                //set back groundblack
                //board[row * 8 + col]->SetBackgroundColour(wxColour(0, 0, 0));
                tile->SetBackgroundColour(wxColour(160, 0, 0));
            }
            else
            {
                //set background color red
                //board[row * 8 + col]->SetBackgroundColour(wxColour(160, 0, 0));
                tile->SetBackgroundColour(wxColour(0, 0, 0));
            }   
        }
    }
    //set the usable area to fit the board

    //update_timer = wxTimer();
    //update_timer = wxTimer(this);
    //update_timer.SetOwner(this, -1);
    //update_timer.Start(30, false);
    //this->SetSize(960, 960);
    //update_timer.Bind(wxEVT_TIMER, &cMain::updateTiles, this);
}
cMain::~cMain()
{
    game->~Game();
}

//void cMain::updateTiles(wxTimerEvent& evt)
//{
//    for (int i = 0; i < 8; i++)
//    {
//        for (int j = 0; j < 8; j++)
//        {
//            board->getTile(i, j)->Refresh();
//            board->getTile(i, j)->Update();
//        }
//    }
//}


//BEGIN_EVENT_TABLE(cMain, wxFrame)
//    EVT_TIMER(-1, cMain::updateTiles)
//END_EVENT_TABLE()