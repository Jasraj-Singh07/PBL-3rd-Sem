
#include "chessboard.h"
#include "ai.h"
#include "playerstats.h"

#include <QMessageBox>
#include <QTimer>

void ChessBoard::handleSquareClicked()
{
    if (aiMoveInProgress)
        return;

    // Black is controlled by AI in this mode.
    if (playerMode == "Human vs AI" &&
        game.getCurrentTurn() == PieceColor::Black)
        return;

    QPushButton *button =
        qobject_cast<QPushButton *>(sender());

    if (!button)
        return;

    int row = button->property("row").toInt();
    int col = button->property("col").toInt();

    if (selectedRow == -1)
    {
        Piece piece = game.getPiece(row, col);

        if (piece.isEmpty() ||
            piece.color != game.getCurrentTurn())
            return;

        selectedRow = row;
        selectedCol = col;

        calculateValidMoves(row, col);
        updateBoard();
        return;
    }

    if (selectedRow == row && selectedCol == col)
    {
        selectedRow = -1;
        selectedCol = -1;
        clearValidMoves();
        updateBoard();
        return;
    }

    if (game.movePiece(selectedRow, selectedCol, row, col))
    {
        selectedRow = -1;
        selectedCol = -1;

        clearValidMoves();
        updateBoard();
        checkForGameEnd();

        if (!game.isGameOver() &&
            playerMode == "Human vs AI" &&
            game.getCurrentTurn() == PieceColor::Black)
        {
            QTimer::singleShot(
                250, this, &ChessBoard::makeAIMove);
        }

        return;
    }

    // Allow selecting another piece belonging to this player.
    Piece piece = game.getPiece(row, col);

    if (!piece.isEmpty() &&
        piece.color == game.getCurrentTurn())
    {
        selectedRow = row;
        selectedCol = col;
        calculateValidMoves(row, col);
    }

    updateBoard();
}

void ChessBoard::makeAIMove()
{
    if (playerMode != "Human vs AI" ||
        game.getCurrentTurn() != PieceColor::Black ||
        game.isGameOver())
        return;

    aiMoveInProgress = true;

    Move move;

    if (ChessAI::chooseMove(game, difficulty, move))
    {
        game.movePiece(
            move.fromRow, move.fromCol,
            move.toRow, move.toCol);
    }

    aiMoveInProgress = false;

    selectedRow = -1;
    selectedCol = -1;

    clearValidMoves();
    updateBoard();
    checkForGameEnd();
}

void ChessBoard::saveResult(const QString &result)
{
    if (resultSaved)
        return;

    resultSaved = true;

    PlayerStats::addGame(
        playerMode,
        playerMode == "Human vs AI"
            ? difficulty : "None",
        result);
}

void ChessBoard::checkForGameEnd()
{
    PieceColor turn = game.getCurrentTurn();

    if (game.isCheckmate(turn))
    {
        QString winner =
            turn == PieceColor::White ? "Black" : "White";

        QString result =
            winner == "White" ? "WIN" : "LOSS";

        saveResult(result);
        clockTimer->stop();

        QMessageBox::information(
            this, "Checkmate",
            winner + " wins by checkmate!");

        returnToDashboard();
    }
    else if (game.isStalemate(turn))
    {
        saveResult("DRAW");
        clockTimer->stop();

        QMessageBox::information(
            this, "Draw", "The game ended in stalemate.");

        returnToDashboard();
    }
}

void ChessBoard::undoLastMove()
{
    if (playerMode == "Human vs AI")
    {
        QMessageBox::information(
            this, "Undo",
            "Undo is currently available only in Human vs Human mode.");
        return;
    }

    if (game.undoMove())
    {
        selectedRow = -1;
        selectedCol = -1;

        clearValidMoves();
        updateBoard();
    }
}

void ChessBoard::returnToDashboard()
{
    clockTimer->stop();
    close();
}

void ChessBoard::openSettings()
{
    QMessageBox::information(
        this, "Game Settings",
        "Mode: " + playerMode +
        "\nTime control: " + modeName +
        "\nDifficulty: " + difficulty +
        "\n\nLegal move highlighting: ON"
        "\nBoard coordinates: ON");
}