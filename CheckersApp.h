class CheckersApp: public wxApp
{
public:
    CheckersApp();
    ~CheckersApp();
    virtual bool OnInit();
private:
    cMain* m_frame;
};
