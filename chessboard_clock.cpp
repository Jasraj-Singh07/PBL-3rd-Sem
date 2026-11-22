#include "chessboard.h"
#include <QMessageBox>

QString ChessBoard::formatTime(
    int seconds) const
{
    if (seconds < 0)
    {
        seconds = 0;
    }

    int minutes =
        seconds / 60;

    int remainingSeconds =
        seconds % 60;

    return QString("%1:%2")
        .arg(
            minutes,
            2,
            10,
            QChar('0'))
        .arg(
            remainingSeconds,
            2,
            10,
            QChar('0'));
}


// ============================================================
// UPDATE CLOCK LABELS
// ============================================================
void ChessBoard::updateClockLabels()
{
    whiteClockLabel->setText(
        "WHITE\n" +
        formatTime(
            whiteRemainingSeconds));

    blackClockLabel->setText(
        "BLACK\n" +
        formatTime(
            blackRemainingSeconds));

    whiteElapsedLabel->setText(
        "Elapsed: " +
        formatTime(
            whiteElapsedSeconds));

    blackElapsedLabel->setText(
        "Elapsed: " +
        formatTime(
            blackElapsedSeconds));
}


// ============================================================
// CLOCK
// ============================================================
void ChessBoard::updateClock()
{
    if (game.getCurrentTurn() ==
        PieceColor::White)
    {
        if (whiteRemainingSeconds > 0)
        {
            whiteRemainingSeconds--;

            whiteElapsedSeconds++;
        }
        else
        {
            handleTimeOut();

            return;
        }
    }
    else
    {
        if (blackRemainingSeconds > 0)
        {
            blackRemainingSeconds--;

            blackElapsedSeconds++;
        }
        else
        {
            handleTimeOut();

            return;
        }
    }

    updateClockLabels();
}


// ============================================================
// TIME OUT
// ============================================================

void ChessBoard::handleTimeOut()
{
    clockTimer->stop();

    QString winner =
        game.getCurrentTurn() == PieceColor::White
            ? "Black" : "White";

    QString result =
        winner == "White" ? "WIN" : "LOSS";

    saveResult(result);

    QMessageBox::information(
        this,
        "Time Out",
        winner + " wins because the opponent's time expired.");

    returnToDashboard();
}


// ============================================================
// RETURN TO DASHBOARD
// ============================================================
