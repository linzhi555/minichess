#include "chess.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

char piece_simp_names[] = {
    [King] = 'K', [Knight] = 'N', [Queen] = 'Q', [Rook] = 'R', [Bishop] = 'B', [Pawn] = 'P',
};

const char* Response_to_str(Response r) {
    switch (r) {
    case Success:
        return "Success";
    case ErrParseCmd:
        return "ErrParseCmd";
    case ErrNoPieceThere:
        return "ErrNoPieceThere";
    case ErrWrongPos:
        return "ErrWrongPos";
    case ErrAlreadyFinish:
        return "ErrAlreadyFinish";
    case ErrNotYourTurn:
        return "ErrNotYourTurn";
    case ErrPieceRule:
        return "ErrPieceRule";
    case ErrPawnMove:
        return "ErrPawnMove";
    case ErrKingMove:
        return "ErrKingMove";
    case ErrBishopMove:
        return "ErrBishopMove";
    case ErrKnightMove:
        return "ErrKnightMove";
    case ErrQueenMove:
        return "ErrQueenMove";
    case ErrRookMove:
        return "ErrRookMove";
    case ErrBlocked:
        return "ErrBlocked";
    case ErrKillSame:
        return "ErrKillSame";
    case ErrSucide:
        return "ErrSucide";
    }
    return "Undefined";
}

static void _initPiece(GameState* game, int x, int y, Piece p, Team t) {
    Elem* ptr = &game->board[x + 8 * y];
    ptr->isEmpty = false;
    ptr->team = t;
    ptr->piece = p;
}

void InitGame(GameState* game) {
    game->stepNum = 0;
    game->isFinished = false;
    game->turn = White;

    // init boards

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

static int parse_Pos(const char* str, Pos* from, Pos* to) {
    if (strlen(str) < 4) {
        return -1;
    }

    if ('a' > str[0] || str[0] > 'h') {
        return 1;
    }
    if ('1' > str[1] || str[1] > '7') {
        return 2;
    }
    if ('a' > str[2] || str[2] > 'h') {
        return 3;
    }
    if ('1' > str[3] || str[3] > '7') {
        return 4;
    }

    from->x = str[0] - 'a';
    from->y = str[1] - '1';
    to->x = str[2] - 'a';
    to->y = str[3] - '1';

    return 0;
}

static Elem* findElem(GameState* game, Pos p) {
    return &game->board[p.x + 8 * p.y];
}

Response Game_exec(GameState* game, const char* const cmd) {
    Step step;

    int err = parse_Pos(cmd, &step.from, &step.to);
    if (err != 0) return ErrParseCmd;

    Elem* temp = findElem(game, step.from);
    if (temp->isEmpty) return ErrNoPieceThere;
    if (temp->team != game->turn) return ErrNotYourTurn;
    step.p = temp->piece;
    step.turn = game->turn;

    return Success;
}
