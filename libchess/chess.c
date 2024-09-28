#include "chess.h"
#include <stdbool.h>
#include <stdio.h>

char piece_simp_names[] = {
    [King] = 'K', [Knight] = 'N', [Queen] = 'Q', [Rook] = 'R', [Bishop] = 'B', [Pawn] = 'P',
};

void _initPiece(GameState* game, int x, int y, Piece p, Team t) {
    Elem* ptr = &game->board[x + 8 * y];
    ptr->isEmpty = false;
    ptr->team = t;
    ptr->piece = p;
}

void InitGame(GameState* game) {
    game->round = 0;
    game->isFinished = false;
    game->turn = White;

    for (int i = 0; i < 64; i++) {
        Elem temp = {
            .isEmpty = true,
        };
        game->board[i] = temp;
    }

    _initPiece(game, 4, 0, King, White);
    _initPiece(game, 4, 7, King, Black);

    _initPiece(game, 3, 0, Queen, White);
    _initPiece(game, 3, 7, Queen, Black);

    for (int i = 0; i < 8; i += 7) {
        _initPiece(game, i, 0, Rook, White);
        _initPiece(game, i, 7, Rook, Black);
    }

    for (int i = 1; i < 7; i += 5) {
        _initPiece(game, i, 0, Knight, White);
        _initPiece(game, i, 7, Knight, Black);
    }

    for (int i = 2; i < 6; i += 3) {
        _initPiece(game, i, 0, Bishop, White);
        _initPiece(game, i, 7, Bishop, Black);
    }

    for (int i = 0; i < 8; i++) {
        _initPiece(game, i, 1, Pawn, White);
        _initPiece(game, i, 6, Pawn, Black);
    }
}

void Game_debug(GameState* game) {
    printf("  A B C D E F G H\n");
    for (int j = 7; j >= 0; j--) {
        printf("%c", '1' + j);
        for (int i = 0; i < 8; i++) {
            printf("[");
            Elem* ptr = &game->board[i + j * 8];
            if (ptr->isEmpty) {
                printf((i + j) % 2 == 0 ? "#" : " ");
            } else {
                char simpname = piece_simp_names[ptr->piece];
                printf("%s", ptr->team == White ? "\033[0;32m" : "\033[0;31m");
                printf("%c", simpname);
                printf("\033[0m");
            }
        }
        printf("]");
        printf("%c\n", '1' + j);
    }
    printf("  A B C D E F G H\n");
}
