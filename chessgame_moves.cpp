#include "chessgame.h"
#include <cmath>

bool ChessGame::isPathClear(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol) const
{
    int rowDirection = 0;
    int colDirection = 0;

    if (toRow > fromRow)
    {
        rowDirection = 1;
    }
    else if (toRow < fromRow)
    {
        rowDirection = -1;
    }

    if (toCol > fromCol)
    {
        colDirection = 1;
    }
    else if (toCol < fromCol)
    {
        colDirection = -1;
    }

    int row = fromRow + rowDirection;
    int col = fromCol + colDirection;

    while (row != toRow ||
           col != toCol)
    {
        if (!board[row][col].isEmpty())
        {
            return false;
        }

        row += rowDirection;
        col += colDirection;
    }

    return true;
}


// =====================================================
// PAWN MOVEMENT
// =====================================================
bool ChessGame::isPawnMoveValid(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol) const
{
    Piece pawn = board[fromRow][fromCol];

    int direction;

    if (pawn.color == PieceColor::White)
    {
        direction = -1;
    }
    else
    {
        direction = 1;
    }


    // ONE SQUARE FORWARD

    if (toCol == fromCol &&
        toRow == fromRow + direction &&
        board[toRow][toCol].isEmpty())
    {
        return true;
    }


    // TWO SQUARES FROM STARTING POSITION

    int startingRow;

    if (pawn.color == PieceColor::White)
    {
        startingRow = 6;
    }
    else
    {
        startingRow = 1;
    }

    if (fromRow == startingRow &&
        toCol == fromCol &&
        toRow == fromRow + 2 * direction &&
        board[fromRow + direction][fromCol].isEmpty() &&
        board[toRow][toCol].isEmpty())
    {
        return true;
    }


    // DIAGONAL CAPTURE

    if (std::abs(toCol - fromCol) == 1 &&
        toRow == fromRow + direction &&
        !board[toRow][toCol].isEmpty() &&
        board[toRow][toCol].color != pawn.color)
    {
        return true;
    }

    return false;
}


// =====================================================
// ROOK
// =====================================================
bool ChessGame::isRookMoveValid(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol) const
{
    if (fromRow != toRow &&
        fromCol != toCol)
    {
        return false;
    }

    return isPathClear(
        fromRow,
        fromCol,
        toRow,
        toCol);
}


// =====================================================
// BISHOP
// =====================================================
bool ChessGame::isBishopMoveValid(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol) const
{
    int rowDifference =
        std::abs(toRow - fromRow);

    int colDifference =
        std::abs(toCol - fromCol);

    if (rowDifference != colDifference)
    {
        return false;
    }

    return isPathClear(
        fromRow,
        fromCol,
        toRow,
        toCol);
}


// =====================================================
// KNIGHT
// =====================================================
bool ChessGame::isKnightMoveValid(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol) const
{
    int rowDifference =
        std::abs(toRow - fromRow);

    int colDifference =
        std::abs(toCol - fromCol);

    return
        (rowDifference == 2 &&
         colDifference == 1) ||
        (rowDifference == 1 &&
         colDifference == 2);
}


// =====================================================
// QUEEN
// =====================================================
bool ChessGame::isQueenMoveValid(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol) const
{
    bool straight =
        (fromRow == toRow ||
         fromCol == toCol);

    bool diagonal =
        std::abs(toRow - fromRow) ==
        std::abs(toCol - fromCol);

    if (!straight && !diagonal)
    {
        return false;
    }

    return isPathClear(
        fromRow,
        fromCol,
        toRow,
        toCol);
}


// =====================================================
// KING
// =====================================================
bool ChessGame::isKingMoveValid(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol) const
{
    int rowDifference =
        std::abs(toRow - fromRow);

    int colDifference =
        std::abs(toCol - fromCol);

    return rowDifference <= 1 &&
           colDifference <= 1;
}


// =====================================================
// BASIC PIECE MOVEMENT
// =====================================================
bool ChessGame::isValidMoveForPiece(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol) const
{
    if (!isInsideBoard(fromRow, fromCol) ||
        !isInsideBoard(toRow, toCol))
    {
        return false;
    }

    if (fromRow == toRow &&
        fromCol == toCol)
    {
        return false;
    }

    Piece source =
        board[fromRow][fromCol];

    Piece destination =
        board[toRow][toCol];


    if (source.isEmpty())
    {
        return false;
    }


    // A piece cannot capture its own piece.

    if (!destination.isEmpty() &&
        destination.color == source.color)
    {
        return false;
    }


    // IMPORTANT:
    // A king can NEVER be captured.
    //
    // Checkmate ends the game instead.

    if (!destination.isEmpty() &&
        destination.type == PieceType::King)
    {
        return false;
    }


    switch (source.type)
    {
    case PieceType::Pawn:

        return isPawnMoveValid(
            fromRow,
            fromCol,
            toRow,
            toCol);


    case PieceType::Rook:

        return isRookMoveValid(
            fromRow,
            fromCol,
            toRow,
            toCol);


    case PieceType::Knight:

        return isKnightMoveValid(
            fromRow,
            fromCol,
            toRow,
            toCol);


    case PieceType::Bishop:

        return isBishopMoveValid(
            fromRow,
            fromCol,
            toRow,
            toCol);


    case PieceType::Queen:

        return isQueenMoveValid(
            fromRow,
            fromCol,
            toRow,
            toCol);


    case PieceType::King:

        return isKingMoveValid(
            fromRow,
            fromCol,
            toRow,
            toCol);


    default:

        return false;
    }
}


// =====================================================
// FIND KING
// =====================================================
