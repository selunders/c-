BIN = c-
CC = g++

SRCS = $(BIN).y  $(BIN).l util.cpp
HDRS = scanType.h globals.hpp util.hpp
OBJS = lex.yy.o $(BIN).tab.o util.o
CFLAGS = -std=c++11 -g


$(BIN) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN)

lex.yy.c : $(BIN).l $(BIN).tab.h $(HDRS)
	flex $(BIN).l

$(BIN).tab.h $(BIN).tab.c : $(BIN).y
	bison -v -t -d $(BIN).y

util.o :
	$(CC) $(CFLAGS) -c util.cpp -o util.o

clean :
	rm -f *~ $(OBJS) $(BIN) lex.yy.c $(BIN).tab.h $(BIN).tab.c $(BIN).output

tar : $(HDRS) $(SRCS) makefile
	tar -cvf $(BIN).tar $(HDRS) $(SRCS) $(DOCS) makefile
