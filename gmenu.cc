/*
 * File:	gmenu.cc
 * Purpose:	Game Menu base class
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
#include "wx_help.h"
#include "ggame.h"
#include "gpanel.h"
#include "gdialog.h"
#include "gcanvas.h"
#include "gmenu.h"
#include "gframe.h"

#ifdef wx_msw
#include "mmsystem.h"
#endif

wxHelpInstance *help = 0;

//------------------------------------------------------------------------
// The various menu bars for the different modes

wxMenu *GameMenu::MakeFileMenu()
{
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(GM_NEW_GAME,  "&New game",  "Start a new game");
    file_menu->Append(GM_LOAD_GAME, "&Load game", "Load a saved game");
    file_menu->Append(GM_SAVE_GAME, "&Save game", "Save the current game");
    file_menu->AppendSeparator();
    file_menu->Append(GM_QUIT,      "&Quit",               "Quit program");
    return file_menu;
}

wxMenu *GameMenu::MakeOptionsMenu()
{
    wxMenu *options_menu = new wxMenu;
    options_menu->Append(GM_COLOURS, "&Configure Colours",       "Change the colour of things");
    return options_menu;
}

wxMenu *GameMenu::MakeHelpMenu()
{
    wxMenu *help_menu = new wxMenu;
    help_menu->Append(GM_INDEX, "&Help Index",            "View help table of contents");
    help_menu->Append(GM_SEARCH,"&Search for Help On...", "Search help using a keyword");
    help_menu->Append(GM_HELPHELP,"&Using Help",            "About the help system");
    help_menu->AppendSeparator();
    char buf[80];
    sprintf(buf, "&About %s", appname);
    help_menu->Append(GM_ABOUT, buf, buf+1);
    return help_menu;
}

void GameMenu::MakeMiddleMenus()
{
}

GameMenu::GameMenu(char *shortappname_in, char *appname_in, 
		   char *description_in,
		   char *inifile_in, GameCanvas *canvas_in,
		   int numextra)
    : wxMenuBar(), shortappname(shortappname_in), appname(appname_in), 
	description(description_in),
	ini(inifile_in), canvas(canvas_in),
	nummenus(numextra+3)
{
    if (help == 0)
    {
	help = new wxHelpInstance(TRUE);
	help->Initialize(shortappname);
    }
}

void GameMenu::Enable()
{
    for (int i = 0; i < nummenus; i++)
	EnableTop(i, TRUE);
}

void GameMenu::Disable()
{
    for (int i = 0; i < nummenus; i++)
	EnableTop(i, FALSE);
}

void GameMenu::Init()
{
    Append(MakeFileMenu(), "&File");
    MakeMiddleMenus();
    Append(MakeOptionsMenu(),  "&Options");
    Append(MakeHelpMenu(),  "&Help");
    DisableGameSpecific();
}

void GameMenu::NewGame()
{
    ((GameFrame*)menu_bar_frame)->NewGame();
}

void GameMenu::LoadGame()
{
    ((GameFrame*)menu_bar_frame)->LoadGame();
}

void GameMenu::SaveGame()
{
    ((GameFrame*)menu_bar_frame)->SaveGame();
}

int GameMenu::Quit()
{
    return 1;
}

void GameMenu::EnableGameSpecific()
{
}

void GameMenu::DisableGameSpecific()
{
}

int GameMenu::Handle(int id)
{
    switch(id)
    {
    case GM_NEW_GAME:
	NewGame();
	break;
    case GM_LOAD_GAME:
	LoadGame();
	break;
    case GM_SAVE_GAME:
	SaveGame();
	break;
    case GM_QUIT:
	return Quit();
    case GM_COLOURS:
	RunDialog(new ConfigureColoursDlg(canvas, ini));
	break;
    case GM_INDEX:
	help->DisplayContents();
	break;
    case GM_SEARCH:
	{
	    char *kw = wxGetTextFromUser("Keyword");
	    if (kw) help->KeywordSearch(kw); // who deletes the keyword??
	}
	break;
    case GM_HELPHELP:
	help->KeywordSearch("Help");
	break;
    case GM_ABOUT:
	{
	    char buf[80];
	    sprintf(buf, "About %s", appname);
	    (void)wxMessageBox(description, buf, wxOK|wxCENTRE);
	}
	break;
    }
    return 0;
}



