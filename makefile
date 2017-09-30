CC=bcc
MODEL=l
INC=..\include
CFLAGS=-v -ls -DDEBUG -O -I$(INC) -m$(MODEL) -DDRAWTREE
LFLAGS=-v -ls -m$(MODEL)
GAMELIB=game$(MODEL).lib

lib: $(GAMELIB)

all: $(GAMELIB)

$(GAMELIB): game.obj gridgame.obj movegame.obj
	tlib $(GAMELIB) +-game.obj +-gridgame.obj +-movegame.obj

game.obj: game.cpp $(INC)\game.h $(INC)\gridgame.h
	$(CC) -c $(CFLAGS) game.cpp

gridgame.obj: gridgame.cpp $(INC)\gridgame.h
	$(CC) -c $(CFLAGS) gridgame.cpp

movegame.obj: movegame.cpp $(INC)\movegame.h
	$(CC) -c $(CFLAGS) movegame.cpp

$(INC)\movegame.h: $(INC)\gridgame.h
	touch $(INC)\movegame.h

$(INC)\gridgame.h: $(INC)\game.h
	touch $(INC)\gridgame.h

