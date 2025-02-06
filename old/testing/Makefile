
SRC	= main.c tokens.c memory.c parser.c scanner.c fileio.c errors.c

all:
	bison parser.y
	flex scanner.l
	gcc -g $(SRC) -o t
