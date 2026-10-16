#include "chessgame.h"
#include <cmath>

bool ChessGame::findKing(
    PieceColor color,
    int &kingRow,
    int &kingCol) const
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            Piece piece = board[row][col];

            if (!piece.isEmpty() &&
                piece.type == PieceType::King &&
                piece.color == color)
            {
                kingRow = row;
                kingCol = col;

                return true;
            }
        }
    }

    return false;
}


// =====================================================
// CHECK IF A SQUARE IS ATTACKED
// =====================================================
bool ChessGame::isSquareAttacked(
    int row,
    int col,
    PieceColor attackingColor) const
{
    for (int fromRow = 0; fromRow < 8; fromRow++)
    {
        for (int fromCol = 0; fromCol < 8; fromCol++)
        {
            Piece piece =
                board[fromRow][fromCol];

            if (piece.isEmpty())
            {
                continue;
            }

            if (piece.color != attackingColor)
            {
                continue;
            }


            int rowDifference =
                std::abs(row - fromRow);

            int colDifference =
                std::abs(col - fromCol);


            // =========================
            // PAWN ATTACK
            // =========================

            if (piece.type == PieceType::Pawn)
            {
                int direction;

                if (attackingColor == PieceColor::White)
                {
                    direction = -1;
                }
                else
                {
                    direction = 1;
                }

                if (row == fromRow + direction &&
                    colDifference == 1)
                {
                    return true;
                }

                continue;
            }


            // =========================
            // KNIGHT ATTACK
            // =========================

            if (piece.type == PieceType::Knight)
            {
                if ((rowDifference == 2 &&
                     colDifference == 1) ||
                    (rowDifference == 1 &&
                     colDifference == 2))
                {
                    return true;
                }

                continue;
            }


            // =========================
            // KING ATTACK
            // =========================

            if (piece.type == PieceType::King)
            {
                if (rowDifference <= 1 &&
                    colDifference <= 1 &&
                    (rowDifference != 0 ||
                     colDifference != 0))
                {
                    return true;
                }

                continue;
            }


            // =========================
            // ROOK / BISHOP / QUEEN
            // =========================

            bool validDirection = false;

            if (piece.type == PieceType::Rook)
            {
                validDirection =
                    (fromRow == row ||
                     fromCol == col);
            }
            else if (piece.type == PieceType::Bishop)
            {
                validDirection =
                    (rowDifference == colDifference);
            }
            else if (piece.type == PieceType::Queen)
            {
                validDirection =
                    (fromRow == row ||
                     fromCol == col ||
                     rowDifference == colDifference);
            }

            if (!validDirection)
            {
                continue;
            }


            // Check whether anything blocks the attack.

            int rowDirection = 0;
            int colDirection = 0;

            if (row > fromRow)
            {
                rowDirection = 1;
            }
            else if (row < fromRow)
            {
                rowDirection = -1;
            }

            if (col > fromCol)
            {
                colDirection = 1;
            }
            else if (col < fromCol)
            {
                colDirection = -1;
            }

            int checkRow =
                fromRow + rowDirection;

            int checkCol =
                fromCol + colDirection;

            bool blocked = false;

            while (checkRow != row ||
                   checkCol != col)
            {
                if (!board[checkRow][checkCol].isEmpty())
                {
                    blocked = true;
                    break;
                }

                checkRow += rowDirection;
                checkCol += colDirection;
            }

            if (!blocked)
            {
                return true;
            }
        }
    }

    return false;
}


// =====================================================
// CHECK
// =====================================================
bool ChessGame::isKingInCheck(
    PieceColor color) const
{
    int kingRow = -1;
    int kingCol = -1;

    if (!findKing(
            color,
            kingRow,
            kingCol))
    {
        // If the king does not exist,
        // consider the position invalid/check.

        return true;
    }


    PieceColor enemyColor;

    if (color == PieceColor::White)
    {
        enemyColor = PieceColor::Black;
    }
    else
    {
        enemyColor = PieceColor::White;
    }


    return isSquareAttacked(
        kingRow,
        kingCol,
        enemyColor);
}


// =====================================================
// WOULD MOVE LEAVE KING IN CHECK?
// =====================================================
bool ChessGame::wouldMoveLeaveKingInCheck(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol) const
{
    Piece movingPiece =
        board[fromRow][fromCol];

    if (movingPiece.isEmpty())
    {
        return true;
    }


    // Make a temporary copy of the entire game.

    ChessGame testGame = *this;


    testGame.board[toRow][toCol] =
        testGame.board[fromRow][fromCol];

    testGame.board[fromRow][fromCol] =
        Piece();


    return testGame.isKingInCheck(
        movingPiece.color);
}


// =====================================================
// FULL LEGAL MOVE CHECK
// =====================================================
bool ChessGame::isValidMove(
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


    Piece source =
        board[fromRow][fromCol];


    if (source.isEmpty())
    {
        return false;
    }


    // Only the current player can move.

    if (source.color != currentTurn)
    {
        return false;
    }


    // Check the normal movement rules.

    if (!isValidMoveForPiece(
            fromRow,
            fromCol,
            toRow,
            toCol))
    {
        return false;
    }


    // IMPORTANT:
    //
    // A player cannot make a move that leaves
    // their own king in check.

    if (wouldMoveLeaveKingInCheck(
            fromRow,
            fromCol,
            toRow,
            toCol))
    {
        return false;
    }


    return true;
}


// =====================================================
// HAS ANY LEGAL MOVE?
// =====================================================
bool ChessGame::hasAnyLegalMove(
    PieceColor color) const
{
    for (int fromRow = 0;
         fromRow < 8;
         fromRow++)
    {
        for (int fromCol = 0;
             fromCol < 8;
             fromCol++)
        {
            Piece piece =
                board[fromRow][fromCol];

            if (piece.isEmpty())
            {
                continue;
            }

            if (piece.color != color)
            {
                continue;
            }


            for (int toRow = 0;
                 toRow < 8;
                 toRow++)
            {
                for (int toCol = 0;
                     toCol < 8;
                     toCol++)
                {
                    // We cannot directly use isValidMove()
                    // because that checks currentTurn.
                    //
                    // Temporarily make a copy with this color
                    // as the current player.

                    ChessGame testGame = *this;

                    testGame.currentTurn = color;

                    if (testGame.isValidMove(
                            fromRow,
                            fromCol,
                            toRow,
                            toCol))
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}


// =====================================================
// CHECKMATE
// =====================================================
bool ChessGame::isCheckmate(
    PieceColor color) const
{
    // Must actually be in check.

    if (!isKingInCheck(color))
    {
        return false;
    }


    // If there is any legal move, it is not checkmate.

    if (hasAnyLegalMove(color))
    {
        return false;
    }


    return true;
}


// =====================================================
// STALEMATE
// =====================================================
bool ChessGame::isStalemate(
    PieceColor color) const
{
    // Stalemate means:
    //
    // NOT in check
    // AND
    // NO legal moves

    if (isKingInCheck(color))
    {
        return false;
    }

    return !hasAnyLegalMove(color);
}


// =====================================================
// GAME OVER
// =====================================================
bool ChessGame::isGameOver() const
{
    return
        isCheckmate(currentTurn) ||
        isStalemate(currentTurn);
}


// =====================================================
// MOVE PIECE
// =====================================================
