
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

    static int moveLoss(const ChessGame &game, const ChessGame &played);
    static int positionValue(const ChessGame &g) { return evaluate(g); }
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