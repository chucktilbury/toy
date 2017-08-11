################################################################################
#
#	Make file for simple objects
#
################################################################################
PLATNAME := $(shell uname)

OBJS	=	parser.o \
		errors.o

OBJS1	=	main.o

TARGET	=	toi

CARGS	=	-Wall
DEBUG	=	-g -DTRACE -DDEBUGGING
HEADERS	=	$(OBJS:.o=.h)

ifeq ($(PLATNAME), Linux)
	LIBS	=	-ll -ly
else ifeq ($(PLATNAME), MSYS_NT-6.1)
	LIBS	=	-L/c/MinGW/msys/1.0/lib -lfl -ly
endif

.c.o:
	g++ $(CARGS) $(DEBUG) -c $< -o $@

all: $(TARGET)

$(TARGET): $(OBJS1) $(OBJS) $(HEADERS)
	g++ $(CARGS) $(DEBUG) $(OBJS1) $(OBJS) -o $(TARGET) $(LIBS)

scanner.c: scanner.flex parser.h scanner.h
	flex -o scanner.c scanner.flex

scanner.o: scanner.c scanner.h
	gcc $(CARGS) $(DEBUG) -c scanner.o scanner.c

main.o: main.c $(OBJS)
parser.o: parser.c errors.h
errors.o: errors.c errors.h

clean:
	-rm -f scanner.c parse.h $(OBJS1) $(OBJS) $(TARGET)
