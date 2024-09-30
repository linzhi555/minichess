#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "chess.h"

int main() {
    GameState game;
    InitGame(&game);
    char buffer[100];

    Response res = Success;
    while (1) {
        system("clear");
        Game_debug(&game);
        printf("%s\n", Response_tostr(res));
        printf(">>> \n");

        ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
            perror("read");
            return 1;
        }
        buffer[bytes_read] = '\0';
        res = Game_exec(&game, buffer);
    }
}
