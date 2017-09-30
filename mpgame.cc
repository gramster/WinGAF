#include <stdio.h>
#include "mpggame.h"

void Board::Create(int rows_in, int cols_in)
{
    if (squares) Delete();
    rows = rows_in;
    cols = cols_in;
    squares = new BoardSquare*[rows];
    for (int r = 0; r < rows; r++)
	squares[r] = new BoardSquare[cols];
}

void Board::Copy(const Board &b)
{
    if (b.Rows() != rows || b.Cols() != cols)
        Create(b.Rows(), b.Cols());
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
	    squares[r][c] = b.squares[r][c];
}

void Board::Delete()
{
    for (int r = 0; r < rows; r++)
	delete [] squares[r];
    delete [] squares;
    squares = 0;
    rows = 0;
}

Board::Board(int rows_in, int cols_in)
    : rows(0), squares(0)
{
    Create(rows_in, cols_in);    
}

Board::Board(const Board &b)
    : rows(0), squares(0)
{
    Copy(b);
}

int Board::CountOwners(int *counts) const
{
    int ecnt = 0;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
	    if (IsEmpty(r,c)) ecnt++;
	    else if (counts) counts[Owner(r,c)]++;
    return ecnt;
}

int CountOwnersInRow(int r, int *counts) const
{
    int ecnt = 0;
    for (int c = 0; c < cols; c++)
	if (IsEmpty(r,c)) ecnt++;
	else if (counts) counts[Owner(r,c)]++;
    return ecnt;
}

int CountOwnersInCol(int c, int *counts) const
{
    int ecnt = 0;
    for (int r = 0; r < rows; r++)
	if (IsEmpty(r,c)) ecnt++;
	else if (counts) counts[Owner(r,c)]++;
    return ecnt;
}

Board::~Board()
{
    Delete();
}

//----------------------------------------------------------------------

MoveGenerator::MoveGenerator(const Board *b_in, const int move_in,
				const int p_in)
	: b(b_in), movenum(move_in), player(p_in)
{
}

//-----------------------------------------------------------------
// Placement move generator

Move *PlaceMoveGenerator::NextMove()
{
    while (++pos < b->Size())
	if (IsEmpty(pos))
	    return new PlaceMove(pos);
    return 0;
}

//---------------------------------------------------------------------------

void GameRecord::Save(FILE *fp)
{
    MoveRecord *mr = firstmove;
    if (mr) mr = mr->Next(); // skip dummy entry
    int mcnt = movenum;
    while (mr && --mcnt>0)
    {
	mr->Move()->Write(fp);
	fputc('\n', fp);
	mr = mr->Next();
    }
}

GameRecord::~GameRecord()
{
    while (firstmove)
    {
	MoveRecord *tmr = gamerecord;
	firstmove = firstmove->Next();
	delete tmr;
    }
}

//-----------------------------------------------------------------

MultiPlayerGame::MultiPlayerGame(Board *b_in = 0)
    : Game(), gamerecord(0), board(b_in), movenum(0), autoplay(0),
	level(3)
{
}

int MultiPlayerGame::FindMove(const int lookahead, const int mvnum, const int player,
	const Board* &b_in, move_t* &move, int pweight, generator_t *pgen)
{
    int rtn = UNKNOWN;
    move_t *nxtmv, *mv;
    generator_t *gen;
    if (lookahead <= 0 || GameOver(b_in, player))
    {
	rtn = Weight(b_in, player); // terminal node
	return rtn;
    }
    gen = Generator(b_in, mvnum, player, pgen);
    for (mv = gen->NextMove(),
	 CopyMove(move, mv)
	 ; mv
	 ; mv = gen->NextMove())
    {
	Board *nb = NewBoard(b_in);
	(void)ApplyMove(nb, mv, mvnum, player);
	move_t *m = NewMove();

	int wn = FindMove(lookahead-1, mvnum+1,
			(player == COMPUTER) ? HUMAN : COMPUTER,
			(const Board* &)nb, m, rtn, gen);
	delete m;
	delete nb;

	cnt++;
	if (player == COMPUTER) // maximising?
	{
#ifdef ABPRUNE
	    if (wn >= pweight && pweight!=UNKNOWN)
	    {
		rtn = wn;
		goto done; // prune
	    }
#endif
	    // The random choice here must only be at lookahead
	    // one as pruning confuses it.
#if __BORLANDC__
	    if (wn > rtn || rtn==UNKNOWN || (lookahead==1 && wn==rtn && random(2)))
#else
	    if (wn > rtn || rtn==UNKNOWN || (lookahead==1 && wn==rtn && random()%2))
#endif
	    {
		rtn = wn;
		CopyMove(move, mv);
	    }
	}
	else // human move; minimise
	{
#ifdef ABPRUNE
	    if (wn <= pweight && pweight!=UNKNOWN)
	    {
		rtn = wn;
		goto done; // prune
	    }
#endif
	    if (wn < rtn || rtn==UNKNOWN)
	    {
		rtn = wn;
		CopyMove(move, mv); // for debugging
	    }
	}
	delete mv;
	EraseMove(mvnum); // take out of record & fix lastmove
    }
done:
    if (mv)
    {
	delete mv;
	EraseMove(mvnum);
    }
    delete gen;
    if (rtn == UNKNOWN) rtn = 0;
    if (rtn < (FIRSTLOSE-25))
    	rtn+=25; // prolong the agony; maybe opponent will slip up
    else if (rtn > (FIRSTWIN+25))
    	rtn-=25; // win as fast as possible
    if (cnt == 0)
    	if (blockwins)
    	    rtn = (player==HUMAN) ? FIRSTWIN : FIRSTLOSE;
    	else if (blockloses)
    	    rtn = (player==COMPUTER) ? FIRSTWIN : FIRSTLOSE;
    return rtn;
}

