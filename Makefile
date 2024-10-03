INC=-I./libchess
SRC=main.c ./libchess/chess.c

JSONSRC= ./minijson/minijson.c
JSONINC= -I./minijson


TEST1SRC = ./test/minijson_test.c
BEAR=bear --

all: build
	
compile_commands.json:
	$(BEAR)  make build

run:build
	./example
test:build
	./test1

libminijson.a:$(JSONSRC)
	gcc -c $(JSONSRC) -o minijson.o
	ar rcs libminijson.a  minijson.o

build:test1 example

example:
	gcc -g $(SRC) $(INC) -o example

test1:libminijson.a
	gcc -g $(TEST1SRC)  $(JSONINC) -L. -lminijson -o test1

clean:
	rm -f test1 *.o example libminijson.a compile_commands.json
