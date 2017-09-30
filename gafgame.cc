#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include "ggame.h"
#include "gafgame.h"

#ifndef max
#define max(a, b)	( ((a)>(b)) ? (a) : (b) )
#endif
#ifndef min
#define min(a, b)	( ((a)>(b)) ? (b) : (a) )
#endif

#define UNKNOWN	0 // for now?

#ifdef DRAWTREE
TreeNode *tree = 0;
#endif

FILE *tron = 0;

//---------------------------------------------------------------

int TreeNode::CalcLayout(int xx, int yy)
{
    if (firstchild)
    {
	int lastx = firstchild->CalcLayout(xx, yy+1);
	x = xx + (lastx - xx) / 2;
	xx = lastx;
    }
    else // terminal node
	x = xx;
    y = yy;
    // do siblings
    if (nextsibling)
	xx = nextsibling->CalcLayout(xx+6, yy);
    return xx;
}

//---------------------------------------------------------------

void BoardSquare::Print(FILE *fp) const
{
    if (state & BS_EMPTY) fputc('.', fp);
    else fprintf(fp, "%d", owner);
}

void Board::Print(FILE *fp) const
{
    for (int i = 0; i < rows; i++)
    {
    	for (int j = 0; j < cols; j++)
	    squares[i][j].Print(fp);
	fputc('\n', fp);
    }
}

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

int Board::Step(int &r, int &c) const
{
    c++;
    if (c >= Cols())
    {
	c = 0;
	r++;
	if (r >= Rows())
	    return -1;
    }
    return 0;
}

Board::Board(const Board &b)
    : rows(0), squares(0)
{
    Copy(b);
}