void MultiPlayerGame::AutoPlay(int nmoves)
{
    while (nmoves-- > 0)
    {
        Move *m = NewMove();
        (void)FindMove(level, movenum, nextplayer, (const Board* &)b,
			m, UNKNOWN);
        int prt = ApplyMove(m, movenum, nextplayer);
        delete m;
        if (prt) b->Print();
        nextplayer = (nextplayer+1) % numplayers;
        movenum++;
    }
}

#if 0
moverecord_t *game_t::RecordMove(int movenum_in, Board *board_in, move_t *move_in)
{
    moverecord_t *m = gamerecord, *p = NULL,
		*newm = new moverecord_t(board_in, move_in, NULL);
    assert(newm);
    while (movenum_in-->0)
    {
	assert(m);
	p = m;
	m = m->Next();
    }
    if (m) // Replacing existing entries? Delete 'em...
    {
	if (p) p->Link(NULL);
	while (m)
	{
	    moverecord_t *tm = m;
	    m = m->Next();
	    delete tm;
	}
    }
    if (p) p->Link(newm);
    else gamerecord = newm; // first move (guaranteed by assert)
    return lastmove = newm;
}

void game_t::EraseMove(int movenum_in)
{
    moverecord_t *m = gamerecord, *p = NULL;
    assert(movenum_in>0);
    while (movenum_in-->0)
    {
	assert(m);
	p = m;
	m = m->Next();
    }
    assert(p);
    lastmove = p;
    p->Link(NULL);
    while (m)
    {
	moverecord_t *tm = m;
	m = m->Next();
	delete tm;
    }
}

void MultiPlayerGame::Save(FILE *fp)
{
    gamerecord->Save(fp);
}

int MultiPlayerGame::Load(FILE *fp)
{
    assert(gamerecord); // must have first move
    Init();
    fscanf(fp,"First %d", &firstplayer);
    nextplayer = firstplayer;
    (void)RecordMove(0, NewBoard((const Board *)b), NULL);
    movenum = 1;
    move_t *m = NewMove();
    while (m->Read(fp))
    {
	(void)ApplyMove(m, movenum++, nextplayer);
	nextplayer = 3-nextplayer;
	b->Print();
    }
    delete m;
    return 0;
}

int MultiPlayerGame::Undo(int mvcnt)
{
    while (moves > 0 && --mvcnt>=0)
    {
	moves--;
	player = (player+numplayers-1)%numplayers;
	Move *m = gamerecord->Backup();
	Undo(m);
    }
    players[player]->MakeManual();
}

#endif

MultiPlayerGame::~MultiPlayerGame()
{
    delete gamerecord;
}

//----------------------------------------------------------------------

Move *MoveGame::TranslateMove(char *buf)
{
    int from, to, cnt;
    cnt = sscanf(buf, "%d %d", &from, &to);
    if (cnt==0) // Try letters; needed for games > size 9; ZZ is off-board
    {
	char c1,c2,c3,c4;
	cnt = sscanf(buf, "%c%c %c%c", &c1, &c2, &c3, &c4);
	if (cnt != 2 && cnt != 4) return NULL;
	if (c1>'Z') c1 -= 'a'; else c1 -= 'A';
	if (c2>'Z') c2 -= 'a'; else c2 -= 'A';
	if (cnt == 2)
	{
	    from = -1;
	    to = c1 * brd.Cols() + c2;
	}
	else
	{
	    from = c1 * brd.Cols() + c2;
	    if (c3>'Z') c3 -= 'a'; else c3 -= 'A';
	    if (c4>'Z') c4 -= 'a'; else c4 -= 'A';
	    if (c3==25 && c4==25)
	        to = -1;
	    else to = c3 * brd.Cols() + c4;
	}
    }
    else // numbers
    {
	if (cnt==1)
	{
	    to = from;
	    from = -1;
	}
	else if (cnt!=2) return 0;
	if (to>=0) to = (to/10-1)*brd.Cols()+(to%10-1);
	if (from>=0) from = (from/10-1)*brd.Cols()+(from%10-1);
    }
    BoardMove *rtn = new BoardMove(from, to);
    assert(rtn);
    if (!Validate(b, HUMAN, rtn))
    {
	delete rtn;
	rtn = 0;
    }
    return rtn;
}

//---------------------------------------------------------------------------
// Tic-tac-toe test game

class tictactoe : public PlaceGame
{
public:
    tictactoe(int bsize)
	: PlaceGame(bsize)
    { }
    int Weight(const Board* &b_in, int player);
};

int tictactoe::Weight(const Board* &b, int player)
{
    GridBoard_t *B = (GridBoard_t *)b;
    int sz = B->Cols();
    (void)player; // has no influence in xxx
    for (int p = 1; p<=2; p++)
    {
	int r, c;
	// check for win across
	for (r=0; r<sz; r++)
	{
	    for (c=0; c<sz; c++)
		if (B->get(r,c)!=p)
		    break;
	    if (c==sz) goto win;
	}
	// check for win down
	for (c=0; c<sz; c++)
	{
	    for (r=0; r<sz; r++)
		if (B->get(r,c)!=p)
		    break;
	    if (r==sz) goto win;
	}
	// check for diagonal win
	for (r=0; r<sz; r++)
	    if (B->get(r,r)!=p)
		break;
	if (r==sz) goto win;
	for (r=0; r<sz; r++)
	    if (B->get(r,sz-r-1)!=p)
		break;
	if (r==sz) goto win;
    }
    // check for draw
    return (B->full() ? DRAW : EVEN);
win:
    return (p==COMPUTER) ? MAXWEIGHT : MINWEIGHT;
}

Game *NewGame(int sz)
{
    return new tictactoe(sz);
}
