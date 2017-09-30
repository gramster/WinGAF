/*
 * File:	wingaf.h
 * Purpose:	Generic 2-player board game engine for Windoze
 * Author:	Graham Wheeler
 * Created:	1996
 * Updated:	
 * Copyright:	(c) 1996, Graham Wheeler
 */

#ifndef WINGAF_H
#define WINGAF_H

#ifdef __GNUG__
#pragma interface
#endif

// Initialisation file

#ifdef wx_msw
#define ini	"wingaf.ini"
#endif
#ifdef wx_x
#define ini	".wingaf"
#endif

class WinGAFCanvas;
class WinGAFFrame;

// menu extensions

#define GM_BSIZE    31031

//------------------------------------------------------------------------

class WinGAFApp: public wxApp
{
//    char *MakeDictionary();
    WinGAFFrame *frame;
  public:
    WinGAFApp() {}
    void BackgroundProcess();
    wxFrame *OnInit();
};

class WinGAFMenuBar : public GameMenu
{
  protected:
    //virtual void MakeMiddleMenus();
    //virtual void EnableGameSpecific();
    //virtual void DisableGameSpecific();
    virtual wxMenu *MakeOptionsMenu();
  public:
    WinGAFMenuBar(GameCanvas *canvas_in);
    virtual int Handle(int id);
    virtual ~WinGAFMenuBar();
};

class WinGAFCanvas: public GameCanvas
{
  protected:
    virtual void HandleLeftMouseClickOnBoard(int r, int c);
    //virtual void HandleRightMouseClickOnBoard(int r, int c);
    virtual void HandleLeftMouseClickOffBoard(int x, int y);
#ifdef DRAWTREE
    virtual void HandleKey(int ch);
#endif
  public:
    WinGAFCanvas(GameFrame *frame, int w, int h, unsigned flags_in = 0);
    int GetMove(char *buf);
#ifdef DRAWTREE
    void DrawLink(int ypos, long px, long cx);
    void DrawTreeNode(long x, int y, const Board *b, int val, int ismax);
    void DrawTree();
#endif
    //virtual void SetGame(Game *game_in);
    virtual void DrawSquare(int r, int c);
    virtual void DrawBoard();
    virtual void OnPaint();
    virtual ~WinGAFCanvas() ;
};

class WinGAFPanel : public GamePanel
{
    MultiPlayerGame *game;
    WinGAFCanvas *canvas;

  public:
    WinGAFPanel(GameFrame *f, WinGAFCanvas *canvas_in,
		int left, int width, int height, int nlbls = 0);
    inline void SetGame(MultiPlayerGame *game_in)
    {
	game = game_in;
    }
    void Auto();
    void Pass();
    int Done();
    void Next();
    void Run();
    void Undo();
    void Redo();
    virtual void HandleButton(int bnum);
    virtual ~WinGAFPanel();
};

class WinGAFFrame: public GameFrame
{
    //virtual void LoadGame();
    void SetGame(MultiPlayerGame *g);
    MultiPlayerGame *AllocateGame(int g, FILE *fp);
    MultiPlayerGame *AllocateGame();
    virtual GamePanel *MakePanel(int x, int w, int h);
    virtual GameCanvas *MakeCanvas(int w, int h);
    virtual GameMenu *MakeMenuBar();
    virtual void AllocateNewGame();
    virtual int AllocateGameForReading(FILE *fp);
    virtual void WriteGame(FILE *fp);
  public:
    WinGAFFrame();
    void UpdateStatusBar();
    void AutoPlay();
    virtual ~WinGAFFrame();
};

inline void WinGAFApp::BackgroundProcess()
{
    frame->AutoPlay();
}

#endif

