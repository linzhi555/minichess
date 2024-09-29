#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "chess.h"

int main() {
    GameState game;
    InitGame(&game);
    char buffer[100];
    system("clear");
    Game_debug(&game);
    printf(">>> \n");
    while (1) {
        ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
            perror("read");
            return 1;
        }
        buffer[bytes_read] = '\0';
        system("clear");
        Game_debug(&game);
        Response res = Game_exec(&game, buffer);
        printf("%s\n", Response_to_str(res));
        printf(">>> \n");
        sleep(1);
    }
}
