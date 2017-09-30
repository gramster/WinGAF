/*
 * File:	gdialog.h
 * Purpose:	wxDialogBox wrapper class for self-containment
 * Author:	Graham Wheeler
 * Created:	1996
 * Updated:	
 * Copyright:	(c) 1996, Graham Wheeler
 */

#ifndef GDIALOG_H
#define GDIALOG_H

#ifdef __GNUG__
#pragma interface
#endif

class GameDialog : public wxDialogBox
{
    static void button_proc(wxButton& but, wxCommandEvent& event);
  protected:
    void AddButton(int bnum, const char *label, int is_default=0);
    wxSlider *MakeSlider(char *lbl, int v, int mn, int mx, int nl=1,
			wxFunction cb = (wxFunction)0);
  public:
    GameDialog(const char *title);
    virtual void HandleButton(int bnum);
    virtual ~GameDialog();
};

class FileBrowser : public wxFrame
{
    char fn[256];
public:
    FileBrowser(char *title, char *fname);
    Bool OnClose() { Show(FALSE); unlink(fn); return TRUE; }
};

void RunDialog(wxDialogBox *dlg);

// Background processing

typedef void (*Cycler)(void *);
int AddCycler(Cycler fp, void *arg);
int RemoveCycler(Cycler fp);
int RunCycler(Cycler fp);
void BackgroundProcess(wxApp *a);

#endif


