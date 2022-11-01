
BIN = c-
CC = g++

SRCS = $(BIN).y  $(BIN).l util.cpp main.cpp symbolTable.cpp analyze.cpp errorMsg.cpp yyerror.cpp
HDRS = scanType.h globals.hpp util.hpp analyze.hpp symbolTable.hpp errorMsg.hpp colorPrint.hpp yyerror.h
LIBS = io.c-
OBJS = lex.yy.o $(BIN).tab.o util.o main.o analyze.o symbolTable.o errorMsg.o yyerror.o
CFLAGS = -std=c++11 -g


$(BIN) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN)

lex.yy.c : $(BIN).l $(BIN).tab.h $(HDRS)
	flex $(BIN).l

$(BIN).tab.h $(BIN).tab.c : $(BIN).y
	bison -v -t -d $(BIN).y

util.o :
	$(CC) $(CFLAGS) -c util.cpp -o util.o

main.o :
	$(CC) $(CFLAGS) -c main.cpp -o main.o

symbolTable.o :
	$(CC) $(CFLAGS) -c symbolTable.cpp -o symbolTable.o

analyze.o :
	$(CC) $(CFLAGS) -c analyze.cpp -o analyze.o

errorMsg.o :
	$(CC) $(CFLAGS) -c errorMsg.cpp -o errorMsg.o

yyerror.o :
	$(CC) $(CFLAGS) -c yyerror.cpp -o yyerror.o

clean :
	rm -f *~ $(OBJS) $(BIN) lex.yy.c $(BIN).tab.h $(BIN).tab.c $(BIN).output *.gch *.o

tar : $(HDRS) $(SRCS) makefile
	tar -cvf $(BIN).tar $(HDRS) $(SRCS) $(DOCS) $(LIBS) makefile
