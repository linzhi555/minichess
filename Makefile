INC=-I./libchess
SRC=main.c ./libchess/chess.c

run:
	@ gcc -g $(SRC) $(INC) -o example
	@ ./example

