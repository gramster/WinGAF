/*
 * File:	gtoolb.cc
 * Purpose:	General purpose game toolbar class
 * Author:	Graham Wheeler
 * Created:	1998
 * Updated:	
 * Copyright:	(c) 1998, Graham Wheeler
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx.h"
#include "wx_bbar.h"
//#include "ggame.h"
//#include "gdialog.h"
//#include "gcanvas.h"
#include "gframe.h"
#include "gtoolb.h"

#ifdef wx_msw
#include "mmsystem.h"
#endif

#ifndef max
#define max(a, b)	( ((a)>(b)) ? (a) : (b) )
#endif
#ifndef min
#define min(a, b)	( ((a)>(b)) ? (b) : (a) )
#endif


GameToolbar::GameToolbar(GameFrame *parent_in)
    : wxButtonBar((wxFrame*)(parentframe = parent_in), -1, -1, -1, 40,
    					wxTB_3DBUTTONS, wxHORIZONTAL, 8)
{
    SetMargins(2, 2);
    GetDC()->SetBackground(wxGREY_BRUSH);
}

void GameToolbar::AddToolFromResource(int idx, const char *bmpfile, 
				const char *shorthelp, 
				const char *longhelp)
{
    wxButtonBar::AddTool(idx, new wxBitmap((char*)bmpfile, 
		    			wxBITMAP_TYPE_BMP_RESOURCE | 
					wxBITMAP_DISCARD_COLOURMAP));
    if (shorthelp) SetToolShortHelp(idx, (char*)shorthelp);
    if (longhelp) SetToolLongHelp(idx, (char*)longhelp);
}

void GameToolbar::AddToolFromFile(int idx, const char *bmpfile, 
				const char *shorthelp, 
				const char *longhelp)
{
    wxButtonBar::AddTool(idx, new wxBitmap((char*)bmpfile, 
		    			wxBITMAP_TYPE_BMP | 
//					wxBITMAP_DISCARD_COLOURMAP));
					0));
    if (shorthelp) SetToolShortHelp(idx, (char*)shorthelp);
    if (longhelp) SetToolLongHelp(idx, (char*)longhelp);
}

void GameToolbar::Init()
{
    CreateTools();
    Layout();
}

Bool GameToolbar::OnLeftClick(int toolIndex, Bool toggled)
{
    parentframe->HandleToolbarClick(toolIndex);
    return TRUE;
}

GameToolbar::~GameToolbar()
{
}

