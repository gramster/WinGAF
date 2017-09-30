/*
 * File:	gdialog.cc
 * Purpose:	wxDialogBox wrapper class for self-containment
 * Author:	Graham Wheeler
 * Created:	1996
 * Updated:	
 * Copyright:	(c) 1996, Graham Wheeler
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx.h"
#include "wx_timer.h"
#include "gdialog.h"

#ifdef wx_msw
#include "mmsystem.h"
#endif

//--------------------------------------------------------------------

GameDialog::GameDialog(const char *title)
    : wxDialogBox(0, (char*)title, TRUE)
{
}

wxSlider *GameDialog::MakeSlider(char *lbl, int v, int mn, int mx, int nl,
				wxFunction cb)
{
    wxSlider *rtn = new wxSlider(this,cb,lbl,v,mn,mx,150,-1,-1,wxFIXED_LENGTH);
    rtn->SetClientData((char *)this);
    if (nl) NewLine();
    return rtn;
}

void GameDialog::AddButton(int bnum, const char *label, int is_default)
{
    wxButton *btn;
    btn = new wxButton(this, (wxFunction)&button_proc, (char*)label);
    btn->SetClientData((char*)bnum);
    if (is_default) btn->SetDefault();
}

void GameDialog::HandleButton(int bnum)
{
    if (bnum == 0) Show(FALSE); // reserved for cancel
}

void GameDialog::button_proc(wxButton& but, wxCommandEvent& event)
{
    GameDialog *dialog = (GameDialog*)but.GetParent();
    int btn = (int)but.GetClientData();
    dialog->HandleButton(btn);
}

GameDialog::~GameDialog()
{
}

//---------------------------------------------------------------------

FileBrowser::FileBrowser(char *title, char *fname)
    : wxFrame(0, title, -1, -1, 200, 400)
{
  strcpy(fn, fname);
  wxTextWindow *tw = new wxTextWindow(this,-1,-1,-1,-1,wxREADONLY);
  tw->SetFont(wxTheFontList->FindOrCreateFont(12, wxSWISS, wxNORMAL, wxNORMAL));
  tw->LoadFile(fname);
}

void RunDialog(wxDialogBox *dlg)
{
    dlg->Show(TRUE);
    delete dlg;
}

//--------------------------------------------------------------------
// Background process manager

#define MAX_CYCLERS  8
static Cycler cyclers[MAX_CYCLERS] = { 0 };
static void *cycargs[MAX_CYCLERS] = { 0 };

class CycleTimer : public wxTimer
{
  public:
    CycleTimer(int interval = 100)
	: wxTimer()
    {
	Start(interval);
    }
    virtual void Notify();
    virtual ~CycleTimer()
    { }
};

static CycleTimer *bgtimer = 0;

int AddCycler(Cycler fp, void *arg)
{
    if (bgtimer == 0)
	bgtimer = new CycleTimer();
    for (int i = 0; i < MAX_CYCLERS; i++)
	if (cyclers[i] == 0)
	{
	    cyclers[i] =fp;
	    cycargs[i] = arg;
	    return 0;
	}
    return -1;
}

int RemoveCycler(Cycler fp)
{
    for (int i = 0; i < MAX_CYCLERS; i++)
	if (cyclers[i] == fp)
	{
	    cyclers[i] = 0;
	    return 0;
	}
    return -1;
}

int RunCycler(Cycler fp)
{
    for (int i = 0; i < MAX_CYCLERS; i++)
	if (cyclers[i] == fp)
	{
	    (cyclers[i])(cycargs[i]);
	    return 0;
	}
    return -1;
}

void BackgroundProcess(wxApp *a)
{
    (void)a;
    for (int i = 0; i < MAX_CYCLERS; i++)
	if (cyclers[i])
	    (cyclers[i])(cycargs[i]);
}

void CycleTimer::Notify()
{
    BackgroundProcess(0);
}