Board::Board(const Board *b)
    : rows(0), squares(0)
{
    Copy(*b);
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

int Board::CountOwnersInRow(int r, int *counts) const
{
    int ecnt = 0;
    for (int c = 0; c < cols; c++)
	if (IsEmpty(r,c)) ecnt++;
	else if (counts) counts[Owner(r,c)]++;
    return ecnt;
}

int Board::CountOwnersInCol(int c, int *counts) const
{
    int ecnt = 0;
    for (int r = 0; r < rows; r++)
	if (IsEmpty(r,c)) ecnt++;
	else if (counts) counts[Owner(r,c)]++;
    return ecnt;
}

int Board::CountOwnersInAscendingDiagonal(int r, int c, int n, int *counts) const
{
    int ecnt = 0;
    for (int i = 0; i < n; i++)
    {
	if (IsEmpty(r-i,c+i)) 
	    ecnt++;
	else if (counts) 
	    counts[Owner(r-i,c+i)]++;
    }
    return ecnt;
}

int Board::CountOwnersInDescendingDiagonal(int r, int c, int n, int *counts) const
{
    int ecnt = 0;
    for (int i = 0; i < n; i++)
    {
	if (IsEmpty(r+i,c+i)) 
	    ecnt++;
	else if (counts) 
	    counts[Owner(r+i,c+i)]++;
    }
    return ecnt;
}

void Board::Empty()
{
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
	    SetEmpty(r, c);
}

Board::~Board()
{
    Delete();
}


//-----------------------------------------------------------------

void PlaceMove::Print(FILE *fp) const
{
    fprintf(fp, "Move to %d,%d\n", r, c);
}

void BoardMove::Print(FILE *fp) const
{
    fprintf(fp, "Move from %d, %d to %d,%d\n", fr, fc, tr, tc);
}

//-----------------------------------------------------------------
// Placement move generator - returns next empty square

Move *PlaceMoveGenerator::NextMove()
{
    PlaceMove *rtn = new PlaceMove();
    for (;;)
    {
	if (b->Step(r, c) != 0)
	    break; // no more
	rtn->Set(r, c);
	if (game->Validate(b, player, rtn))
	    return rtn;
    }
    delete rtn;
    return 0;
}

//-----------------------------------------------------------------
// pawn move generator

Move *PawnMoveGenerator::NextMove()
{
    int tr, tc;
    int cols = b->Cols();
    int rows = b->Rows();
    BoardMove *rtn = 0;
    for (;;)
    {
	int tr = fr + ((player == 0) ? -1: 1);
	if (tr < 0 || tr >= rows || cnt>=3 || b->IsEmpty(fr, fc) ||
		b->Owner(fr, fc) != player)
	{
	    if (b->Step(fr, fc) != 0)
		break;
	    cnt = 0;
	    continue;
	}
	cnt++;
	int tc = fc + cnt - 2;
	if (tc>=0 && tc < cols)
	{
	    if (b->IsEmpty(tr, tc))
	    {
	        if (cnt==2)
		{
		    rtn = new BoardMove(fr, fc, tr, tc, -1);
		    break;
		}
	    }
	    else if (b->Owner(tr, tc) != player)
	    {
		// if cnt is 1 mustn't be on left edge
		// if cnt is 3 mustn't be on right edge
		if ((cnt==1 && fc>0) ||
		    (cnt==3 && fc<(cols-1)))
		{
		    rtn = new BoardMove(fr, fc, tr, tc, b->Owner(tr,tc));
		    break;
		}
	    }
	}
    }
    if (tron)
    {
	fprintf(tron, "\n\n\nGenerator returns:\n\t");
	if (rtn) rtn->Print(tron);
	else fprintf(tron, "no more moves\n"); 
    }
    return rtn;
}

//---------------------------------------------------------------------------
// Move record list for save/undo/redo

void GameRecord::Save(FILE *fp)
{
    MoveRecord *mr = first;
    int mcnt = movenum;
    while (mr && --mcnt>=0)
    {
	mr->GetMove()->Write(fp);
	fputc('\n', fp);
	mr = mr->Next();
    }
}

MoveRecord *GameRecord::RecordMove(int movenum_in, Board *board_in, Move *move_in)
{
    MoveRecord *m = first, *p = 0, *newm;
    newm = new MoveRecord(board_in, move_in, 0);
    assert(newm);
    movenum = movenum_in+1;
    while (movenum_in-->0)
    {
	assert(m);
	p = m;
	m = m->Next();
    }
    if (m) // Replacing existing entries? Delete 'em...
    {
	if (p) p->Link(0);
	while (m)
	{
	    MoveRecord *tm = m;
	    m = m->Next();
	    delete tm;
	}
    }
    if (p) p->Link(newm);
    else first = newm; // first move (guaranteed by assert)
    return newm;
}

void GameRecord::EraseMove(int movenum_in)
{
    MoveRecord *m = first, *p = 0;
    assert(movenum_in>0);
    while (movenum_in-->0)
    {
	assert(m);
	p = m;
	m = m->Next();
    }
    assert(p);
    p->Link(0);
    while (m)
    {
	MoveRecord *tm = m;
	m = m->Next();
	delete tm;
    }
}

MoveRecord *GameRecord::Backup()
{
    MoveRecord *mr = first;
    int mcnt = movenum;
    while (mr && --mcnt>0)
	mr = mr->Next();
    if (movenum) movenum--;
    return mr;
}

Move *GameRecord::Forward()
{
    MoveRecord *mr = first;
    int mcnt = movenum;
    while (mr && --mcnt>=0)
	mr = mr->Next();
    if (mr)
    {
	movenum++;
	return mr->GetMove();
    }
    return 0;
}

void GameRecord::Empty()
{
    while (first)
    {
	MoveRecord *tmr = first;
	first = first->Next();
	delete tmr;
    }
    first = 0;
    movenum = 0;
}

GameRecord::~GameRecord()
{
    Empty();
}

//---------------------------------------------------------------

int MultiPlayerGame::Height() const
{
    return board->Rows();
}

int MultiPlayerGame::Width() const
{
    return board->Cols();
}

void MultiPlayerGame::StopAuto()
{
    automask = 0l;
}

void MultiPlayerGame::MakePlayerAuto()
{
    automask |= (1l << nextplayer);
}

int MultiPlayerGame::IsAutoPlayer()
{
    return ((automask & (1l << nextplayer)) != 0);
}

void MultiPlayerGame::MakePlayerManual()
{
    automask &= ~(1l << nextplayer);
}

char *MultiPlayerGame::PlayMove(char *input)
{
    Move *m = TranslateMove(input);
    if (m)
    {
	gamerecord->RecordMove(movenum, NewBoard(board), NewMove(m));
        ApplyMove(board, m, movenum++, nextplayer);
        nextplayer = (nextplayer+1) % numplayers;
        return 0;
    }
    return "Invalid move!";
}

void MultiPlayerGame::PlayNext()
{
}

#ifdef DRAWTREE
int MultiPlayerGame::FindMove(const int lookahead, const int mvnum, const int player,
			      int pnow, const Board *b, Move *move, 
			      int pweight, TreeNode *parent)
#else
int MultiPlayerGame::FindMove(const int lookahead, const int mvnum, const int player,
			      int pnow, const Board *b, Move *move, 
			      int pweight)
#endif
{
    int rtn = UNKNOWN_V, cnt = 0;
    Move *nxtmv, *mv;
    unsigned f;
    int w = Weight(b, player, pnow, f);
#ifdef DRAWTREE
    TreeNode *lastchild = 0;
#endif
    if (tron)
    {
	fprintf(tron, "===================\n");
	fprintf(tron, "In FindMove, lookahead %d mvnum %d player %d pnow %d\nBoard:\n",
		lookahead, mvnum, player, pnow);
	b->Print(tron);
	move->Print(tron);
	static char *results[] = { "Playing", "Draw", "Player %d wins", "Player %d wins" };
	char fbuf[20];
	sprintf(fbuf, results[f&3], WINNER(f));
	fprintf(tron, "Weight %ld  Flags %s\n\n", w, fbuf);
    }
    if (lookahead <= 0 || (f != 0)) // terminal node?
    {
	switch (f&3)
	{
	default:
	    if (tron) 
		fprintf(tron, "Terminal node: player %d pnow %d weight %d\n",
				player, pnow, w);
#ifdef DRAWTREE
	    if (parent) parent->SetWeight(w);
#endif
	    return w;
	case DRAW:
	    if (tron) 
		fprintf(tron, "Terminal node: player %d pnow %d weight %d\n",
				player, pnow, DRAW_V);
#ifdef DRAWTREE
	    if (parent) parent->SetWeight(DRAW_V);
#endif
	    return DRAW_V;
	case WIN:
	    if (tron) 
		fprintf(tron, "Terminal node: player %d pnow %d weight %d\n",
				player, pnow, WIN_V);
	    assert(WINNER(f) == player);
#ifdef DRAWTREE
	    if (parent) parent->SetWeight(WIN_V);
#endif
	    return WIN_V;
	case LOSE:
	    if (tron) 
		fprintf(tron, "Terminal node: player %d pnow %d weight %d\n",
				player, pnow, LOSE_V);
	    assert(WINNER(f) != player);
#ifdef DRAWTREE
	    if (parent) parent->SetWeight(LOSE_V);
#endif
	    return LOSE_V;
	}
    }
    MoveGenerator *gen = NewGenerator(b, mvnum, pnow, 0/*pgen*/);
    for (Move *mv = gen->NextMove();
	 mv != 0 ;
	 mv = gen->NextMove())
    {
	Board *nb = NewBoard(b);
	ApplyMove(nb, mv, mvnum, pnow);
	if (tron)
	{
	    fprintf(tron, "\n\nResulting board:\n");
	    nb->Print(tron);
	    fprintf(tron, "\n\n");
	}
	Move *m = NewMove();
#ifdef DRAWTREE
	if (parent)
	    lastchild = new TreeNode(parent, lastchild, nb);
	int wn = FindMove(lookahead-1, mvnum+1, player, (pnow+1)%numplayers, 
				(const Board*)nb, m, rtn, lastchild);
	delete m;
	if (parent == 0)
	    delete nb;
#else
	int wn = FindMove(lookahead-1, mvnum+1, player, (pnow+1)%numplayers, 
				(const Board*)nb, m, rtn);
	delete m;
	delete nb;
#endif
	cnt++;

	if (pnow == player) // maximise
	{
	    if (tron) fprintf(tron, "Maximising weight %d\n", wn);
#ifdef ABPRUNE
	    if (wn >= pweight && pweight!=UNKNOWN_V)
	    {
	        rtn = wn;
	        delete mv;
	        break;
	    }
	    // The random choice here must only be at lookahead
	    // one as pruning confuses it.
	    if (wn > rtn || rtn==UNKNOWN_V)
	    	// || (lookahead==1 && wn==rtn && random(2))
#else	    
	    if (wn > rtn || rtn==UNKNOWN_V)
	    	// || (wn==rtn && random(2))
#endif
	    {
	        rtn = wn;
	        move->Set(mv);
	    }
	}
	else // minimise
	{
	    if (tron) fprintf(tron, "Minimising weight %d\n", wn);
#ifdef ABPRUNE
	    if (wn <= pweight && pweight!=UNKNOWN_V)
	    {
	        rtn = wn;
	        delete mv;
	        break;
	    }
	    // The random choice here must only be at lookahead
	    // one as pruning confuses it.
	    if (wn < rtn || rtn==UNKNOWN_V)
	    	// || (lookahead==1 && wn==rtn && random(2))
#else	    
	    if (wn < rtn || rtn==UNKNOWN_V)
	    	// || (wn==rtn && random(2))
#endif
	    {
	        rtn = wn;
	        move->Set(mv);
	    }
	}
	delete mv;
    }
done:
    delete gen;
    if (cnt == 0)
    {
    	if (lastmoverwins)
    	    rtn = (player == pnow) ? LOSE_V : WIN_V;
    	else if (lastmoverloses)
    	    rtn = (player == pnow) ? WIN_V : LOSE_V;
	else
	    rtn = DRAW_V;
    }
#ifdef DRAWTREE
    if (parent) 
    {
	parent->SetWeight(rtn);
	parent->IsMax(pnow == player);
    }
#endif
    return rtn;
}

void MultiPlayerGame::AutoPlay(int nmoves)
{
    busy = 1;
    while (nmoves-- > 0)
    {
        Move *m = NewMove();
        (void)FindMove(level, movenum, nextplayer, nextplayer,
			(const Board*)board,
			m, UNKNOWN_V);
	gamerecord->RecordMove(movenum, NewBoard(board), m);
        ApplyMove(board, m, movenum++, nextplayer);
        nextplayer = (nextplayer+1) % numplayers;
    }
    busy = 0;
}

void MultiPlayerGame::Save(FILE *fp)
{
    fprintf(fp, "%d %d\n", board->Rows(), board->Cols());
    gamerecord->Save(fp);
}

int MultiPlayerGame::Load(FILE *fp)
{
    gamerecord->Empty();
    nextplayer = movenum = 0;
    Move *m = NewMove();
    while (m->Read(fp))
    {
	gamerecord->RecordMove(movenum, NewBoard(board), NewMove(m));
	ApplyMove(board, m, movenum++, nextplayer);
	nextplayer = (nextplayer+1)%numplayers;
	Paint();
    }
    delete m;
    return 0;
}

int MultiPlayerGame::Undo(int mvcnt)
{
    while (movenum > 0 && --mvcnt>=0)
    {
	movenum--;
	nextplayer = (nextplayer+numplayers-1)%numplayers;
	MoveRecord *mr = gamerecord->Backup();
	Undo(mr->GetMove(), nextplayer);
	*board = *(mr->GetBoard());
	Paint();
    }
    MakePlayerManual();
    return 0;
}

int MultiPlayerGame::Redo(int mvcnt)
{
    while (--mvcnt>=0)
    {
        Move *m = gamerecord->Forward();
        if (m)
        {
            ApplyMove(board, m, movenum++, nextplayer);
            nextplayer = (nextplayer+1) % numplayers;
	    Paint();
        }
	else return -1;
    }
    return 0;
}

int MultiPlayerGame::GameOver(const Board *b, int player, unsigned *flags)
{
    unsigned f;
    (void)Weight(b, player, player, f);
    if (flags) *flags = f;
    return (f != 0);
}

int MultiPlayerGame::GameOver(unsigned *flags)
{
    return GameOver(board, nextplayer, flags);
}

Board *MultiPlayerGame::NewBoard(int r, int c)
{
    return new Board(r,c);
}

Board *MultiPlayerGame::NewBoard(const Board *b)
{
    return new Board(b);
}

MultiPlayerGame::MultiPlayerGame(Board *board_in, int numplayers_in)
    : Game(), gamerecord(new GameRecord), board(board_in), numplayers(numplayers_in),
      nextplayer(0), automask(0l), busy(0), movenum(0), level(5),
      lastmoverwins(0), lastmoverloses(0)
{
}

MultiPlayerGame::~MultiPlayerGame()
{
    delete gamerecord;
}

//----------------------------------------------------------------------

Move *PlaceGame::NewMove(Move *m)
{
    return new PlaceMove(m);
}

MoveGenerator *PlaceGame::NewGenerator(const Board *b, int mvnum, int player,
		MoveGenerator *parent)
{
    return new PlaceMoveGenerator(this, b, mvnum, player);
}

int PlaceGame::Validate(const Board *b, int player, Move *m) const
{
    int r = ((PlaceMove*)m)->Row();
    int c = ((PlaceMove*)m)->Col();
    return (b->OnBoard(r, c) && b->IsEmpty(r, c));
}

void PlaceGame::ApplyMove(Board *b, Move *m, int mvnum, int player)
{
    int r = ((PlaceMove*)m)->Row();
    int c = ((PlaceMove*)m)->Col();
    if (r>=0 && r<b->Rows() && c>=0 && c<=b->Cols())
        b->SetOwner(r, c, player);
}

Move *PlaceGame::TranslateMove(char *buf)
{
    int r, c, cnt;
    cnt = sscanf(buf, "%d %d", &r, &c);
    if (cnt != 2) return 0;
    PlaceMove *move = new PlaceMove(r, c);
    if (!Validate(board, nextplayer, move))
    {
	delete move;
	return 0;
    }
    return move;
}

void PlaceGame::Undo(Move *m, int player)
{
//    int r = ((PlaceMove*)m)->Row();
//    int c = ((PlaceMove*)m)->Col();
//    board->SetEmpty(r, c);
}

int PlaceGame::GameOver(unsigned *flags)
{
    return MultiPlayerGame::GameOver(flags);
}

int PlaceGame::GameOver(const Board *b, int player, unsigned *flags)
{
    return MultiPlayerGame::GameOver(b, player, flags);
}

PlaceGame::~PlaceGame()
{
}

int PlaceGame::IsSelected(int r, int c) const
{
    return (r == lastr && c == lastc);
}

void PlaceGame::HandleBoardSelect(int r, int c)
{
    if (board->IsEmpty(r, c))
    {
        lastr = r;
        lastc = c;
    }
}

int PlaceGame::GetBoardSelect(char *buf)
{
    if (lastr >= 0)
    {
	sprintf(buf, "%d %d", lastr, lastc);
	lastr = lastc = -1;
	return 0;
    }
    return -1;
}

//----------------------------------------------------------------------

Move *BoardMoveGame::NewMove(Move *m)
{
    return new BoardMove(m);
}

void BoardMoveGame::ApplyMove(Board *b, Move *m, int mvnum, int player)
{
    int fr = ((BoardMove*)m)->FromRow();
    int fc = ((BoardMove*)m)->FromCol();
    int tr = ((BoardMove*)m)->ToRow();
    int tc = ((BoardMove*)m)->ToCol();
    if (fr>=0 && fr<b->Rows() && fc>=0 && fc<=b->Cols())
        b->SetEmpty(fr, fc);
    if (tr>=0 && tr<b->Rows() && tc>=0 && tc<=b->Cols())
        b->SetOwner(tr, tc, player);
}

Move *BoardMoveGame::TranslateMove(char *buf)
{
    int fr, fc, tr, tc, cnt;
    cnt = sscanf(buf, "%d%d%d%d", &fr, &fc, &tr, &tc);
    if (cnt != 4) return 0;
    int oo;
    if (board->IsEmpty(tr, tc)) oo = -1;
    else oo = board->Owner(tr, tc);
    BoardMove *move = new BoardMove(fr, fc, tr, tc, oo);
    if (!Validate(board, nextplayer, move))
    {
	delete move;
	return 0;
    }
    return move;
}

void BoardMoveGame::Undo(Move *m, int player)
{
//    int fr = ((BoardMove*)m)->FromRow();
//    int fc = ((BoardMove*)m)->FromCol();
//    int tr = ((BoardMove*)m)->ToRow();
//    int tc = ((BoardMove*)m)->ToCol();
//    int oo = ((BoardMove*)m)->OldOwner();
//    board->SetOwner(fr, fc, player);
//    if (oo >= 0)
//	board->SetOwner(tr, tc, oo);
//    else
//	board->SetEmpty(tr, tc);
}

int BoardMoveGame::GameOver(unsigned *flags)
{
    return MultiPlayerGame::GameOver(flags);
}

int BoardMoveGame::GameOver(const Board *b, int player, unsigned *flags)
{
    return MultiPlayerGame::GameOver(b, player, flags);
}

BoardMoveGame::~BoardMoveGame()
{
}

int BoardMoveGame::IsSelected(int r, int c) const
{
    return (r == lastfr && c == lastfc) ||
	   (r == lasttr && c == lasttc);
}

void BoardMoveGame::HandleBoardSelect(int r, int c)
{
    if (!board->IsEmpty(r, c) && board->Owner(r, c) == nextplayer)
    {
        lastfr = r;
        lastfc = c;
	lasttr = lasttc = -1;
    }
    else
    {
        if (lastfr >= 0)
	{
            lasttr = r;
            lasttc = c;
	}
    }
}

int BoardMoveGame::GetBoardSelect(char *buf)
{
    if (lasttr >= 0)
    {
	sprintf(buf, "%d %d %d %d", lastfr, lastfc, lasttr, lasttc);
	lastfr = lastfc = lasttr = lasttc = -1;
	return 0;
    }
    return -1;
}

//---------------------------------------------------------------------
// Pawn games

int PawnGame::CheckPawnMove(const Board *bd, int player, int fr, int fc, 
				int tr, int tc) const
{
    int rr = bd->Rows(), cc = bd->Cols();
    if (fr < 0 || fr >= rr || fc < 0 || fc >= cc)
	return 0;
    if (tr < 0 || tr >= rr || tc < 0 || tc >= cc)
	return 0;
    if (bd->Owner(fr, fc) != player) 
	return 0;
    if (player==0)
    {
	if ((fr-tr) != 1) return 0;
    }
    else
    {
	if ((tr-fr) != 1) return 0;
    }
    if (bd->IsEmpty(tr, tc))
    {
        if (fc != tc) return 0;
    }
    else if (fc == tc || bd->Owner(tr, tc) == player)
	return 0;
    else
    {
	// Validate a take move
	if (fc==0)
	{
	    if (tc!=1) return 0;
	}
	else if (fc==(cc-1))
	{
	    if (tc!=(cc-2)) return 0;
	}
	else if (tc != (fc-1) && tc != (fc+1))
	    return 0;
    }
    return 1;
}

int PawnGame::Validate(const Board *bd, int player, Move *m) const
{
    return CheckPawnMove(bd, player, ((BoardMove *)m)->FromRow(),
			 ((BoardMove *)m)->FromCol(),
			 ((BoardMove *)m)->ToRow(),
			 ((BoardMove *)m)->ToCol());
}

int PawnGame::LocatePawn(const Board *b, int player, int flags, int &r, int &c)
{
    for (r=0; r < b->Rows(); r++)
    {
	for (c=0; c < b->Cols(); c++)
	{
	    if (!b->IsEmpty(r, c) && b->Owner(r, c) == player)
	    {
		int off = (player==0)? (-1) : 1;
		if ((flags & CANADVANCE)!=0 && CheckPawnMove(b, player, r, c, r+off, c))
		    return 0;
		if ((flags & CANTAKE)!=0 &&
		    (CheckPawnMove(b,player, r, c, r+off, c-1)
		  || CheckPawnMove(b,player, r, c, r+off, c+1)))
			return 0;
	    }
	}
    }
    return -1;
}


