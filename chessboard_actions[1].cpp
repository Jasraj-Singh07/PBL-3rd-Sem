#include <QInputDialog>

#include "chessboard.h"
#include "ai.h"
#include "playerstats.h"
#include "soundeffects.h"
#include <QSettings>
#include <QDateTime>

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

    Piece targetPiece = game.getPiece(row, col);
    bool isCapture = !targetPiece.isEmpty();

    PieceType promotion = PieceType::Queen;
    if (game.getPiece(selectedRow, selectedCol).type == PieceType::Pawn &&
        (row == 0 || row == 7) && game.isValidMove(selectedRow, selectedCol, row, col))
    {
        const QStringList options{"Queen", "Rook", "Bishop", "Knight"};
        bool accepted = false;
        const QString choice = QInputDialog::getItem(this, "Promote pawn", "Piece", options, 0, false, &accepted);
        if (!accepted)
            return;
        promotion = static_cast<PieceType>(int(PieceType::Queen) + options.indexOf(choice));
    }
    if (game.movePiece(selectedRow, selectedCol, row, col, promotion))
    {
        if (isCapture)
        {
            SoundEffects::capture();
            QSettings settings("ChessPBL", "ChessArena");
            if (settings.value("settings/animations", true).toBool())
            {
                reactionLabel->setText("🐈 BONK! NICE CAPTURE");
                reactionLabel->show();
                QTimer::singleShot(1000, reactionLabel, [this]() {
                    if (reactionLabel) reactionLabel->hide();
                });
            }
        }
        else
            SoundEffects::move();

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
        bool capture = !game.getPiece(move.toRow, move.toCol).isEmpty();
        game.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol, move.promotion);
        if (capture) SoundEffects::capture(); else SoundEffects::move();
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

    GameRecord record;
    record.date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    record.mode = playerMode;
    record.difficulty = playerMode == "Human vs AI" ? difficulty : "Not applicable";
    record.result = result;
    record.type = "REAL";
    record.moves = game.getMoveCount();
    record.durationSeconds = whiteElapsedSeconds + blackElapsedSeconds;
    record.averageMoveTime = record.moves > 0
        ? double(record.durationSeconds) / record.moves : 0.0;

    // Until position-by-position engine analysis is added, this is a
    // transparent outcome-based estimate, not a chess-engine accuracy score.
    record.accuracy = result == "WIN" ? 82.0 :
                      (result == "DRAW" ? 74.0 : 66.0);
    record.blunders = result == "LOSS" ? 2 : (result == "DRAW" ? 1 : 0);
    PlayerStats::addGame(record);
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

        QString roast;
        if (playerMode == "Human vs AI")
            roast = winner == "White"
                ? "\n\n🔥 The AI just got sent back to the tutorial."
                : "\n\n💀 The AI read your moves like an open book.";
        else
            roast = "\n\n♟ Clean finish. Run it back?";
        QMessageBox::information(
            this, "Checkmate",
            winner + " wins by checkmate!" + roast);

        returnToDashboard();
    }
    else if (game.isStalemate(turn) || !game.drawReason().isEmpty())
    {
        saveResult("DRAW");
        clockTimer->stop();

        QMessageBox::information(
            this, "Draw",
            game.drawReason().isEmpty() ? QString("Stalemate") : game.drawReason());

        returnToDashboard();
    }
}

void ChessBoard::undoLastMove()
{
    if (game.undoMove())
    {
        // In Human vs AI, undo the human move and the AI reply together
        // whenever both moves exist, returning control to the human.
        if (playerMode == "Human vs AI" &&
            game.getCurrentTurn() == PieceColor::Black)
            game.undoMove();

        selectedRow = -1;
        selectedCol = -1;
        clearValidMoves();
        updateBoard();
    }
}

void ChessBoard::redoLastMove()
{
    if (!game.redoMove())
        return;

    if (playerMode == "Human vs AI" &&
        game.getCurrentTurn() == PieceColor::Black)
        game.redoMove();

    selectedRow = -1;
    selectedCol = -1;
    clearValidMoves();
    updateBoard();
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