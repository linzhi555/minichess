#pragma once
#include <stdbool.h>

#define MAXSTEPS 1000

typedef enum Response {
    Success = 0,
    ErrParseCmd,
    ErrNoPieceThere,
    ErrWrongPos,
    ErrAlreadyFinish,
    ErrNotYourTurn,
    ErrPieceRule,
    ErrPawnMove,
    ErrKingMove,
    ErrBishopMove,
    ErrKnightMove,
    ErrQueenMove,
    ErrRookMove,
    ErrBlocked,
    ErrKillSame,
    ErrSucide,
} Response;

typedef enum Team {
    White,
    Black,
    NoTeam,
} Team;

typedef enum Piece {
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn,
} Piece;

typedef struct Elem {
    bool isEmpty;
    Team team;
    Piece piece;
} Elem;

typedef struct {
    int x;
    int y;
} Vec2;

typedef struct {
    Vec2 from;
    Vec2 to;
    Piece p;
    Team turn;
    bool isEat;
    Piece died;
    bool isPrompt;
    Piece newP;
    bool isCastle;
    bool isEnpass;
    bool isCheckMate;
} Step;

typedef struct {
    bool isFinished;
    Team winner;
    Team turn;
    Elem board[64];
    int stepNum;
    Step history[MAXSTEPS];
} GameState;

void InitGame(GameState* game);
void Game_debug(GameState* game);
Response Game_exec(GameState* game, const char* const cmd);
const char* Response_tostr(Response r);
