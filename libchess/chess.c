#include "chess.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const Elem* findElem(const GameState* game, Vec2 p);

char piece_simp_names[] = {
    [King] = 'K', [Knight] = 'N', [Queen] = 'Q', [Rook] = 'R', [Bishop] = 'B', [Pawn] = 'P',
};

typedef Response (*PieceRule)(const GameState* game, Step* step);

bool Vec2_hasAbsDiff(Vec2 p1, Vec2 p2, int xdiff, int ydiff) {
    if (abs(p2.x - p1.x) != xdiff) {
        return false;
    }
    if (abs(p2.y - p1.y) != ydiff) {
        return false;
    }
    return true;
}

// 1 means y postive director -1 mean negative direction, 0 means same
int Vec2_yDirect(Vec2 from, Vec2 to) {
    if (to.y - from.y == 0) {
        return 0;
    }
    return (to.y - from.y) / abs(to.y - from.y);
}

int Vec2_xDirect(Vec2 from, Vec2 to) {
    if (to.x - from.x == 0) {
        return 0;
    }
    return (to.x - from.x) / abs(to.x - from.x);
}

int Vec2_xAbsDiff(Vec2 from, Vec2 to) {
    return abs(to.x - from.x);
}

bool Vec2_isSame(Vec2 v1, Vec2 v2) {
    return v1.x == v2.x && v1.y == v2.y;
}

Vec2 Vec2_add(Vec2 v1, Vec2 v2) {
    Vec2 res;
    res.x = v1.x + v2.x;
    res.y = v1.y + v2.y;
    return res;
}

int Vec2_yAbsDiff(Vec2 from, Vec2 to) {
    return abs(to.y - from.y);
}

// the bishop-like move
bool Vec2_isSlash(Vec2 from, Vec2 to, Vec2* direct) {
    if (Vec2_xAbsDiff(from, to) == Vec2_yAbsDiff(from, to)) {
        direct->x = Vec2_xDirect(from, to);
        direct->y = Vec2_yDirect(from, to);
        return true;
    }

    return false;
}

// the rook-like move
bool Vec2_isStraight(Vec2 from, Vec2 to, Vec2* direct) {
    if (Vec2_xAbsDiff(from, to) * Vec2_yAbsDiff(from, to) == 0) {
        direct->x = Vec2_xDirect(from, to);
        direct->y = Vec2_yDirect(from, to);
        return true;
    }

    return false;
}

int Vec2_betweens(Vec2 from, Vec2 to, Vec2 betweens[], int n) {
    Vec2 direct;
    if (!Vec2_isSlash(from, to, &direct) && !Vec2_isStraight(from, to, &direct)) {
        return 0;
    }

    int res = 0;
    for (Vec2 cur = Vec2_add(from, direct); !Vec2_isSame(cur, to) && res < n; cur = Vec2_add(cur, direct)) {
        betweens[res] = cur;
        res++;
    }

    return res;
}

static Response kingRule(const GameState* game, Step* step);
static Response queenRule(const GameState* game, Step* step);
static Response rookRule(const GameState* game, Step* step);
static Response bishopRule(const GameState* game, Step* step);
static Response knightRule(const GameState* game, Step* step);
static Response pawnRule(const GameState* game, Step* step);

static PieceRule PRuleTable[] = {
    [King] = kingRule,     [Queen] = queenRule,   [Rook] = rookRule,
    [Bishop] = bishopRule, [Knight] = knightRule, [Pawn] = pawnRule,
};

static Response kingRule(const GameState* game, Step* step) {
    if (Vec2_hasAbsDiff(step->to, step->from, 1, 1)) return Success;
    if (Vec2_hasAbsDiff(step->to, step->from, 1, 0)) return Success;
    if (Vec2_hasAbsDiff(step->to, step->from, 0, 1)) return Success;
    return ErrKingMove;
}

static bool _blocked(const GameState* game, Vec2 from, Vec2 to) {
    Vec2 betweens[8];
    int res = Vec2_betweens(from, to, betweens, 8);
    for (int i = 0; i < res; i++) {
        const Elem* elem = findElem(game, betweens[i]);

        if (!elem->isEmpty) {
            return true;
        }
    }

    return false;
}

static Response rookRule(const GameState* game, Step* step) {
    if (Vec2_xAbsDiff(step->from, step->to) * Vec2_yAbsDiff(step->from, step->to) != 0) {
        return ErrRookMove;
    }
    if (_blocked(game, step->from, step->to)) return ErrBlocked;

    return Success;
}

static Response bishopRule(const GameState* game, Step* step) {
    if (Vec2_xAbsDiff(step->from, step->to) != Vec2_yAbsDiff(step->from, step->to)) {
        return ErrBishopMove;
    }

    if (_blocked(game, step->from, step->to)) return ErrBlocked;
    return Success;
}

static Response queenRule(const GameState* game, Step* step) {
    Response res = bishopRule(game, step);
    if (res == Success) return Success;

    res = rookRule(game, step);
    if (res == Success) return Success;

    return ErrQueenMove;
}

static Response knightRule(const GameState* game, Step* step) {
    if (Vec2_hasAbsDiff(step->to, step->from, 2, 1)) return Success;
    if (Vec2_hasAbsDiff(step->to, step->from, 1, 2)) return Success;

    return ErrKnightMove;
}

