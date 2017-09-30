#include <stdio.h>
#include "ggame.h"

int Game::Undo(int mvcnt)
{
    return 0;
}

int Game::Redo(int mvcnt)
{
    return 0;
}

void Game::Save(FILE *fp)
{
}

int Game::Load(FILE *fp)
{
    return 0;
}

int Game::Height() const
{
    return 1;
}

int Game::Width() const
{
    return 1;
}

int Game::GameOver(unsigned *flags)
{
    if (flags) *flags = 0;
    return 0;
}

void Game::AutoPlay(int nmoves)
{
}

