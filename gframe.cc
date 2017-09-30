/*
 * File:	gframe.cc
 * Purpose:	Game Frame base class
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
//#include "wx_timer.h"
#include "ggame.h"
#include "gpanel.h"
#include "gdialog.h"
#include "gcanvas.h"
#include "gmenu.h"
#include "gframe.h"

#ifdef wx_msw
#include "mmsystem.h"
#endif

void GameFrame::WriteGame(FILE *fp)
{
    game->Save(fp);
}

int GameFrame::ReadGame(FILE *fp)
{
    return game->Load(fp);
}

//-----------------------------------------------------------------------
// Menu bar handlers

void GameFrame::LoadGame()
{
    if (game)
    {
	if (!game->GameOver() &&
	    wxMessageBox("Discard current game?", "Confirm", wxYES_NO|wxCANCEL,
				this) != wxYES)
	    return;
	delete game;
	game = 0;
    }
    menubar->SetGame(0);
    canvas->SetGame(0);
    char suf[10];
    strcpy(suf, fsuf);
    char *s = wxFileSelector("Load game file", 0, 0, 0, suf);
    if (s)
    {
	FILE *fp = fopen(s, "r");
	int rtn = -1;
	if (fp != 0)
	{
	    if (AllocateGameForReading(fp) == 0) 
	    {
	        canvas->SetGame(game);
	        menubar->SetGame(game);
		canvas->DrawBoard();
		rtn = ReadGame(fp);
	    }
	    fclose(fp);
	}
	if (rtn != 0)
	{
            (void)wxMessageBox("Failed to open game file or replay moves!",
            	        "Load failed", wxOK|wxCENTRE);
	    delete game;
	    game = 0;
	}
    }
    canvas->SetGame(game);
    menubar->SetGame(game);
    canvas->OnPaint();
}

void GameFrame::NewGame()
{
    if (game)
    {
	if (!game->GameOver() &&
	    wxMessageBox("Discard current game?", "Confirm", wxYES_NO|wxCANCEL,
				this) != wxYES)
	    return;
	delete game;
	game = 0;
	canvas->SetGame(0);
	menubar->SetGame(game);
    }
    AllocateNewGame(); 
    canvas->SetGame(game);
    menubar->SetGame(game);
    canvas->OnPaint();
}

void GameFrame::SaveGame()
{
    if (game)
    {
	char suf[10];
	strcpy(suf, fsuf);
        char *s = wxFileSelector("Save game file", 0, 0, 0, suf);
	if (s)
	{
	    FILE *fp = fopen(s, "w");
	    if (fp)
	    {
		WriteGame(fp);
		fclose(fp);
	    }
	}
    }
}

//-----------------------------------------------------------------------
// Frame windoze event handlers

void GameFrame::OnActivate(Bool active)
{
    if (active == TRUE) canvas->SetFocus();
}

void GameFrame::OnSize(int x, int y)
{
    int width, height;
    GetClientSize(&width, &height);
    int pw = width/3;
    if (pw > 250) pw = 250;
    if (panel) panel->SetSize(width-pw, 0, pw, height);
    if (canvas)
    {
	canvas->Resize(width-pw, height);
	canvas->Clear();
	canvas->DrawBoard();
    }
}

void GameFrame::OnMenuCommand(int id)
{
    if (menubar->Handle(id))
    {
	OnClose();
        delete this;
    }
}

Bool GameFrame::OnClose(void)
{
  Show(FALSE);
  return TRUE;
}

//-------------------------------------------------------------------------

GameFrame::GameFrame(char *title, int w, int h, const char *icon_in, 
			const char *fsuf_in)
    : wxFrame(0, title, 0, 0, w, h, wxSDI|wxDEFAULT_FRAME),
      panel(0),
      canvas(0),
      menubar(0),
      game(0),
      icon(icon_in),
      fsuf(fsuf_in)
{
    CreateStatusLine(1); // Give it a status line
}

void GameFrame::Init()
{
    int width, height;
    GetClientSize(&width, &height);
    canvas = MakeCanvas(width*2/3, height);
    panel = MakePanel(width*2/3, width/3, height);
    menubar = MakeMenuBar();
    menubar->Init();
    SetMenuBar(menubar);
    //LoadAccelerators("menus_accel") ;
    canvas->Show(TRUE);
    panel->Show(TRUE);
    // Give it an icon
    char icnm[20];
#ifdef wx_msw
    sprintf(icnm, "%s_icn", icon);
#endif
#ifdef wx_x
    sprintf(icnm, "%s.xbm", icon);
#endif
    SetIcon(new wxIcon(icnm));
    Show(TRUE);
    SetStatusText("Welcome!");
}

GameFrame::~GameFrame()
{
}