static Response pawnRule(const GameState* game, Step* step) {
    const Elem* temp = findElem(game, step->from);

    if (Vec2_yDirect(step->from, step->to) != (temp->team == White ? 1 : -1)) {
        return ErrPawnMove;
    }

    if (Vec2_hasAbsDiff(step->to, step->from, 0, 1)) return Success;

    if (Vec2_hasAbsDiff(step->to, step->from, 0, 2)) {
        if (_blocked(game, step->from, step->to)) return ErrBlocked;
        return Success;
    }

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
    game->winner = NoTeam;

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

static int parse_Pos(const char* str, Vec2* from, Vec2* to) {
    if (strlen(str) < 4) {
        return -1;
    }

    if ('a' > str[0] || str[0] > 'h') {
        return 1;
    }
    if ('1' > str[1] || str[1] > '8') {
        return 2;
    }
    if ('a' > str[2] || str[2] > 'h') {
        return 3;
    }
    if ('1' > str[3] || str[3] > '8') {
        return 4;
    }

    from->x = str[0] - 'a';
    from->y = str[1] - '1';
    to->x = str[2] - 'a';
    to->y = str[3] - '1';

    return 0;
}

// return a unmutable reference of a elem
static const Elem* findElem(const GameState* game, Vec2 p) {
    return &game->board[p.x + 8 * p.y];
}

// return a mutable reference of a elem
static Elem* findElemMut(GameState* game, Vec2 p) {
    return &game->board[p.x + 8 * p.y];
}

static void Game_copy(GameState* dist, const GameState* src) {
    memcpy(dist, src, sizeof(GameState));
}

static int Game_doStep(GameState* game, const Step* step) {
    game->turn = game->turn == White ? Black : White;
    game->stepNum++;

    Elem* from = findElemMut(game, step->from);
    Elem* to = findElemMut(game, step->to);

    memcpy(to, from, sizeof(Elem));
    from->isEmpty = true;

    return 0;
}

static bool Game_isCheck(GameState* game, Team team) {
    Vec2 kingPos;
    bool findKing = false;
    for (int i = 0; i < 64; i++) {
        Elem* temp = &game->board[i];
        if (!temp->isEmpty && temp->piece == King && temp->team == team) {
            kingPos.x = i % 8;
            kingPos.y = i / 8;
            findKing = true;
        }
    }
    if (!findKing) return false;

    for (int i = 0; i < 64; i++) {
        Vec2 from;
        Elem* temp = &game->board[i];
        if (!temp->isEmpty && temp->team != team) {
            from.x = i % 8;
            from.y = i / 8;
        } else {
            continue;
        }

        Step step = { .from = from, .to = kingPos };
        PieceRule rule = PRuleTable[temp->piece];
        Response res = rule(game, &step);
        if (res == Success) return true;
    }

    return false;
}

static Response Game_isLegalMove(const GameState* game, Step* step) {
    const Elem* temp = findElem(game, step->from);
    if (temp->isEmpty) return ErrNoPieceThere;
    if (temp->team != game->turn) return ErrNotYourTurn;

    step->p = temp->piece;
    step->turn = temp->team;

    const Elem* toElem = findElem(game, step->to);
    if (!toElem->isEmpty && toElem->team == step->turn) return ErrBlocked;

    PieceRule rule = PRuleTable[step->p];
    Response res = rule(game, step);
    if (res != Success) return res;

    GameState newState;
    Game_copy(&newState, game);
    Game_doStep(&newState, step);

    if (Game_isCheck(&newState, game->turn)) return ErrSucide;

    return Success;
}

static int possibleMoves(const GameState* game, Vec2 pos, Step result[], int maxLen) {
    int resultNum = 0;

    const Elem* elem = findElem(game, pos);
    if (elem == NULL && elem->isEmpty == true) return 0;
    PieceRule rule = PRuleTable[elem->piece];

    for (int i = 0; i < 64 && resultNum < maxLen; i++) {
        Vec2 to = { .x = i % 8, .y = i / 8 };
        Step step = { .from = pos, .to = to };
        if (Game_isLegalMove(game, &step) == Success) {
            result[resultNum] = step;
            resultNum++;
        }
    }

    return resultNum;
}

static bool Game_hasLegalMove(const GameState* game) {
    for (int i = 0; i < 64; i++) {
        Vec2 pos = { .x = i % 8, .y = i / 8 };
        const Elem* elem = findElem(game, pos);
        if (!elem->isEmpty && elem->team == game->turn) {
            Step result[64];
            int moveNum = possibleMoves(game, pos, result, 64);
            if (moveNum != 0) return true;
        }
    }
    return false;
}

Response Game_exec(GameState* game, const char* const cmd) {
    Step step;

    int err = parse_Pos(cmd, &step.from, &step.to);
    if (err != 0) return ErrParseCmd;
    Response resp = Game_isLegalMove(game, &step);
    if (resp != Success) {
        return resp;
    }

    // finally all check is done ,we need change the game state
    Game_doStep(game, &step);
    // check if the game is finished
    if (Game_isCheck(game, game->turn)) {
        if (!Game_hasLegalMove(game)) {
            game->isFinished = true;
            game->winner = game->turn;
        }
    } else if (!Game_hasLegalMove(game)) {
        game->isFinished = true;
        game->winner = NoTeam;
    }

    return Success;
}
