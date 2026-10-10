#ifndef MOVE_H
#define MOVE_H

#include "piece.h"

struct Move
{
    int fromRow;
    int fromCol;

    int toRow;
    int toCol;

    Piece movedPiece;
    Piece capturedPiece;

    Move()
    {
        fromRow = -1;
        fromCol = -1;

        toRow = -1;
        toCol = -1;

        movedPiece = Piece();
        capturedPiece = Piece();
    }

    Move(
        int fr,
        int fc,
        int tr,
        int tc,
        Piece moved,
        Piece captured)
    {
        fromRow = fr;
        fromCol = fc;

        toRow = tr;
        toCol = tc;

        movedPiece = moved;
        capturedPiece = captured;
    }
};

#endif