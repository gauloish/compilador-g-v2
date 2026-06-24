COMPILER = gcc
NAME = g-v2
OBJECTS_DIR = objects
INCLUDE = -I"include/"
FILES = memory            \
		syntax_tree       \
		symbol_table      \
		semantic_analysis \
		code_generation   \
		lexer             \
		parser            \
		main

SOURCES = $(FILES:%=source/%.c)
OBJECTS = $(FILES:%=objects/%.o)

all: main

main: source/parser.c source/lexer.c $(OBJECTS)
	$(COMPILER) $(INCLUDE) -o $(NAME) $(OBJECTS)

objects/%.o: source/%.c | $(OBJECTS_DIR)
	$(COMPILER) $(INCLUDE) -c -o $@ $<

source/parser.c: source/parser.y
	bison --header=include/tokens.h -o source/parser.c source/parser.y

source/lexer.c: source/lexer.l
	flex -o source/lexer.c source/lexer.l

$(OBJECTS_DIR):
	mkdir -p $(OBJECTS_DIR)

clean:
	rm g-v1 g-v1.exe include/tokens.h objects/*.o source/lexer.c source/parser.c