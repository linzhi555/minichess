#include "chess.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Elem* findElem(GameState* game, Pos p);
// static Piece* findPiece(GameState* game, Pos p);

char piece_simp_names[] = {
    [King] = 'K', [Knight] = 'N', [Queen] = 'Q', [Rook] = 'R', [Bishop] = 'B', [Pawn] = 'P',
};

typedef Response (*PieceRule)(GameState* game, Step* step);

bool Pos_hasAbsDiff(Pos p1, Pos p2, int xdiff, int ydiff) {
    if (abs(p2.x - p1.x) != xdiff) {
        return false;
    }
    if (abs(p2.y - p1.y) != ydiff) {
        return false;
    }
    return true;
}

// 1 means y postive director -1 mean negative direction, 0 means same
int Pos_yDirect(Pos from, Pos to) {
    if (to.y - from.y == 0) {
        return 0;
    }
    return (to.y - from.y) / abs(to.y - from.y);
}

int Pos_xAbsDiff(Pos from, Pos to) {
    return abs(to.x - from.x);
}

int Pos_yAbsDiff(Pos from, Pos to) {
    return abs(to.y - from.y);
}

static Response kingRule(GameState* game, Step* step);
static Response queenRule(GameState* game, Step* step);
static Response rookRule(GameState* game, Step* step);
static Response bishopRule(GameState* game, Step* step);
static Response knightRule(GameState* game, Step* step);
static Response pawnRule(GameState* game, Step* step);

static PieceRule PRuleTable[] = {
    [King] = kingRule,     [Queen] = queenRule,   [Rook] = rookRule,
    [Bishop] = bishopRule, [Knight] = knightRule, [Pawn] = pawnRule,
};

static Response kingRule(GameState* game, Step* step) {
    if (Pos_hasAbsDiff(step->to, step->from, 1, 1)) return Success;
    if (Pos_hasAbsDiff(step->to, step->from, 1, 0)) return Success;
    if (Pos_hasAbsDiff(step->to, step->from, 0, 1)) return Success;
    return ErrKingMove;
}

static Response rookRule(GameState* game, Step* step) {
    if (Pos_xAbsDiff(step->from, step->to) * Pos_yAbsDiff(step->from, step->to) != 0) {
        return ErrRookMove;
    }

    return Success;
}

static Response bishopRule(GameState* game, Step* step) {
    if (Pos_xAbsDiff(step->from, step->to) != Pos_yAbsDiff(step->from, step->to)) {
        return ErrBishopMove;
    }
    return Success;
}

static Response queenRule(GameState* game, Step* step) {
    Response res = bishopRule(game, step);
    if (res == Success) return Success;

    res = rookRule(game, step);
    if (res == Success) return Success;

    return ErrQueenMove;
}

static Response knightRule(GameState* game, Step* step) {
    if (Pos_hasAbsDiff(step->to, step->from, 2, 1)) return Success;
    if (Pos_hasAbsDiff(step->to, step->from, 1, 2)) return Success;

    return ErrKnightMove;
}

static Response pawnRule(GameState* game, Step* step) {
    Elem* temp = findElem(game, step->from);

    if (Pos_yDirect(step->from, step->to) != (temp->team == White ? 1 : -1)) {
        return ErrPawnMove;
    }

    if (Pos_hasAbsDiff(step->to, step->from, 0, 1)) return Success;

    if (Pos_hasAbsDiff(step->to, step->from, 0, 2)) return Success;

    return ErrPawnMove;
}

const char* Response_tostr(Response r) {
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

static void Game_copy(GameState* dist, const GameState* src) {
    memcpy(dist, src, sizeof(GameState));
}

static int Game_doStep(GameState* game, const Step* step) {
    game->turn = game->turn == White ? Black : White;
    game->stepNum++;

    Elem* from = findElem(game, step->from);
    Elem* to = findElem(game, step->to);

    memcpy(to, from, sizeof(Elem));
    from->isEmpty = true;

    return 0;
}

static bool Game_isCheckMate(GameState* game, Team team) {
    return false;
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
    PieceRule rule = PRuleTable[step.p];
    Response res = rule(game, &step);
    if (res != Success) return res;

    GameState newState;
    Game_copy(&newState, game);
    Game_doStep(&newState, &step);
    if (Game_isCheckMate(&newState, game->turn)) return ErrSucide;

    // finally all check is donw ,we  can change the game state now
    Game_copy(game, &newState);
    return Success;
}
