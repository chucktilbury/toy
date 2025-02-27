
CHIDE	=	@
EHIDE	=	@
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
		context.o \
		trace.o \
		symbol.o \
		symbol_table.o \
		sym_reference.o \
		func_reference.o \
		type_table.o \
		dump_symbols.o \
		emit_code.o


GEN	=	parser.o \
		scanner.o

SPEC	=	main.o

ALL_OBJ	=	$(OBJS) $(GEN) $(SPEC)

DEBUG	=	-g
OPT 	= 	$(DEBUG) -std=c11 -Wall -Wextra -Wpedantic -pedantic

FORMAT	=	$(OBJS:%.o=%.c) $(OBJS:%.o=%.h)

all: $(TARGET) Makefile

%.o: %.c
	$(EHIDE)echo "$(CC) $@"
	$(CHIDE)$(CC) -c $(OPT) $< -o $@

$(DEPS): $(ALL_OBJ:%.o=%.c)
	$(EHIDE)echo "$(CC) $(DEPS)"
	$(CHIDE)$(CC) -MM $^ > $(DEPS)

$(TARGET): $(ALL_OBJ) $(DEPS)
	$(EHIDE)echo "$(CC) $(TARGET)"
	$(CHIDE)$(CC) $(OPT) -o $@ $(ALL_OBJ)

scanner.h scanner.c: scanner.l
	$(EHIDE)echo "flex scanner.l"
	$(CHIDE)flex -i scanner.l

parser.h parser.c: parser.y
	$(EHIDE)echo "bison parser.y"
	$(CHIDE)bison parser.y

scanner.o: scanner.c scanner.h
	$(EHIDE)echo "$(CC) $@"
	$(CHIDE)$(CC) -c -g -std=c11 -Wno-parentheses-equality \
		-Wno-implicit-function-declaration $< -o $@

include $(DEPS)

clean:
	$(EHIDE)echo "clean"
	$(CHIDE)rm -f scanner.c scanner.h parser.c parser.h parser.output \
		$(TARGET) $(OBJS) $(DEPS)

format:
	$(EHIDE)echo "format code"
	$(CHIDE)clang-format -i $(FORMAT)

remake: clean
	$(EHIDE)echo "remake all"
	$(CHIDE)make -C . -j12 $(TARGET)
