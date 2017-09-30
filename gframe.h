/*
 * File:	gframe.h
 * Purpose:	Game Frame base class
 * Author:	Graham Wheeler
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, Graham Wheeler
 */

#ifndef GFRAME_H
#define GFRAME_H

#ifdef __GNUG__
#pragma interface
#endif

//----------------------------------------------------------------

class Game;
class GameMenu;
class GameCanvas;
class GamePanel;

class GameFrame: public wxFrame
{
  protected:
    GamePanel 	*panel;
    GameCanvas  *canvas;
    GameMenu    *menubar;
    Game        *game;
    const char	*fsuf;
    const char	*icon;
    virtual void OnActivate(Bool active);
    virtual void OnSize(int x, int y);
    virtual void OnMenuCommand(int id);
    virtual Bool OnClose();
    // derived classes must define these
    virtual void WriteGame(FILE *fp);
    virtual int ReadGame(FILE *fp);
    virtual void AllocateNewGame() = 0;
    virtual int AllocateGameForReading(FILE *fp) = 0;
    virtual GamePanel *MakePanel(int x, int w, int h) = 0;
    virtual GameCanvas *MakeCanvas(int w, int h) = 0;
    virtual GameMenu *MakeMenuBar() = 0;
  public:
    GameCanvas *GetCanvas() const;
    Game *GetGame() const;
    // Menu handlers
    virtual void LoadGame();
    virtual void SaveGame();
    virtual void NewGame();
    virtual void Init();
    GameFrame(char *title, int w, int h, const char *icon_in, 
			const char *fsuf_in);
    virtual ~GameFrame();
};

inline Game *GameFrame::GetGame() const
{
    return game;
}

inline GameCanvas *GameFrame::GetCanvas() const
{
    return canvas;
}

#endif

