/*
 * File:	gcanvas.cc
 * Purpose:	General purpose game canvas class
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
#include "gdialog.h"
#include "gcanvas.h"

#ifdef wx_msw
#include "mmsystem.h"
#endif

#ifndef max
#define max(a, b)	( ((a)>(b)) ? (a) : (b) )
#endif
#ifndef min
#define min(a, b)	( ((a)>(b)) ? (b) : (a) )
#endif

//---------------------------------------------------------------

GameCanvas::GameCanvas(wxFrame *frame, int w, int h, int numresources_in,
			unsigned flags_in)
    : wxCanvas(frame, 0, 0, w, h, wxRETAINED),
	flags(flags_in),
	rows(1), cols(1), 
	keepsquare(1), rightspace(0), bottomspace(0),
	squarewidth(1), squareheight(1),
	boardwidth(1), boardheight(1),
	numresources(numresources_in)
{
    if (numresources<1) numresources = 1;
    resources = new ColourResource*[numresources];
    for (int i = 0; i < numresources; i++)
	resources[i] = 0;
    SetResource(0, "Background", "background", C_WHITE);
    dc = GetDC();
}

GameCanvas::~GameCanvas()
{
    for (int i = 0; i < numresources; i++)
	delete resources[i];
    delete [] resources;
}

//---------------------------------------------------------------
// Colour management. This is done by means of names resources.
// The base class only has a single colour resource named
// "Background". Derived classes must define methods to return
// resource names, .ini IDs, and default values. The GameCanvas
// class can then manage colour settings on behalf of the child.

#ifdef wx_msw
char *ColourNames[] = 		 
{
    "Black",		    "Blue",		    "Blue Violet",
    "Coral",		    "Cornflower Blue",	    "Cyan",
    "Grey",		    "Green",		    "Light Grey",
    "Magenta",		    "Medium Slate Blue",    "Medium Spring Green",
    "Orange Red",	    "Red",		    "Salmon",
    "Slate Blue",	    "Spring Green",	    "White",
    "Yellow"
};
#endif

#ifdef wx_x
char *ColourNames[] = 		 
{
    "Aquamarine",	    "Black",		    "Blue",
    "Blue Violet",	    "Brown",		    "Cadet Blue",
    "Coral",		    "Cornflower Blue",	    "Cyan",
    "Dark Grey",	    "Dark Green",	    "Dark Olive Green",
    "Dark Orchid",	    "Dark Slate Blue",	    "Dark Slate Grey ",
    "Dark Turquoise",	    "Dim Grey",		    "Firebrick",
    "Forest Green",	    "Gold",		    "Goldenrod",
    "Grey",		    "Green",		    "Green Yellow",
    "Indian Red",	    "Khaki",		    "Light Blue",
    "Light Grey",	    "Light Steel Blue",	    "Lime Green",
    "Magenta",		    "Maroon",		    "Medium Aquamarine",
    "Medium Blue",	    "Medium Forest Green",  "Medium Goldenrod",
    "Medium Orchid",	    "Medium Sea Green",	    "Medium Slate Blue",
    "Medium Spring Green,", "Medium Turquoise",	    "Medium Violet Red",
    "Midnight Blue",	    "Navy",		    "Orange",
    "Orange Red",	    "Orchid",		    "Pale Green",
    "Pink",		    "Plum",		    "Purple",
    "Red",		    "Salmon",		    "Sea Green",
    "Sienna",		    "Sky Blue",		    "Slate Blue",
    "Spring Green",	    "Steel Blue",	    "Tan",
    "Thistle",		    "Turquoise",	    "Violet",
    "Violet Red",	    "Wheat",		    "White",
    "Yellow",		    "Yellow Green"
};
#endif

void ColourResource::Load(const char *inifile, const char *section)
{
    (void)wxGetResource(section, id, &val, inifile);
}

void ColourResource::Save(const char *inifile, const char *section)
{
    (void)wxWriteResource(section, id, val, inifile);
}

void ColourResource::Select(wxDC *dc)
{
    char cname[40];
    strcpy(cname, ColourNames[val]);
    strupr(cname);
    wxColour *cl = wxTheColourDatabase->FindColour(cname);
    wxBrush *br = wxTheBrushList->FindOrCreateBrush(cl, wxSOLID);
    dc->SetBrush(br);
    dc->SetTextBackground(cl);
    dc->SetBackgroundMode(wxTRANSPARENT);
}

void GameCanvas::SetResource(int rnum, const char *name, const char *id,
				int val)
{
    delete resources[rnum];
    resources[rnum] = new ColourResource(name, id, val);
}

void GameCanvas::SetResource(const char *inifile, int rnum, const char *name,
				const char *id, int val)
{
    delete resources[rnum];
    resources[rnum] = new ColourResource(name, id, val);
    resources[rnum]->Load(inifile);
}

void GameCanvas::SetColour(int rnum)
{
    resources[rnum]->Select(dc);    
}

//------------------------------------------------------------------------
// Choose colours dialog

void chgcol(wxChoice& chc, wxCommandEvent& event)
{
    ConfigureColoursDlg *d = (ConfigureColoursDlg*)chc.GetClientData();
    d->Test();
}

void ConfigureColoursDlg::Test()
{
    for (int i = 0;  i<nr; i++) resources[i]->SetValue(opt[i]->GetSelection());
    canvas->OnPaint();
    for (int i = 0;  i<nr; i++) resources[i]->SetValue(oldvals[i]);
}

ConfigureColoursDlg::ConfigureColoursDlg(GameCanvas *canvas_in,
					 const char *inifile_in)
    : GameDialog("Configure Colours"),
      canvas(canvas_in), inifile(inifile_in), 
      resources(canvas->resources), nr(canvas->numresources)
{
    int i;
    oldvals = new int[nr];
    opt = new wxChoice*[nr];
    for (i = 0;  i<nr; i++) oldvals[i] = resources[i]->Value();
    // the things we do for item sizing...
    char *x[10];
    for (int i = 0; i < 10; i++) x[i]="XXXXXXXXXXXXXXX";
    for (i = 0;  i<nr; i++)
    {
        opt[i] = new wxChoice(this, (wxFunction)&chgcol, "xxxxxxxxxxxxxxxxxxxx",
				-1,-1,-1,-1, 10, x);
	opt[i]->SetClientData((char*)this);
	NewLine();
    }
    AddButton(SaveID, "Save these Colours", 0);
    Tab();
    AddButton(DoneID, "Done", 1);
    Fit();
    for (i = 0;  i<nr; i++)
    {
	opt[i]->Clear();
     	opt[i]->SetLabel((char*)resources[i]->Name());
	for (int c = 0; c < C_NUMCOLOURS; c++)
	    opt[i]->Append(ColourNames[c]);
	opt[i]->SetSelection(resources[i]->Value());
    }
}

void ConfigureColoursDlg::Save()
{
    for (int i = 0;  i < nr; i++)
    {
	resources[i]->SetValue(opt[i]->GetSelection());
	resources[i]->Save(inifile);
    }
}

void ConfigureColoursDlg::HandleButton(int bnum)
{
    if (bnum == SaveID) Save();
    else GameDialog::HandleButton(bnum);
}

ConfigureColoursDlg::~ConfigureColoursDlg()
{
    delete [] oldvals;
    delete [] opt;
    for (int i = 0;  i < nr; i++)
	resources[i]->Load(inifile);
    canvas->OnPaint();
}

//---------------------------------------------------------------------

void GameCanvas::SelectPen(int cnum)
{
    char cname[40];
    strcpy(cname, ColourNames[cnum]);
    strupr(cname);
    wxColour *cl = wxTheColourDatabase->FindColour(cname);
    wxPen *p = wxThePenList->FindOrCreatePen(cl, 1, wxSOLID);
    dc->SetPen(p);
    dc->SetTextForeground(cl);
}

void GameCanvas::SelectResourcePen(int rnum)
{
    SelectPen(resources[rnum]->Value());
}

void GameCanvas::SelectBrush(int cnum)
{
    char cname[40];
    strcpy(cname, ColourNames[cnum]);
    strupr(cname);
    wxColour *cl = wxTheColourDatabase->FindColour(cname);
    wxBrush *br = wxTheBrushList->FindOrCreateBrush(cl, wxSOLID);
    dc->SetBrush(br);
}

void GameCanvas::SelectResourceBrush(int rnum)
{
    SelectBrush(resources[rnum]->Value());
}

void GameCanvas::DrawBoardSquare(int r, int c, int border, int rnum)
{
    if (rnum>=0) SetColour(rnum);
    if (border) SelectPen(C_BLACK);
    else dc->SetPen(wxTRANSPARENT_PEN);
    dc->DrawRectangle((c+1)*squarewidth, (r+1)*squareheight,
			   (float)squarewidth, (float)squareheight);
}

void GameCanvas::ClearSurroundingArea()
{
    int width, height;
    GetSize(&width, &height);
    float x = (float)((cols+2) * squarewidth);
    float y = (float)((rows+2) * squareheight);
    SetColour(0);
    dc->SetPen(wxTRANSPARENT_PEN);
    dc->DrawRectangle(1., 1., (float)(width-2), (float)(squareheight-2));
    dc->DrawRectangle(1., 1., (float)(squarewidth-2), (float)(height-2));
    dc->DrawRectangle(1., y+1., (float)(width-2), (float)(height-2)-y);
    dc->DrawRectangle(x+1., 1., (float)(width-2)-x, (float)(height-2));
}

void GameCanvas::DrawGrid(int x, int y, int nr, int nc, int w, int h)
{
    int r = 0, c = 0;
    int vertex = ((flags & GR_VERTEX) != 0);
    int noborder = ((flags & GR_NOBORDER) != 0);
    SelectPen(C_BLACK);
    if (vertex)
    {
	if (noborder) // shrink to fit
	{
	    x += w/2;
	    y += h/2;
	    nr--;
	    nc--;
	    noborder = 0; // force `border' to be drawn
	}
	else // expand to fit
	{
	    x -= w/2;
	    y -= h/2;
	    nr++;
	    nc++;
	}
    }
    if (noborder)
    {
	r++;
	c++;
    }
    else dc->DrawRectangle(x, y, (float)(w*nc+1), (float)(h*nr+1));
    for (; r < nr; r++)
       dc->DrawLine((float)x, (float)(y+r*h),
		 	 (float)(x+nc*w), (float)(y+r*h));
    for (; c < nc; c++)
       dc->DrawLine((float)(x+c*w), (float)y,
			 (float)(x+c*w), (float)(y+nr*h));
}

void GameCanvas::DrawGrid()
{
    DrawGrid(squarewidth, squareheight, rows, cols, squarewidth, squareheight);
}

//----------------------------------------------------------------------
// Some simple graphic ops about a center point

void GameCanvas::DrawPlus(int x, int y, int xtent)
{
    dc->DrawLine((float)(x-xtent), (float)y,
			(float)(x+xtent+1), (float)y);
    dc->DrawLine((float)x, (float)(y-xtent),
			(float)x, (float)(y+xtent+1));
}

void GameCanvas::DrawX(int x, int y, int xtent)
{
    dc->DrawLine((float)(x-xtent), (float)(y-xtent),
			(float)(x+xtent+1), (float)(y+xtent+1));
    dc->DrawLine((float)(x-xtent), (float)(y+xtent+1),
			(float)(x+xtent+1), (float)(y-xtent));
}

void GameCanvas::DrawBox(int x, int y, int xtent)
{
    dc->DrawLine((float)(x-xtent), (float)(y-xtent),
    	(float)(x+xtent), (float)(y-xtent));
    dc->DrawLine((float)(x-xtent), (float)(y+xtent),
    	(float)(x+xtent), (float)(y+xtent));
    dc->DrawLine((float)(x-xtent), (float)(y+xtent),
    	(float)(x-xtent), (float)(y-xtent));
    dc->DrawLine((float)(x+xtent), (float)(y+xtent),
    	(float)(x+xtent), (float)(y-xtent));
}

void GameCanvas::DrawCenteredString(int x, int y, int xtent, char *str)
{
    float w, h;
    for (int fs=xtent; fs>0; fs--)
    {
        wxFont *font = wxTheFontList->FindOrCreateFont(fs, wxMODERN, wxNORMAL, wxNORMAL);
	if (font == 0) continue;
	dc->SetFont(font);
	dc->GetTextExtent(str, &w, &h);
	if ((int)w <= xtent && (int)h <= xtent)
	    break;
    }
    dc->DrawText(str, x-w/2, y-h/2);
}

void GameCanvas::DrawCenteredLetter(int x, int y, int xtent, int ch)
{
    char buf[2];
    buf[0] = ch; buf[1] = 0;
    DrawCenteredString(x, y, xtent, buf);
}

void GameCanvas::DrawButton(int frnum, int brnum, int x, int y, int radius)
{
    SelectPen(C_BLACK);
    SelectResourceBrush(brnum);
    DrawArc((float)(x-radius), (float)(y),
	    (float)(x-radius), (float)(y),
	    (float)x, (float)y);
    radius--;
    SelectResourcePen(brnum);
    SelectResourceBrush(brnum);
    DrawArc((float)(x-radius), (float)(y),
	    (float)(x-radius), (float)(y),
	    (float)x, (float)y);
    if (radius/8>2)
	radius -= radius/8;
    else if (radius>2)
	radius -= 2;
    SelectResourcePen(brnum);
    SelectResourceBrush(frnum);
    DrawArc((float)(x), (float)(y-radius),
	    (float)(x-radius), (float)(y),
	    (float)x, (float)y);
    if (radius/8>2)
	radius -= radius/8;
    else if (radius>2)
	radius -= 2;
    SelectResourcePen(brnum);
    SelectResourceBrush(brnum);
    DrawArc((float)(x-radius), (float)(y),
	    (float)(x-radius), (float)(y),
	    (float)x, (float)y);
}

//-------------------------------------------------------------------------

void GameCanvas::DrawArrow(int x, int y, int len, int dx, int dy)
{
    int hl = len/5;
    if (hl < 5) hl = 5;
    else if (hl > 15) hl = 15;
    dc->DrawLine((float)(x), (float)(y), (float)(x+dx*len), (float)(y+dy*len));
    if (dx)
    {
        dc->DrawLine((float)(x), (float)(y), (float)(x+dx*hl), (float)(y+hl));
        dc->DrawLine((float)(x), (float)(y), (float)(x+dx*hl), (float)(y-hl));
    }
    else
    {
        dc->DrawLine((float)(x), (float)(y), (float)(x-hl), (float)(y+dy*hl));
        dc->DrawLine((float)(x), (float)(y), (float)(x+hl), (float)(y+dy*hl));
    }
}

void GameCanvas::DrawLeftArrow(int x, int y, int len)
{
    DrawArrow(x, y, len, 1, 0);
}

void GameCanvas::DrawRightArrow(int x, int y, int len)
{
    DrawArrow(x, y, len, -1, 0);
}

void GameCanvas::DrawDownArrow(int x, int y, int len)
{
    DrawArrow(x, y, len, 0, -1);
}

void GameCanvas::DrawUpArrow(int x, int y, int len)
{
    DrawArrow(x, y, len, 0, 1);
}

//----------------------------------------------------------------------

void GameCanvas::SquareToCenterPixel(int &r, int &c)
{
    int cx = (c+1) * squarewidth + squarewidth/2;
    int cy = (r+1) * squareheight + squareheight/2;
    r = cy; c = cx;
}

void GameCanvas::DrawRoundedRectangle(int x, int y, int w, int h)
{
#ifdef wx_msw
    dc->DrawRoundedRectangle(x, y, w, h, w/5.);
#else
    dc->DrawRoundedRectangle(x, y, w, h, w/7.);
#endif
}

void GameCanvas::OnPaint(void)
{
    Clear();
    DrawBoard();
    //SetFocus();
}

void GameCanvas::OnScroll(wxCommandEvent &event)
{
    wxCanvas::OnScroll(event);
    OnPaint();
}

void GameCanvas::OnEvent(wxMouseEvent& event)
{
    if (event.IsButton()) SetFocus();
    float x, y;
    event.Position(&x, &y);
    int r = (int)y/squareheight-1;
    int c = (int)x/squarewidth-1;
    if (r>=0 && c>=0 && r<rows && c<cols)
    {
        if (event.Moving())
            HandleMouseMoveOnBoard(r, c);
        else if (event.ButtonUp(1))
	    HandleLeftMouseClickOnBoard(r, c);
        else if (event.ButtonUp(3))
	    HandleRightMouseClickOnBoard(r, c);
    }
    else
    {
        if (event.Moving())
            HandleMouseMoveOffBoard(x, y);
        else if (event.ButtonUp(1))
	    HandleLeftMouseClickOffBoard(x, y);
        else if (event.ButtonUp(3))
	    HandleRightMouseClickOffBoard(x, y);
    }
}

void GameCanvas::HandleLeftMouseClickOnBoard(int r, int c)
{
}

void GameCanvas::HandleRightMouseClickOnBoard(int r, int c)
{
}

void GameCanvas::HandleMouseMoveOnBoard(int r, int c)
{
}

void GameCanvas::HandleLeftMouseClickOffBoard(int x, int y)
{
}

void GameCanvas::HandleRightMouseClickOffBoard(int x, int y)
{
}

void GameCanvas::HandleMouseMoveOffBoard(int x, int y)
{
}

void GameCanvas::HandleKey(int ch)
{
}

void GameCanvas::OnChar(wxKeyEvent& event)
{
    HandleKey(event.KeyCode());
}

void GameCanvas::SetBoardDimensions(int rows_in, int cols_in,
				    int keepsquare_in, int rightspace_in,
				    int bottomspace_in)
{
    rows = rows_in;
    cols = cols_in;
    keepsquare = keepsquare_in;
    rightspace = rightspace_in;
    bottomspace = bottomspace_in;
    // get the panel size
    int width, height;
    GetSize(&width, &height);
    squarewidth = (width / (cols+2+rightspace));
    squareheight = (height / (rows+2+bottomspace));
    if (keepsquare)
	squarewidth = squareheight = min(squarewidth, squareheight);
    boardwidth = cols * squarewidth;
    boardheight = rows * squareheight;
    // ensure some sanity
    if (squarewidth<1) squarewidth = 1;
    if (squareheight<1) squareheight = 1;
    if (boardwidth<1) boardwidth = 1;
    if (boardheight<1) boardheight = 1;
}

void GameCanvas::AdjustBoardDimensions()
{
    SetBoardDimensions(rows, cols, keepsquare, rightspace, bottomspace);
}

void GameCanvas::DrawSquare(int r, int c)
{
}

void GameCanvas::DrawBoard()
{
    for (int r = 0; r < rows; r++)
       for (int c = 0; c < cols; c++)
	  DrawSquare(r, c);
}

void GameCanvas::PrintBoard() // this may have to be expanded
{
    dc = new wxPrinterDC(0,0,0);
    if (dc->Ok()) DrawBoard();
    delete dc;
    dc = GetDC();
}

void GameCanvas::Clear()
{
    // wxCanvas::Clear doesn't do the job of setting the
    // background colour properly, so we do it this way...
    SetColour(0);
    SelectPen(C_BLACK);
    int width, height;
    GetVirtualSize(&width, &height);
    dc->DrawRectangle(0., 0., (float)width, (float)(height));
}

void GameCanvas::Resize(int width, int height)
{
    SetSize(0, 0, width, height);
    AdjustBoardDimensions();
    Clear();
    DrawBoard();
}

void GameCanvas::SetGame(Game *game_in)
{
    game = game_in;
    if (game)
    {
        rows = game->Height();
	cols = game->Width();
	AdjustBoardDimensions();
    }
}



