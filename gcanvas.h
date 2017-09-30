/*
 * File:	gcanvas.h
 * Purpose:	General purpose game canvas class
 * Author:	Graham Wheeler
 * Created:	1996
 * Updated:	
 * Copyright:	(c) 1996, Graham Wheeler
 */

#ifndef GCANVAS_H
#define GCANVAS_H

#ifdef __GNUG__
#pragma interface
#endif

// The colours we support

#ifdef wx_msw
#define C_BLACK			0
#define C_BLUE			1
#define C_BLUE_VIOLET		2
#define C_CORAL			3
#define C_CORNFLOWER_BLUE	4
#define C_CYAN			5
#define C_GREY			6
#define C_GREEN			7
#define C_LIGHT_GREY		8
#define C_MAGENTA		9
#define C_MEDIUM_SLATE_BLUE	10
#define C_MEDIUM_SPRING_GREEN	11
#define C_ORANGE_RED		12
#define C_RED			13
#define C_SALMON		14
#define C_SLATE_BLUE		15
#define C_SPRING_GREEN		16
#define C_WHITE			17
#define C_YELLOW		18
#define C_NUMCOLOURS		19
#endif
#ifdef wx_x
#define C_AQUAMARINE		0
#define C_BLACK			1
#define C_BLUE			2
#define C_BLUE_VIOLET		3
#define C_BROWN			4
#define C_CADET_BLUE		5
#define C_CORAL			6
#define C_CORNFLOWER_BLUE	7
#define C_CYAN			8
#define C_DARK_GREY		9
#define C_DARK_GREEN		10
#define C_DARK_OLIVE_GREEN	11
#define C_DARK_ORCHID		12
#define C_DARK_SLATE_BLUE	13
#define C_DARK_SLATE_GREY	14
#define C_DARK_TURQUOISE	15
#define C_DIM_GREY		16
#define C_FIREBRICK		17
#define C_FOREST_GREEN		18
#define C_GOLD			19
#define C_GOLDENROD		20
#define C_GREY			21
#define C_GREEN			22
#define C_GREEN_YELLOW		23
#define C_INDIAN_RED		24
#define C_KHAKI			25
#define C_LIGHT_BLUE		26
#define C_LIGHT_GREY		27
#define C_LIGHT_STEEL_BLUE	28
#define C_LIME_GREEN		29
#define C_MAGENTA		30
#define C_MAROON		31
#define C_MEDIUM_AQUAMARINE	32
#define C_MEDIUM_BLUE		33
#define C_MEDIUM_FOREST_GREEN	34
#define C_MEDIUM_GOLDENROD	35
#define C_MEDIUM_ORCHID		36
#define C_MEDIUM_SEA_GREEN	37
#define C_MEDIUM_SLATE_BLUE	38
#define C_MEDIUM_SPRING_GREEN	39
#define C_MEDIUM_TURQUOISE	40
#define C_MEDIUM_VIOLET_RED	41
#define C_MIDNIGHT_BLUE		42
#define C_NAVY			43
#define C_ORANGE		44
#define C_ORANGE_RED		45
#define C_ORCHID		46
#define C_PALE_GREEN		47
#define C_PINK			48
#define C_PLUM			49
#define C_PURPLE		50
#define C_RED			51
#define C_SALMON		52
#define C_SEA_GREEN		53
#define C_SIENNA		54
#define C_SKY_BLUE		55
#define C_SLATE_BLUE		56
#define C_SPRING_GREEN		57
#define C_STEEL_BLUE		58
#define C_TAN			59
#define C_THISTLE		60
#define C_TURQUOISE	     	61
#define C_VIOLET		62
#define C_VIOLET_RED		63
#define C_WHEAT			64
#define C_WHITE			65
#define C_YELLOW		66
#define C_YELLOW_GREEN		67
#define C_NUMCOLOURS		68
#endif
				 
class ColourResource
{
    const char *name;
    const char *id;
    int defval;
    int val;
  public:
    ColourResource(const char *name_in, const char *id_in, int defval_in);
    const char *Name() const;
    const char *ID() const;
    int DefaultValue() const;
    int Value() const;
    void SetValue(int value_in);
    void Load(const char *inifile, const char *section = "Colours");
    void Save(const char *inifile, const char *section = "Colours");
    void Select(wxDC *dc);
    ~ColourResource() {}
};

