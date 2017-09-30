/*
 * File:	ggame.h
 * Purpose:	Common Game Interface
 * Author:	Graham Wheeler
 * Created:	1996
 * Updated:	
 * Copyright:	(c) 1996, Graham Wheeler
 */

#ifndef GGAME_H
#define GGAME_H

// This is the minimally required class. It is the immediate parent of
// an Xword `game'

class Game
{
  protected:
    int playernum;
  public:
    Game(int playernum_in = 0);
    virtual int Undo(int mvcnt = 1);
    virtual int Redo(int mvcnt = 1);
    virtual void Save(FILE *fp);
    virtual int Load(FILE *fp);
    virtual int Height() const;
    virtual int Width() const;
    virtual int GameOver(unsigned *flags = 0);
    virtual void AutoPlay(int nmoves = 1);
    virtual void HandleBoardSelect(int r, int c) = 0;
    virtual int IsSelected(int r, int c) const = 0;
    virtual ~Game()
    {}
};

inline Game::Game(int playernum_in)
    : playernum(playernum_in)
{
}

#endif

