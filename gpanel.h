/*
 * File:	gpanel.h
 * Purpose:	Games panel base class
 * Author:	Graham Wheeler
 * Created:	1996
 * Updated:	
 * Copyright:	(c) 1996, Graham Wheeler
 */
//------------------------------------------------------------------------

#ifndef GPANEL_H
#define GPANEL_H

#define BUTTON_PASS		800
#define BUTTON_AUTOPLAY		801
#define BUTTON_AUTOMOVE		802
#define BUTTON_STOPAUTO		803
#define BUTTON_DONE		804
#define BUTTON_UNDO		805
#define BUTTON_REDO		806
#define BUTTON_HINT		807
#define BUTTON_NEXT		808

class Game;

class GamePanel : public wxPanel
{
  protected:
    wxMessage **lbl;
    int nlabels;
    static void button_proc(wxButton& but, wxCommandEvent& event);
  public:
    GamePanel(wxFrame *f, int left, int width, int height,
		char *buttons, int nlabels);
    void SetLabel(int n, char *buf);
    void ClearLabel(int n);
    void ClearLabels(int start = 0, int end = -1);
    virtual void HandleButton(int bnum);
    virtual ~GamePanel();
};

inline void GamePanel::ClearLabel(int n)
{
    SetLabel(n, "");
}

#endif

