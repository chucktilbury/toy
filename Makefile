
HIDE	=	@
TARGET	=	toy
DEPS	=	$(TARGET).deps
CC 	= 	clang
OBJS	=	parser.o \
		scanner.o \
		main.o \
		errors.o \
		memory.o \
		pointer_list.o

DEBUG	=	-g
OPT 	= 	$(DEBUG) -std=c11 -Wall -Wextra -Wpedantic -pedantic

all: $(TARGET) Makefile

%.o: %.c
	@echo "build $@"
	$(HIDE)$(CC) -c $(OPT) $< -o $@

$(DEPS): $(OBJS:%.o=%.c)
	@echo "make depends"
	$(HIDE)$(CC) -MM $^ > $(DEPS)

$(TARGET): $(OBJS) $(DEPS)
	@echo "make $(TARGET)"
	$(HIDE)$(CC) $(OPT) -o $@ $(OBJS)

scanner.h scanner.c: scanner.l
	@echo "build scanner.l"
	$(HIDE)flex scanner.l

parser.h parser.c: parser.y
	@echo "build parser.y"
	$(HIDE)bison parser.y

scanner.o: scanner.c scanner.h
	@echo "build $@"
	$(HIDE)$(CC) -c -g -std=c11 -Wno-parentheses-equality \
		-Wno-implicit-function-declaration $< -o $@

include $(DEPS)

clean:
	@echo "clean"
	@rm -f scanner.c scanner.h parser.c parser.h parser.output \
		$(TARGET) $(OBJS) $(DEPS)
