OUTPUT_BIN = plz
OBJECTS = main.o parser.o lexer.o token.o
CC = clang
CFLAGS = -std=c11 -c -O0 -Wall


$(OUTPUT_BIN) : $(OBJECTS)
	$(CC) -o $(OUTPUT_BIN) $(OBJECTS)

main.o: src/main.c
	$(CC) $(CFLAGS) src/main.c

parser.o: src/parser.c src/parser.h
	$(CC) $(CFLAGS) src/parser.c

lexer.o: src/lexer.c src/lexer.h
	$(CC) $(CFLAGS) src/lexer.c

token.o: src/token.c src/token.h
	$(CC) $(CFLAGS) src/token.c


.PHONY: clean_obj clean_all
clean_obj:
	rm -f *.o

clean_all:
	rm -f *.o $(OUTPUT_BIN)