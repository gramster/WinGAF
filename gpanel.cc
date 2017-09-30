/*
 * File:	gpanel.cc
 * Purpose:	Games panel base class
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
#include "gpanel.h"

#ifdef wx_msw
#include "mmsystem.h"
#endif

void GamePanel::button_proc(wxButton& but, wxCommandEvent& event)
{
    GamePanel *panel = (GamePanel*)but.GetParent();
    int btn = (int)but.GetClientData();
    panel->HandleButton(btn);
}

GamePanel::GamePanel(wxFrame *f, int left, int width, int height,
			char *buttons, int nlabels_in)
    : wxPanel(f, left, 0, width, height, 0, "GamePanel"), nlabels(nlabels_in)
{
    Show(FALSE);
    SetLabelPosition(wxVERTICAL);
    SetLabelColour(*wxBLACK);
    SetLabelFont(wxTheFontList->FindOrCreateFont(
				  12, wxMODERN, wxNORMAL, wxNORMAL));
    SetButtonFont(wxTheFontList->FindOrCreateFont(
				  11, wxROMAN, wxNORMAL, wxNORMAL));
    wxButton **bptr = 0;
    if (buttons)
    {
        char *buf = new char[strlen(buttons)+1];
	strcpy(buf, buttons);
	int nb = 0, ml = 0;
	for (char *s = strtok(buf, "@"); s; s = strtok(0, "@"))
	{
	    int l = strlen(s);
	    if (l > ml) ml = l;
	    nb++;
	}
	char *xxx = new char[ml+1];
	for (int i = 0; i < ml; i++) xxx[i] = 'X';
	xxx[ml] = 0;
	strcpy(buf, buttons);
	bptr = new wxButton*[nb];
        for (int i = 0; i < nb; i++)
	{
	    wxButton *btn = new wxButton(this, (wxFunction)&button_proc, xxx,
				-1, -1, -1, -1, wxFIXED_LENGTH);
	    bptr[i] = btn;
	    btn->SetClientData((char*)i);
	    if (i%2==0) Tab(); else NewLine();
	}
	delete [] xxx;
	delete [] buf;
	if (nb%2) NewLine();
    }
    lbl = new wxMessage *[nlabels];
    for (int i = 0; i < nlabels; i++)
    {
        lbl[i] = new wxMessage(this, " ");
        NewLine();
    }
    Fit();
    if (buttons)
    {
        char *buf = new char[strlen(buttons)+1];
	strcpy(buf, buttons);
	int nb = 0;
	for (char *s = strtok(buf, "@"); s; s = strtok(0, "@"))
	    bptr[nb++]->SetLabel(s);
	delete [] buf;
	delete [] bptr;
    }
}

void GamePanel::SetLabel(int n, char *buf)
{
    lbl[n]->SetLabel(buf);
}

void GamePanel::ClearLabels(int start, int end)
{
    if (end < 0) end = nlabels-1;
    while (start <= end) ClearLabel(start++);
}

void GamePanel::HandleButton(int bnum)
{
    (void)bnum;
}

GamePanel::~GamePanel()
{
}



