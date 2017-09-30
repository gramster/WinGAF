/*
 * File:	gmenu.cc
 * Purpose:	Game Menu base class
 * Author:	Graham Wheeler
 * Created:	1996
 * Updated:	
 * Copyright:	(c) 1996, Graham Wheeler
 */

//------------------------------------------------------------------------
// The common part of game menu bars

#ifndef GMENU_H
#define GMENU_H

#define GM_QUIT       21723
#define GM_LOAD_GAME  21724
#define GM_SAVE_GAME  21725
#define GM_NEW_GAME   21726
#define GM_ABOUT      21727
#define GM_INDEX      21728
#define GM_SEARCH     21729
#define GM_HELPHELP   21730
#define GM_COLOURS    21731

class GameCanvas;

class GameMenu : public wxMenuBar
{
  protected:
    char *shortappname;
    char *appname;
    char *description;
    char *ini;
    Game *game;
    GameCanvas *canvas;
    int nummenus;
    virtual wxMenu *MakeFileMenu();
    virtual wxMenu *MakeOptionsMenu();
    virtual wxMenu *MakeHelpMenu();
    virtual void MakeMiddleMenus();
    virtual void NewGame();
    virtual void LoadGame();
    virtual void SaveGame();
    virtual int Quit();
    virtual void EnableGameSpecific();
    virtual void DisableGameSpecific();
  public:
    GameMenu(char *shortappname_in, char *appname_in, char *description_in,
	     char *inifile_in, GameCanvas *canvas_in, int numextra);
    virtual void SetGame(Game *game_in);
    virtual void Init();
    virtual int Handle(int id);
    void Enable();
    void Disable();
    virtual ~GameMenu()
    {}
};

inline void GameMenu::SetGame(Game *game_in)
{
    game = game_in;
    if (game) EnableGameSpecific();
    else DisableGameSpecific();
}

#endif

