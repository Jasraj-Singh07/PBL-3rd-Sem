
#ifndef AI_H
#define AI_H

#include "chessgame.h"
#include "move.h"
#include <QString>

class ChessAI
{
public:
    static bool chooseMove(
        const ChessGame &game,
        const QString &difficulty,
        Move &bestMove
    );

private:
    static void collectMoves(
        const ChessGame &game,
        Move moves[],
        int &count
    );

    static int evaluate(const ChessGame &game);

    static int minimax(
        ChessGame game,
        int depth,
        int alpha,
        int beta
    );

    static int pieceValue(PieceType type);
};

#endif