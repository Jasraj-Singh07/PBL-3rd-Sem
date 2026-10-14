#ifndef CHESSGAME_H
#define CHESSGAME_H

#include "piece.h"
#include "move.h"
#include "movehistory.h"

#include <QString>

class ChessGame
{
private:
    Piece board[8][8];

    MoveHistory moveHistory;

    PieceColor currentTurn;

public:
    ChessGame();

    void initializeBoard();

    Piece getPiece(int row, int col) const;

    PieceColor getCurrentTurn() const;

    QString getPieceSymbol(
        int row,
        int col) const;

    QString getPieceName(
        int row,
        int col) const;

    bool isInsideBoard(
        int row,
        int col) const;

    // Checks whether a complete legal move can be made.
    bool isValidMove(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol) const;

    // Makes a legal move.
    bool movePiece(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol);

    bool undoMove();

    int getMoveCount() const;

    // Checks only the movement rules of the individual piece.
    bool isValidMoveForPiece(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol) const;

    // ===== CHECK / CHECKMATE =====

    bool isKingInCheck(
        PieceColor color) const;

    bool isCheckmate(
        PieceColor color) const;

    bool isStalemate(
        PieceColor color) const;

    bool isGameOver() const;

private:
    bool isPawnMoveValid(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol) const;

    bool isRookMoveValid(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol) const;

    bool isBishopMoveValid(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol) const;

    bool isKnightMoveValid(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol) const;

    bool isQueenMoveValid(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol) const;

    bool isKingMoveValid(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol) const;

    bool isPathClear(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol) const;

    // Finds the king of the specified color.
    bool findKing(
        PieceColor color,
        int &kingRow,
        int &kingCol) const;

    // Checks whether a square is attacked by a particular color.
    bool isSquareAttacked(
        int row,
        int col,
        PieceColor attackingColor) const;

    // Checks whether making a move would leave the moving
    // player's king in check.
    bool wouldMoveLeaveKingInCheck(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol) const;

    // Checks whether a player has at least one legal move.
    bool hasAnyLegalMove(
        PieceColor color) const;
};

#endif