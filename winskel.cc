/*
 * File:	winxword.cc
 * Purpose:	XWorth for WxWindows
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
#include "ggame.h"
#include "gpanel.h"
#include "gdialog.h"
#include "gcanvas.h"
#include "gmenu.h"
#include "gframe.h"
#include "winskel.h"

#ifdef wx_msw
#include "mmsystem.h"
#endif

#ifndef max
#define max(a, b)	( ((a)>(b)) ? (a) : (b) )
#endif
#ifndef min
#define min(a, b)	( ((a)>(b)) ? (b) : (a) )
#endif

SkelApp     myApp; // initialise the application

#define APPSHORTNAME 	"winskel"
#define APPNAME		"Skel for Windows" 
#define DESCRIPTION     "Skel for Windows v1.0\nby Graham Wheeler gram@oms.co.za\n(c) 1996"

#define CANVASBACKGROUNDCOLOUR	0

//---------------------------------------------------------------

void SkelGame::Save(FILE *fp)
{
}

int SkelGame::Load(FILE *fp)
{
    return 0;
}

int SkelGame::Height() const
{
    return 1;
}

int SkelGame::Width() const
{
    return 1;
}

SkelGame::SkelGame()
{
}

SkelGame::~SkelGame()
{
}

//---------------------------------------------------------------

SkelMenuBar::SkelMenuBar(GameCanvas *canvas_in)
    : GameMenu(APPSHORTNAME, APPNAME, DESCRIPTION, ini,  canvas_in, 0)
{
}

SkelMenuBar::~SkelMenuBar()
{
}

//------------------------------------------------------------------------
// The right-hand panel 

SkelPanel::SkelPanel(GameFrame *f, int left, int width, int height)
    : GamePanel(f, left, width, height, 0, 18)
{
    Show(TRUE);
}

SkelPanel::~SkelPanel()
{
}

//---------------------------------------------------------------

SkelCanvas::SkelCanvas(GameFrame *frame, int w, int h)
    : GameCanvas(frame, w, h, 1)
{
    SetResource(ini, CANVASBACKGROUNDCOLOUR,"Background",	"CanvasBackground", C_WHITE	);
}

SkelCanvas::~SkelCanvas()
{
}

//------------------------------------------------------------------
// Menu Commands

int SkelFrame::AllocateGameForReading(FILE *fp)	
{
    game = new SkelGame();
    return game ? 0 : -1;
}

void SkelFrame::AllocateNewGame()	
{
    game = new SkelGame();
}

//-----------------------------------------------------------------------

GamePanel *SkelFrame::MakePanel(int x, int w, int h)
{
    return new SkelPanel(this, x, w, h);
}

GameCanvas *SkelFrame::MakeCanvas(int w, int h)
{
    return new SkelCanvas(this, w, h);
}

GameMenu *SkelFrame::MakeMenuBar()
{
    return new SkelMenuBar(canvas);
}

SkelFrame::SkelFrame()
    : GameFrame("Skel for Windows", 650, 600, "winskel", "*.skl")
{
}

SkelFrame::~SkelFrame()
{
}

//------------------------------------------------------------------------
// The `main program' equivalent, creating the windows and returning the
// main xwframe

wxFrame *SkelApp::OnInit(void)
{
    GameFrame *frame = new SkelFrame();
    frame->Init();
    return frame;
}

