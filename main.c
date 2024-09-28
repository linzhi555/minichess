#include <stdio.h>
#include "chess.h"

int main() {
    GameState game;
    InitGame(&game);
    Game_debug(&game);
}
