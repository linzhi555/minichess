INC=-I./libchess
SRC=main.c ./libchess/chess.c

JSONSRC= ./minijson/minijson.c
JSONINC= -I./minijson

TEST1SRC = ./test/minijson_test.c
TARGET=./minichess


all: build

compile_commands.json:
	$(BEAR)  make build

run:build
	$(TARGET)

test:build
	./test1

libminijson.a:$(JSONSRC)
	gcc -c $(JSONSRC) -o minijson.o
	ar rcs libminijson.a  minijson.o

build:test1 $(TARGET)

$(TARGET):
	gcc -g $(SRC) $(INC) -o $(TARGET)

test1:libminijson.a
	gcc -g $(TEST1SRC)  $(JSONINC) -L. -lminijson -o test1

clean:
	rm -f test1 *.o $(TARGET) libminijson.a compile_commands.json
