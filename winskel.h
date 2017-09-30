/*
 * File:	winskel.h
 * Purpose:	Skeleton game
 * Author:	Graham Wheeler
 * Created:	1996
 * Updated:	
 * Copyright:	(c) 1996, Graham Wheeler
 */

#ifndef WINSKEL_H
#define WINSKEL_H

#ifdef __GNUG__
#pragma interface
#endif

// Initialisation file

#ifdef wx_msw
#define ini	"winskel.ini"
#endif
#ifdef wx_x
#define ini	".winskel"
#endif

class SkelCanvas;
class SkelFrame;

//----------------------------------------------------------------

class SkelGame : public Game
{
    virtual void Save(FILE *fp);
    virtual int Load(FILE *fp);
    virtual int Width() const;
    virtual int Height() const;
  public:
    SkelGame();
    virtual ~SkelGame();
};

class SkelApp: public wxApp
{
    char *SkelApp::MakeDictionary();
  public:
    SkelApp() {}
    wxFrame *OnInit();
};

class SkelMenuBar : public GameMenu
{
  protected:
    //virtual void MakeMiddleMenus();
    //virtual void EnableGameSpecific();
    //virtual void DisableGameSpecific();
  public:
    SkelMenuBar(GameCanvas *canvas_in);
    //virtual int Handle(int id);
    virtual ~SkelMenuBar();
};

class SkelCanvas: public GameCanvas
{
  protected:
    //virtual void HandleLeftMouseClickOnBoard(int r, int c);
    //virtual void HandleRightMouseClickOnBoard(int r, int c);
    //virtual void HandleLeftMouseClickOffBoard(int x, int y);
    //virtual void HandleKey(int ch);
  public:
    SkelCanvas(GameFrame *frame, int w, int h);
    //virtual void SetGame(Game *game_in);
    //virtual void DrawSquare(int r, int c);
    //virtual void DrawBoard();
    virtual ~SkelCanvas() ;
};

class SkelPanel : public GamePanel
{
  public:
    SkelPanel(GameFrame *f, int left, int width, int height);
    virtual ~SkelPanel();
};

class SkelFrame: public GameFrame
{
    //virtual void LoadGame();
    virtual GamePanel *MakePanel(int x, int w, int h);
    virtual GameCanvas *MakeCanvas(int w, int h);
    virtual GameMenu *MakeMenuBar();
    virtual void AllocateNewGame();
    virtual int AllocateGameForReading(FILE *fp);
  public:
    SkelFrame();
    virtual ~SkelFrame();
};

#endif

