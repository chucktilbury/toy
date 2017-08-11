################################################################################
#
#	Make file for simple objects
#
################################################################################
PLATNAME := $(shell uname)

OBJS	=	parser.o \
			filestack.o \
			main.o \
			symtab.o \
			token.o \
			scanner.o

TARGET	=	toi

CARGS	=	-Wall
DEBUG	=	-g -DTRACE -DLOGGING
HEADERS	=	errors.h logger.h

.cpp.o:
	g++ $(CARGS) $(DEBUG) -c $< -o $@

all: $(TARGET)

$(TARGET): $(OBJS) $(HEADERS)
	g++ $(CARGS) $(DEBUG) $(OBJS) -o $(TARGET) $(LIBS)

main.o: main.cpp $(HEADERS)
parser.o: parser.cpp parser.h $(HEADERS)
filestack.o: filestack.cpp filestack.h $(HEADERS)
scanner.o: scanner.cpp scanner.h $(HEADERS)
symtab.o: symtab.cpp token.h $(HEADERS)
token.o: token.cpp token.h $(HEADERS)

clean:
	-rm -f $(OBJS) $(TARGET)
