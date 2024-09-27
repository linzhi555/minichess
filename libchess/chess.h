#pragma once
#include <stdbool.h>

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
    int round;
    bool isFinished;
    Team winner;
    Team turn;
    Elem board[64];
} GameState;

void InitGame(GameState* game);
void Game_debug(GameState* game);
