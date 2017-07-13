################################################################################
#
#	Make file for simple objects
#
################################################################################
PLATNAME := $(shell uname)

OBJS	=	scanner.o \
		parse.o \
		errors.o

OBJS1	=	main.o

TARGET	=	toy

CARGS	=	-Wall
DEBUG	=	-g -DTRACE
HEADERS	=	$(OBJS:.o=.h)

ifeq ($(PLATNAME), Linux)
	LIBS	=	-ll -ly
else ifeq ($(PLATNAME), MSYS_NT-6.1)
	LIBS	=	-L/c/MinGW/msys/1.0/lib -lfl -ly
endif

.c.o:
	gcc $(CARGS) $(DEBUG) -c $< -o $@

all: $(TARGET)

$(TARGET): $(OBJS1) $(OBJS) $(HEADERS)
	gcc $(CARGS) $(DEBUG) $(OBJS1) $(OBJS) -o $(TARGET) $(LIBS)

scanner.c: scanner.flex parser.h scanner.h
	flex -o scanner.c scanner.flex

main.o: main.c $(OBJS)
parse.o: parse.c errors.h
errors.o: errors.c errors.h

clean:
	-rm -f scanner.c parse.h $(OBJS1) $(OBJS) $(TARGET)
