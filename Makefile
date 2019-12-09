CC=gcc
CFLAGS=-std=c11 -O3 -Wall -Wextra -Werror -pedantic -lm -g
EXEC=main

$(EXEC): src/scanner/scanner.o src/parser/parser.o src/interpreter/interpreter.o src/utils/utils.o src/main.o
	gcc -o $(EXEC) $(addprefix out/, $(notdir $^)) $(CFLAGS)

%.o: %.c
	gcc -o out/$(notdir $@) -c $< $(CFLAGS)

run:
	./$(EXEC)

clean:
	rm -f out/*.o