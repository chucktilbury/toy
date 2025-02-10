
HIDE	=	@
TARGET	=	toy
DEPS	=	$(TARGET).deps
CC 	= 	clang
OBJS	=	tokens.o \
		fileio.o \
		errors.o \
		memory.o \
		hash.o \
		ast.o \
		ast_tables.o \
		pointer_list.o \
		string_buffer.o \
		trace.o
#  		symtab.o
#		sym_reference.o


GEN	=	parser.o \
		scanner.o

SPEC	=	main.o

ALL_OBJ	=	$(OBJS) $(GEN) $(SPEC)

DEBUG	=	-g
OPT 	= 	$(DEBUG) -std=c11 -Wall -Wextra -Wpedantic -pedantic

FORMAT	=	$(OBJS:%.o=%.c) $(OBJS:%.o=%.h)

all: $(TARGET) Makefile

%.o: %.c
	@echo "build $@"
	$(HIDE)$(CC) -c $(OPT) $< -o $@

$(DEPS): $(ALL_OBJ:%.o=%.c)
	@echo "make depends"
	$(HIDE)$(CC) -MM $^ > $(DEPS)

$(TARGET): $(ALL_OBJ) $(DEPS)
	@echo "make $(TARGET)"
	$(HIDE)$(CC) $(OPT) -o $@ $(ALL_OBJ)

scanner.h scanner.c: scanner.l
	@echo "build scanner.l"
	$(HIDE)flex -i scanner.l

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

format:
	@echo "format code"
	@clang-format -i $(FORMAT)

remake: clean all
