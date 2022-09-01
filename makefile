BIN = c-
CC = g++
CFLAGS = -std=c++11

# DOCS = calcGrammar.pdf
SRCS = $(BIN).y  $(BIN).l
HDRS = scanType.h
OBJS = lex.yy.o $(BIN).tab.o

$(BIN) : $(OBJS)
	$(CC) $(OBJS) -o $(BIN) $(CFLAGS)

lex.yy.cc : $(BIN).l $(BIN).tab.h $(HDR)
	flex -o lex.yy.cc $(BIN).l

$(BIN).tab.h $(BIN).tab.cc : $(BIN).y
	bison -v -t -d $(BIN).y -o $(BIN).tab.cc

clean :
	rm -f *~ $(OBJS) $(BIN) lex.yy.cc $(BIN).tab.h $(BIN).tab.cc $(BIN).output

tar : $(HDR) $(SRCS) makefile
	tar -cvf $(BIN).tar $(HDRS) $(SRCS) $(DOCS) makefile


# BIN = calc
# CC = g++

# DOCS = calcGrammar.pdf
# SRCS = $(BIN).y  $(BIN).l
# HDRS = scanType.h
# OBJS = lex.yy.o $(BIN).tab.o

# $(BIN) : $(OBJS)
# 	$(CC) $(OBJS) -o $(BIN)

# lex.yy.c : $(BIN).l $(BIN).tab.h $(HDR)
# 	flex $(BIN).l

# $(BIN).tab.h $(BIN).tab.c : $(BIN).y
# 	bison -v -t -d $(BIN).y

# clean :
# 	rm -f *~ $(OBJS) $(BIN) lex.yy.c $(BIN).tab.h $(BIN).tab.c $(BIN).output

# tar : $(HDR) $(SRCS) makefile
# 	tar -cvf $(BIN).tar $(HDRS) $(SRCS) $(DOCS) makefile