// flags for grid drawing

#define GR_NOBORDER	1	// don't draw outermost lines
#define GR_VERTEX	2	// displace for games that play on vertices
#define GR_GAMMON	4	// kludge for backgammon

class GameCanvas: public wxCanvas
{
  protected:
    wxDC *dc;
    unsigned flags;
    Game *game;
    int rows, cols, keepsquare, rightspace, bottomspace;
    int squarewidth, squareheight, boardwidth, boardheight;
    int numresources;
    ColourResource **resources;
    friend class ConfigureColoursDlg;

    void SetResource(int rnum, const char *name, const char *id, int val);
    void SetResource(const char *inifile, int rnum, const char *name,
	 	const char *id, int val);
    void SetColour(int rnum);
    void SelectPen(int cnum);
    void SelectResourcePen(int rnum);
    void SelectBrush(int cnum);
    void SelectResourceBrush(int rnum);
    void ClearSurroundingArea();
    void DrawGrid(int x, int y, int nr, int nc, int w, int h);
    void DrawGrid();
    void DrawPlus(int x, int y, int xtent);
    void DrawX(int x, int y, int xtent);
    void DrawBox(int x, int y, int xtent);
    void DrawCenteredString(int x, int y, int xtent, char *str);
    void DrawCenteredLetter(int x, int y, int xtent, int ch);
    void DrawArrow(int x, int y, int len, int dx, int dy);
    void DrawLeftArrow(int x, int y, int len);
    void DrawRightArrow(int x, int y, int len);
    void DrawDownArrow(int x, int y, int len);
    void DrawUpArrow(int x, int y, int len);
    void SquareToCenterPixel(int &r, int &c);
    void DrawRoundedRectangle(int x, int y, int w, int h);
    void DrawBoardSquare(int r, int c, int border = 1, int rnum = -1);
    void DrawButton(int frnum, int brnum, int x, int y, int radius);

    virtual void HandleLeftMouseClickOnBoard(int r, int c);
    virtual void HandleRightMouseClickOnBoard(int r, int c);
    virtual void HandleMouseMoveOnBoard(int r, int c);
    virtual void HandleLeftMouseClickOffBoard(int x, int y);
    virtual void HandleRightMouseClickOffBoard(int x, int y);
    virtual void HandleMouseMoveOffBoard(int x, int y);
    virtual void HandleKey(int ch);

    void SetBoardDimensions(int rows_in, int cols_in, int keepsquare_in,
			    int rightspace_in = 0, int bottomspace_in = 0);
  public:
    GameCanvas(wxFrame *frame, int w, int h, int numresources_in=1,
			unsigned flags_in = 0);
    void AdjustBoardDimensions();
    void Resize(int width, int height);
    void SetFlags(unsigned flags_in);
    virtual void Clear();
    virtual void DrawSquare(int r, int c);
    virtual void DrawBoard();
    void PrintBoard();
    virtual void OnPaint();
    virtual void OnScroll(wxCommandEvent &event);
    virtual void OnEvent(wxMouseEvent& event);
    virtual void OnChar(wxKeyEvent& event);
    virtual void SetGame(Game *game_in);
    virtual ~GameCanvas() ;
};

class ConfigureColoursDlg : public GameDialog
{
    wxChoice **opt;
    GameCanvas *canvas;
    const char *inifile;
    ColourResource **resources;
    int nr;
    int *oldvals;
    enum { DoneID, SaveID };
  public:
    ConfigureColoursDlg(GameCanvas *canvas_in, const char *inifile_in);
    void Test();
    void Save();
    virtual void HandleButton(int bnum);
    ~ConfigureColoursDlg();
};

//---------------------------------------------------------------------

inline ColourResource::ColourResource(const char *name_in, 
				      const char *id_in, int defval_in)
    : name(name_in), id(id_in), defval(defval_in), val(defval_in)
{
}

inline const char *ColourResource::Name() const
{
    return name;
}

inline const char *ColourResource::ID() const
{
    return id;
}

inline int ColourResource::DefaultValue() const
{
    return defval;
}

inline int ColourResource::Value() const
{
    return val;
}

inline void ColourResource::SetValue(int value_in)
{
    val = value_in;
}

inline void GameCanvas::SetFlags(unsigned flags_in)
{
    flags = flags_in;
}

#endif

