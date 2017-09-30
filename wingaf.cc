/*
 * File:	wingaf.cc
 * Purpose:	Generic multiplayer board game engine for Windoze
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
#include "gafgame.h"
#include "gpanel.h"
#include "gdialog.h"
#include "gcanvas.h"
#include "gmenu.h"
#include "gframe.h"
#include "wingaf.h"

#ifdef wx_msw
#include "mmsystem.h"
#endif

#ifndef max
#define max(a, b)	( ((a)>(b)) ? (a) : (b) )
#endif
#ifndef min
#define min(a, b)	( ((a)>(b)) ? (b) : (a) )
#endif

WinGAFApp     myApp; // initialise the application

#define APPSHORTNAME 	"wingaf"
#define APPNAME		"WinGAF for Windows" 
#define DESCRIPTION     "WinGAF for Windows v1.0\nby Graham Wheeler gram@oms.co.za\n(c) 1996"

#define CANVASBACKGROUNDCOLOUR	0
#define BOARDCOLOUR		1
#define TMPBUTTONBGCOLOUR	2
#define TMPBUTTONFGCOLOUR	3
#define P1BUTTONBGCOLOUR	4
#define P1BUTTONFGCOLOUR	5
#define P2BUTTONBGCOLOUR	6
#define P2BUTTONFGCOLOUR	7

#ifdef DRAWTREE

extern TreeNode *tree;
int zoom = 5, xspace, yspace;
extern FILE *tron;

#endif

//---------------------------------------------------------------

class ConfigureBoardSizeDlg : public GameDialog
{
    wxSlider *rs, *cs;
    int gnum;
    const char *inifile;
    enum { CancelID, OKID };
  public:
    ConfigureBoardSizeDlg(int gnum, const char *inifile_in);
    void Save();
    virtual void HandleButton(int bnum);
    ~ConfigureBoardSizeDlg();
};

ConfigureBoardSizeDlg::ConfigureBoardSizeDlg(int gnum_in, const char *inifile_in)
    : GameDialog("Configure Board Size"), inifile(inifile_in), gnum(gnum_in)
{
    int r, c;
    char sec[20];
    sprintf(sec, "Game%d", gnum);
    (void)wxGetResource(sec, "Rows", &r, inifile);
    (void)wxGetResource(sec, "Cols", &c, inifile);
    if (r < 1) r = 1;
    else if (r > 25) r = 25;
    if (c < 1) c = 1;
    else if (c > 25) c = 25;
    rs = MakeSlider("Rows", r, 1, 25);
    NewLine();
    cs = MakeSlider("Cols", c, 1, 25);
    NewLine();
    AddButton(OKID, "OK", 0);
    Tab();
    AddButton(CancelID, "Cancel", 1);
    Fit();
}

void ConfigureBoardSizeDlg::Save()
{
    int r = rs->GetValue();
    int c = cs->GetValue();
    char sec[20];
    sprintf(sec, "Game%d", gnum);
    (void)wxWriteResource(sec, "Rows", r, inifile);
    (void)wxWriteResource(sec, "Cols", c, inifile);
}

void ConfigureBoardSizeDlg::HandleButton(int bnum)
{
    if (bnum == OKID) Save();
    GameDialog::HandleButton(0);
}

ConfigureBoardSizeDlg::~ConfigureBoardSizeDlg()
{
}

//---------------------------------------------------------------

WinGAFMenuBar::WinGAFMenuBar(GameCanvas *canvas_in)
    : GameMenu(APPSHORTNAME, APPNAME, DESCRIPTION, ini,  canvas_in, 0)
{
}

wxMenu *WinGAFMenuBar::MakeOptionsMenu()
{
    wxMenu *rtn = GameMenu::MakeOptionsMenu();
    rtn->Append(GM_BSIZE, "&Board Size", "Change the size of the board");
    return rtn;
}

int WinGAFMenuBar::Handle(int id)
{
    if (id == GM_BSIZE)
    {
	int g;
	wxGetResource("Game", "LastType", &g, ini);
	RunDialog(new ConfigureBoardSizeDlg(g, ini));
	return 0;
    }
    return GameMenu::Handle(id);
}


WinGAFMenuBar::~WinGAFMenuBar()
{
}

//---------------------------------------------------------------

WinGAFCanvas::WinGAFCanvas(GameFrame *frame, int w, int h, unsigned flags_in)
    : GameCanvas(frame, w, h, 8, flags_in)
{
    SetResource(ini, CANVASBACKGROUNDCOLOUR, "Background", "CanvasBackground", C_WHITE	);
    SetResource(ini, BOARDCOLOUR, "Board", "Board", C_CYAN	);
    SetResource(ini, TMPBUTTONBGCOLOUR, "Current Move Button Background", "TmpButtonBg", C_RED );
    SetResource(ini, TMPBUTTONFGCOLOUR, "Current Move Button Foreground", "TmpButtonFg", C_BLACK );
    SetResource(ini, P1BUTTONBGCOLOUR, "Player 1 Button Background", "P1ButtonBg", C_WHITE );
    SetResource(ini, P1BUTTONFGCOLOUR, "Player 1 Button Foreground", "P1ButtonFg", C_BLACK );
    SetResource(ini, P2BUTTONBGCOLOUR, "Player 2 Button Background", "P2ButtonBg", C_WHITE );
    SetResource(ini, P2BUTTONFGCOLOUR, "Player 2 Button Foreground", "P2ButtonFg", C_BLACK );
    rows = cols = 3;
}

void WinGAFCanvas::DrawSquare(int r, int c)
{
    if (game==0) return;
    Board *b = ((MultiPlayerGame*)game)->GetBoard();
    if (b==0) return;
    int w = squarewidth;
    int h = squareheight;
    int xoff = 3*squarewidth/2;
    int yoff = 3*squareheight/2;
    if (!b->IsEmpty(r, c) || game->IsSelected(r,c))
    {
	int p = b->Owner(r, c);
	int fg, bg;
	if (game->IsSelected(r, c))
	{
	    fg = TMPBUTTONFGCOLOUR;
	    bg = TMPBUTTONBGCOLOUR;
	}
	else
	{
	    fg = P1BUTTONFGCOLOUR+2*p;
	    bg = P1BUTTONBGCOLOUR+2*p;
	}
	DrawButton(fg, bg, xoff+c*squarewidth, yoff+r*squareheight, squarewidth/3);
    }
}

void WinGAFCanvas::DrawBoard()
{
    SelectResourceBrush(BOARDCOLOUR);
    DrawGrid();
    GameCanvas::DrawBoard();
}

void WinGAFCanvas::HandleLeftMouseClickOffBoard(int x, int y)
{
#ifdef DRAWTREE
    if (tree)
    {
	delete tree;
	tree = 0;
	SetScrollPos(wxVERTICAL, 0);
	SetScrollPos(wxHORIZONTAL, 0);
	OnPaint();
	return;
    }
#endif
}

void WinGAFCanvas::HandleLeftMouseClickOnBoard(int r, int c)
{
#ifdef DRAWTREE
    if (tree)
    {
	delete tree;
	tree = 0;
	SetScrollPos(wxVERTICAL, 0);
	SetScrollPos(wxHORIZONTAL, 0);
	OnPaint();
	return;
    }
#endif
    if (game == 0) return;
    game->HandleBoardSelect(r, c);
    OnPaint();
}

void WinGAFCanvas::OnPaint()
{
#ifdef DRAWTREE
    if (tree)
	DrawTree();
    else
#endif
    if (game)
    {
	if (flags & GR_GAMMON)
	{
	}
	else GameCanvas::OnPaint();
    }
    ((WinGAFFrame*)window_parent)->UpdateStatusBar();
}

int WinGAFCanvas::GetMove(char *buf)
{
    return ((MultiPlayerGame*)game)->GetBoardSelect(buf);
}

#ifdef DRAWTREE

void WinGAFCanvas::DrawLink(int ypos, long px, long cx)
{
    int cy = ypos * yspace - 15;
    int py = cy-40;
    if (tron)
	fprintf(tron, "In DrawLink px %ld, cx %d, xspace %ld\n",
		px, cx, xspace);
    px = (px * xspace)/6 + 20*zoom;
    cx = (cx * xspace)/6 + 20*zoom;
    if (tron)
	fprintf(tron, "DrawLink %ld,%d to %ld,%d\n", px, py, cx, cy);
    GetDC()->DrawLine((float)px, (float)py, (float)cx, (float)cy);
}

void WinGAFCanvas::DrawTreeNode(long x, int y, const Board *b, int val,
					int ismax)
{
    int r, c;
    int w = (b->Cols() * zoom);
    x = (x * xspace)/6 + 20*zoom;
    y = (y * yspace);
    int xx = GetScrollPos(wxHORIZONTAL)*20;
    int yy = GetScrollPos(wxVERTICAL)*20;
    int ww, hh;
    GetClientSize(&ww, &hh);
    if (x<xx || x > (xx+ww) || y < yy || y > (yy+hh))
	return;
    // draw micro board
    for (r = 0; r < b->Rows(); r++)
    {
	for (c = 0; c < b->Cols(); c++)
	{
	    int p = b->Owner(r,c);
	    int xx = x+c*zoom;
	    int yy = y+r*zoom;
	    if (b->IsEmpty(r, c))
		SelectPen(C_GREEN);
	    else
		SelectPen(b->Owner(r,c) ? C_RED : C_BLUE);
	    for (int dx=0; dx<zoom; dx++)
		for (int dy=0; dy<zoom; dy++)
		    GetDC()->DrawPoint((float)(xx+dx),(float)(yy+dy));
	    if (!b->IsEmpty(r,c) && zoom>2)
	    {
		SelectPen(C_GREEN);
		GetDC()->DrawPoint((float)(xx),(float)(yy));
		GetDC()->DrawPoint((float)(xx), (float)(yy));
		GetDC()->DrawPoint((float)(xx+zoom-1), (float)(yy));
		GetDC()->DrawPoint((float)(xx), (float)(yy+zoom-1));
		GetDC()->DrawPoint((float)(xx+zoom-1), (float)(yy+zoom-1));
	    }
	}
    }
    // draw board outline
    SelectPen(C_BLACK);
    r = b->Rows();
    c = b->Cols();
    if (tron)
	fprintf(tron, "DrawTreeNode at %d,%d\n", x, y);
    GetDC()->DrawLine((float)(x-1),(float)(y-1),(float)(x+c*zoom+1),(float)(y-1)); // across top
    GetDC()->DrawLine((float)(x-1),(float)(y+r*zoom+1),(float)(x+c*zoom+1),(float)(y+r*zoom+1)); // down right
    GetDC()->DrawLine((float)(x-1),(float)(y-1),(float)(x-1),(float)(y+r*zoom+1)); // down left
    GetDC()->DrawLine((float)(x+c*zoom+1),(float)(y-1),(float)(x+c*zoom+1),(float)(y+r*zoom+1)); // across bottom
    if (ismax) SelectPen(C_RED);
    // draw weight
    if (zoom>2)
    {
	char wbuf[10];
	switch(val)
	{
	case WIN_V:
	    strcpy(wbuf, "WIN");
	    break;
	case LOSE_V:
	    strcpy(wbuf, "LOSE");
	    break;
	default:
	    sprintf(wbuf,"%d", val);
	}
	DrawCenteredString(x+w/2, y-10, w, wbuf);
    }
    SelectPen(C_BLACK); // ready for links
}

void WinGAFCanvas::DrawTree()
{
    MultiPlayerGame *g = (MultiPlayerGame*)game;
    if (g==0) return;
    if (tree == 0)
    {
    	class Move *mv;
	tree = new TreeNode(0,0,new Board(g->GetBoard()));
	(void)g->FindMove(g->Level(), g->MoveNum(), g->GetPlayerNum(),
			 g->GetPlayerNum(), g->GetBoard(),
			 mv = g->NewMove(), UNKNOWN_V, tree);
	delete mv;
	(void)tree->CalcLayout(1, 1);
	xspace = g->GetBoard()->Cols()*zoom+10;
	yspace = g->GetBoard()->Rows()*zoom+60;
    }
    Clear();
    tree->Paint(this);
}

void TreeNode::Paint(WinGAFCanvas *c)
{
    c->DrawTreeNode(x, y, b, weight,ismax);
    if (parent)
	c->DrawLink(y, parent->x, x);
    if (firstchild)
	firstchild->Paint(c);
    if (nextsibling)
	nextsibling->Paint(c);
}

void WinGAFCanvas::HandleKey(int ch)
{
    if (game== 0 || tree == 0) return;
    switch(ch)
    {
    case '+':
	zoom++; 
	break;
    case '-':
	if (zoom) zoom--; 
	break;
    case 27:
	delete tree;
	tree = 0;
	OnPaint();
	return;
    }
    MultiPlayerGame *g = (MultiPlayerGame*)game;
    xspace = g->GetBoard()->Cols()*zoom+10;
    yspace = g->GetBoard()->Rows()*zoom+60;
    DrawTree();
}

#endif

WinGAFCanvas::~WinGAFCanvas()
{
}

//------------------------------------------------------------------------
// The right-hand panel 

WinGAFPanel::WinGAFPanel(GameFrame *f, WinGAFCanvas *canvas_in,
			 int left, int width, int height, int nlbls)
    : GamePanel(f, left, width, height, 
	"Auto@Next@Run@Stop@Undo@Redo@Tree@Done", nlbls),
      game(0), canvas(canvas_in)
{
    Show(TRUE);
}

void WinGAFPanel::Pass()
{
    // for now we use this for debugging
    extern FILE *tron;
    tron = fopen("tron", "w");
#ifdef DRAWTREE
    canvas->DrawTree();
#else
    Auto();
#endif
    fclose(tron);
}

int WinGAFPanel::Done()
{
    char input[80];
    if (canvas->GetMove(input) == 0)
    {
	char *msg = game->PlayMove(input);
	if (msg)
	{
 	    ((wxFrame*)window_parent)->SetStatusText(msg);
	    return -1;
	}
	canvas->OnPaint();
    }
    return 0;
}

void WinGAFPanel::Auto()
{
    ((wxFrame*)window_parent)->SetStatusText("Thinking...");
    wxBeginBusyCursor();
    game->AutoPlay();
    wxEndBusyCursor();
    canvas->OnPaint();
}

void WinGAFPanel::Next()
{
    game->PlayNext();
}

void WinGAFPanel::Run()
{
    game->MakePlayerAuto();
}

void WinGAFPanel::Undo()
{
    game->Undo();
}

void WinGAFPanel::Redo()
{
    if (!game->GameOver()) game->Redo();
}

void WinGAFPanel::HandleButton(int bnum)
{
    if (game == 0) return;
    if (bnum == 3) game->StopAuto();
    else if (bnum == 4) Undo();
    else if (!game->AutoPlaying() && !game->GameOver())
    {
        switch (bnum)
        {
        case 0: Auto(); break;
        case 1: Next(); break;
        case 2: Run();  break;
        case 5: Redo(); break;
        case 6: Pass(); break;
        case 7: (void)Done(); break;
	}
    }
}

WinGAFPanel::~WinGAFPanel()
{
}

//------------------------------------------------------------------
// Tic tac toe

class TicTacToeGame : public PlaceGame
{
    GameCanvas *canvas;
    int CheckCounts(int player, unsigned &flags, int n, int *counts);
public:
    TicTacToeGame(GameCanvas *canvas_in, 
		  Board *board_in, 
		  int numplayers_in = 2)
	: PlaceGame(board_in, numplayers_in), canvas(canvas_in)
    { }
    TicTacToeGame(GameCanvas *canvas_in, 
		  FILE *fp,
		  int numplayers_in = 2)
	: PlaceGame(fp, numplayers_in), canvas(canvas_in)
    { }
    virtual unsigned DrawFlags() const
    {
	return GR_NOBORDER;
    }
    virtual int Weight(const Board *b, int player, int nextplayer, unsigned &flags);
    virtual void Paint()
    {
	canvas->OnPaint();
    }
    virtual ~TicTacToeGame()
    { }
};

int TicTacToeGame::CheckCounts(int player, unsigned &flags, int n, int *counts)
{
    for (int p = 0; p < MAX_PLAYERS; p++)
	if (counts[p] == n) // this player wins
	{
	    if (p == player) flags = WIN;
	    else flags = LOSE;
	    SETWINNER(flags, p);
	    return 0;
	}
    return -1;
}

#define EMPTY 	for (int p = 0; p < MAX_PLAYERS; p++) \
			counts[p] = 0; // should memset

int TicTacToeGame::Weight(const Board *b, int player, int nextp, unsigned &flags)
{
    int nr = b->Rows();
    int nc = b->Cols();
    int n = min(nr, nc);
    int counts[MAX_PLAYERS];
    flags = 0;
    // check for win across
    for (int r = 0; r < nr; r++)
    {
	EMPTY
	if (b->CountOwnersInRow(r, counts)) continue;
	if (CheckCounts(player, flags, n, counts) == 0)
	    return 0;
    }
    // check for win down
    for (int c = 0; c < nc; c++)
    {
	EMPTY
	if (b->CountOwnersInCol(c, counts)) continue;
	if (CheckCounts(player, flags, n, counts) == 0)
	    return 0;
    }
    // check for diagonal win
    EMPTY
    if (b->CountOwnersInDescendingDiagonal(0, 0, n, counts) == 0)
    {
	if (CheckCounts(player, flags, n, counts) == 0)
	    return 0;
    }
    EMPTY
    if (b->CountOwnersInAscendingDiagonal(nr-1, 0, min(nr,nc), counts) == 0)
    {
	if (CheckCounts(player, flags, n, counts) == 0)
	    return 0;
    }
    if (b->IsFull())
	flags = DRAW;
    return 0;
}

//----------------------------------------------------------------------

class HexapawnGame : public PawnGame
{
    GameCanvas *canvas;
public:
    HexapawnGame(GameCanvas *canvas_in, Board *board_in, int numplayers_in = 2)
	: PawnGame(board_in, numplayers_in), canvas(canvas_in)
    { 
	HexapawnGame::Init();
    }
    HexapawnGame(GameCanvas *canvas_in, FILE *fp, int numplayers_in = 2)
	: PawnGame(fp, numplayers_in), canvas(canvas_in)
    { 
	HexapawnGame::Init();
    }
    virtual void Paint()
    {
	canvas->OnPaint();
    }
    virtual unsigned DrawFlags() const
    {
	return 0;
    }
    virtual int Weight(const Board *b, int player, int nextplayer, unsigned &flags);
    virtual void Init();
    virtual ~HexapawnGame()
    {} 
};

void HexapawnGame::Init()
{
    PawnGame::Init();
    for (int c = 0, r = board->Rows(); c< board->Cols(); c++)
    {
	board->SetOwner(0, c, 1);
	board->SetOwner(r-1, c, 0);
    }
    lastmoverwins = 1;
}

// Trivial evaluator

int HexapawnGame::Weight(const Board *b, int player, int nextp, unsigned &flags)
{
    int counts[MAX_PLAYERS];
    int r, c;
    flags = 0;
    EMPTY
    b->CountOwners(counts);
    if (counts[player] == 0)
    {
	// all pieces taken?
	flags = LOSE;
	SETWINNER(flags, 1-player);
    }
    else if (counts[1-player] == 0)
    {
	// all opponent's pieces taken?
	flags = WIN;
	SETWINNER(flags, player);
    }
    else
    {
	// does player have pawn in last row?
	int w = -1;
	EMPTY
	b->CountOwnersInRow(0, counts);
	if (counts[0] > 0) // player 0 wins
	    w = 0;
	EMPTY
	b->CountOwnersInRow(b->Rows()-1, counts);
	if (counts[1] > 0) // player 1 wins
	    w = 1;
	if (w >= 0)
	{
	    flags = (player == w) ? WIN : LOSE;
	    SETWINNER(flags, w);
	    return 0;
	}
	// can player move?
	if (LocatePawn(b, nextp, CANMOVE, r, c) < 0) // no move 8-(
	{
	    flags = (player == nextp) ? LOSE : WIN;
	    SETWINNER(flags, 1-nextp);
	}
    }
    return 0;
}

//------------------------------------------------------------------
// Othello

class OthelloGame : public PlaceGame
{
    GameCanvas *canvas;
    int CountCaptures(const Board *b, int player, int r, int c,
			int dr, int dc) const;
    int Capture(Board *b, int player, int r, int c,
		int dr, int dc);
    int Capture(Board *b, int player, int r, int c);
public:
    OthelloGame(GameCanvas *canvas_in, 
		  Board *board_in, 
		  int numplayers_in = 2)
	: PlaceGame(board_in, numplayers_in), canvas(canvas_in)
    {
	OthelloGame::Init();
    }
    OthelloGame(GameCanvas *canvas_in, 
		  FILE *fp,
		  int numplayers_in = 2)
	: PlaceGame(fp, numplayers_in), canvas(canvas_in)
    {
	OthelloGame::Init();
    }
    virtual unsigned DrawFlags() const
    {
	return 0;
    }
    virtual int Validate(const Board *b, int player, Move *m) const;
    virtual void ApplyMove(Board *b, Move *m, int mvnum, int player);
    virtual int Weight(const Board *b, int player, int nextplayer, unsigned &flags);
    virtual void Init();
    virtual void Paint()
    {
	canvas->OnPaint();
    }
    virtual ~OthelloGame()
    { }
};

void OthelloGame::Init()
{
    PlaceGame::Init();
    int r = board->Rows();
    int c = board->Cols();
    board->SetOwner(r/2-1, c/2-1, 0);
    board->SetOwner(r/2, c/2, 0);
    board->SetOwner(r/2-1, c/2, 1);
    board->SetOwner(r/2, c/2-1, 1);
    lastmoverwins = 1;
}

int OthelloGame::CountCaptures(const Board *b, int player, int r, int c,
				int dr, int dc) const
{
    int cnt = 0;
    for (;;)
    {
        r += dr; c += dc;
        if (!b->OnBoard(r, c) || b->IsEmpty(r, c))
	    return 0;
	if (b->Owner(r, c) == player)
	    return cnt;
	cnt++;
    }
}

int OthelloGame::Capture(Board *b, int player, int r, int c,
				int dr, int dc)
{
    // assumes this is legit
    int cnt = 0;
    for (;;)
    {
        r += dr; c += dc;
	if (b->Owner(r, c) == player)
	    return cnt;
	cnt++;
	b->SetOwner(r, c, player);
    }
}

int OthelloGame::Capture(Board *b, int player, int r, int c)
{
    int cnt = 0;
    if (CountCaptures(b, player, r, c, -1, -1))
	cnt += Capture(b, player, r, c, -1, -1);
    if (CountCaptures(b, player, r, c, -1, 0))
	cnt += Capture(b, player, r, c, -1, 0);
    if (CountCaptures(b, player, r, c, -1, 1))
	cnt += Capture(b, player, r, c, -1, 1);
    if (CountCaptures(b, player, r, c, 0, -1))
	cnt += Capture(b, player, r, c, 0, -1);
    if (CountCaptures(b, player, r, c, 0, 1))
	cnt += Capture(b, player, r, c, 0, 1);
    if (CountCaptures(b, player, r, c, 1, -1))
	cnt += Capture(b, player, r, c, 1, -1);
    if (CountCaptures(b, player, r, c, 1, 0))
	cnt += Capture(b, player, r, c, 1, 0);
    if (CountCaptures(b, player, r, c, 1, 1))
	cnt += Capture(b, player, r, c, 1, 1);
    return cnt;
}

int OthelloGame::Validate(const Board *b, int player, Move *m) const
{
    int r = ((PlaceMove*)m)->Row();
    int c = ((PlaceMove*)m)->Col();
    if (!b->OnBoard(r, c) || !b->IsEmpty(r, c))
	return 0;
    if (CountCaptures(b, player, r, c, -1, -1) ||
        CountCaptures(b, player, r, c, -1,  0) ||
        CountCaptures(b, player, r, c, -1, +1) ||
        CountCaptures(b, player, r, c,  0, -1) ||
        CountCaptures(b, player, r, c,  0, +1) ||
        CountCaptures(b, player, r, c, +1, -1) ||
        CountCaptures(b, player, r, c, +1,  0) ||
        CountCaptures(b, player, r, c, +1, +1))
	    return 1;
    return 0;
}

void OthelloGame::ApplyMove(Board *b, Move *m, int mvnum, int player)
{
    (void)PlaceGame::ApplyMove(b, m, mvnum, player);
    int r = ((PlaceMove*)m)->Row();
    int c = ((PlaceMove*)m)->Col();
    (void)Capture(b, player, r, c);
}

int OthelloGame::Weight(const Board *b, int player, int nextp, unsigned &flags)
{
    int counts[MAX_PLAYERS];
    EMPTY
    int ec = b->CountOwners(counts);
    int d = counts[0] - counts[1];
    if (ec == 0 || counts[0] == 0 || counts[1] == 0)
    {
	if (d == 0)
	    flags = DRAW;
	else 
	{
	    int winner = (d>0) ? 0 : 1;
	    flags = (player == winner) ? WIN : LOSE;
	    SETWINNER(flags, winner);
	}
    }
    else flags = 0;
    return (player == 0) ? d : -d;
}

//----------------------------------------------------------------------

class GetGameDlg : public GameDialog
{
    wxChoice *nmc;
    wxRadioBox *plb;
    int &gn, &lev;//, &np;
public:
    GetGameDlg(int &gn_in, int &lev_in);
    virtual void HandleButton(int bnum);
    virtual ~GetGameDlg();
};

GetGameDlg::GetGameDlg(int &gn_in, int &lev_in)
    : GameDialog("Choose Game"), gn(gn_in), lev(lev_in)
{
    nmc = new wxChoice(this, 0, "Game:");
    nmc->SetClientData((char*)&gn);
    for (int i = 1; ; i++)
    {
	char buf[16], *rtn = new char[80];
	sprintf(buf, "Game%d", i);
	strcpy(rtn, "Untitled");
	if (wxGetResource(buf, "Name", &rtn, ini) == FALSE)
	{
	    delete [] rtn;
	    break;
	}
	nmc->Append(rtn);
	delete [] rtn;
    }
    gn = ((gn > 0) ? gn : 1);
    nmc->SetSelection(gn-1);
    NewLine();
    char *lvs[9];
    lvs[0] = "1"; lvs[1] = "2"; lvs[2] = "3"; lvs[3] = "4";
    lvs[4] = "5"; lvs[5] = "6"; lvs[6] = "7"; lvs[7] = "8";
    lvs[8] = "9";
    plb = new wxRadioBox(this, 0, "Level", -1, -1, -1, -1, 9, lvs, 9);
    plb->SetClientData((char*)&lev);
    lev = (lev>0 ? lev : 1);
    plb->SetSelection(lev-1);
    NewLine();
    AddButton(0, "OK", 1);
    Fit();
}

void GetGameDlg::HandleButton(int bnum)
{
    gn = nmc->GetSelection()+1;
    lev = plb->GetSelection()+1;
    GameDialog::HandleButton(0);
}

GetGameDlg::~GetGameDlg()
{
}

//------------------------------------------------------------------
// Menu Commands

MultiPlayerGame *WinGAFFrame::AllocateGame()
{
    int l = 4, g = 1;
    wxGetResource("Game", "LastType", &g, ini);
    RunDialog(new GetGameDlg(g, l));
    wxWriteResource("Game", "LastType", g, ini);
    int r = 8, c = 8;
    char sec[20];
    sprintf(sec, "Game%d", g);
    wxGetResource(sec, "Rows", &r, ini);
    wxGetResource(sec, "Cols", &c, ini);
    // ensure sanity
    if (r < 1) r = 1;
    else if (r > 100) r = 100;
    if (c < 1) c = 1;
    else if (c > 100) c = 100;
    MultiPlayerGame *gm = 0;
    switch(g)
    {
    case 1:    
	gm = new TicTacToeGame(canvas, new Board(r, c));
	break;
    case 2:    
	gm = new HexapawnGame(canvas, new Board(r, c));
	break;
    case 3:    
	gm = new OthelloGame(canvas, new Board(r, c));
	break;
    }
    if (gm) gm->SetLevel(l);
    return gm;
}

MultiPlayerGame *WinGAFFrame::AllocateGame(int g, FILE *fp)
{
    wxWriteResource("Game", "LastType", g, ini);
    switch(g)
    {
    case 1:    
	return new TicTacToeGame(canvas, fp);
    case 2:    
	return new HexapawnGame(canvas, fp);
    case 3:    
	return new OthelloGame(canvas, fp);
    }
    return 0;
}

void WinGAFFrame::SetGame(MultiPlayerGame *g)
{
    ((WinGAFPanel*)panel)->SetGame(g);
    canvas->SetGame(g);
    canvas->SetFlags(g ? g->DrawFlags() : 0); // should be in canvas->SetGame
    game = g;
}

int WinGAFFrame::AllocateGameForReading(FILE *fp)	
{
    int g = 1, l = 4;
    fscanf(fp, "%d%d", &g, &l);
    SetGame(AllocateGame(g, fp));
    if (game) ((MultiPlayerGame*)game)->SetLevel(l);
    return game ? 0 : -1;
}

void WinGAFFrame::AllocateNewGame()	
{
    SetGame(AllocateGame());
}

//-----------------------------------------------------------------------

void WinGAFFrame::UpdateStatusBar()
{
    char buf[80];
    unsigned f;
    if (game==0)
	strcpy(buf, "No game.");
    else if (game->GameOver(&f))
    {
	if ((f & 3) == DRAW)
	    strcpy(buf, "Game drawn.");
	else 
	    sprintf(buf, "Game over- player %d wins.", WINNER(f)+1);
    }
    else
        sprintf(buf, "Player %d make your move!", ((MultiPlayerGame*)game)->GetPlayerNum()+1);
    SetStatusText(buf);
}

GamePanel *WinGAFFrame::MakePanel(int x, int w, int h)
{
    return new WinGAFPanel(this, (WinGAFCanvas*)canvas, x, w, h);
}

GameCanvas *WinGAFFrame::MakeCanvas(int w, int h)
{
    GameCanvas *rtn = new WinGAFCanvas(this, w, h, 
			(game ? ((MultiPlayerGame*)game)->DrawFlags() : 0));
#ifdef DRAWTREE
    rtn->SetScrollbars(20,20,200,50,10,5);
    rtn->EnableScrolling(TRUE, TRUE);
#endif
    return rtn;
}

GameMenu *WinGAFFrame::MakeMenuBar()
{
    return new WinGAFMenuBar(canvas);
}

void WinGAFFrame::WriteGame(FILE *fp)
{
    int g;
    wxGetResource("Game", "LastType", &g, ini);
    fprintf(fp, "%d %d\n", g, ((MultiPlayerGame*)game)->Level());
    game->Save(fp);
}

WinGAFFrame::WinGAFFrame()
    : GameFrame("WinGAF for Windows", 650, 600, "wingaf", "*.skl")
{
    wxWriteResource("Game1", "Name", "TicTacToe", ini);
    wxWriteResource("Game2", "Name", "Hexapawn", ini);
    wxWriteResource("Game3", "Name", "Othello", ini);
}

void WinGAFFrame::AutoPlay()
{
    if (game)
	if (game->GameOver())
	    ((MultiPlayerGame*)game)->StopAuto();
	else if (((MultiPlayerGame*)game)->IsAutoPlayer())
	    ((WinGAFPanel*)panel)->Auto();
}

WinGAFFrame::~WinGAFFrame()
{
    extern FILE *tron;
    if (tron) fclose(tron);
    if (tree) delete tree;
}

//------------------------------------------------------------------------
// The `main program' equivalent, creating the windows and returning the
// main xwframe

void MyBackgroundProcess(void *a)
{
    ((WinGAFApp*)a)->BackgroundProcess();
}

wxFrame *WinGAFApp::OnInit(void)
{
    frame = new WinGAFFrame();
    frame->Init();
    work_proc = ::BackgroundProcess;
    AddCycler(MyBackgroundProcess, this);
    return frame;
}

