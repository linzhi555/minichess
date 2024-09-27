#include <stdio.h>
#include "chess.h"

int main() {
    printf("hello world\n");
    GameState game;
    InitGame(&game);
    Game_debug(&game);
}
