#include <wx/wx.h>
#include "cMain.h"
#include "CheckersApp.h"

wxIMPLEMENT_APP(CheckersApp);

bool CheckersApp::OnInit()
{
    m_frame = new cMain();
    m_frame->SetClientSize(960, 960);
    m_frame->Show();
    return true;
}

CheckersApp::CheckersApp()
{
}

CheckersApp::~CheckersApp()
{
}