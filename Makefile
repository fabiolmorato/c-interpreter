CC=gcc
CFLAGS=-std=c11 -O3 -Wall -Wextra -Werror -pedantic -lm
EXEC=main

$(EXEC): src/scanner/scanner.c src/parser/parser.c src/interpreter/interpreter.c src/utils/utils.c src/main.c
	gcc -o $(EXEC) $^ $(CFLAGS)

%.o: %.c
	gcc -o out/$(notdir $@) -c $< $(CFLAGS)

run:
	./$(EXEC)

clean:
	rm -f out/*.o