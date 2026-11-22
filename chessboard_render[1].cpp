#include "chessboard.h"
#include <QSettings>
#include <QFont>

QString ChessBoard::getPieceDisplayText(
    int row,
    int col) const
{
    Piece piece =
        game.getPiece(
            row,
            col);

    if (piece.isEmpty())
    {
        return "";
    }

    // ONLY the chess symbol.
    // No King/Pawn name.
    // No e4/e5 coordinate.
    return game.getPieceSymbol(
        row,
        col);
}


// ============================================================
// SQUARE COLORS
// ============================================================
QString ChessBoard::getSquareStyle(
    int row,
    int col) const
{
    bool selected =
        (row == selectedRow &&
         col == selectedCol);

    bool valid =
        validMoves[row][col];

    Piece piece =
        game.getPiece(
            row,
            col);

    bool capture =
        valid &&
        !piece.isEmpty();


    // Selected square
    if (selected)
    {
        return
            "QPushButton {"
            "background-color: #facc15;"
            "border: 3px solid #f59e0b;"
            "}";
    }


    // Capture square
    if (capture)
    {
        return
            "QPushButton {"
            "background-color: #ef4444;"
            "border: 3px solid #991b1b;"
            "}";
    }


    // Normal valid move
    if (valid)
    {
        return
            "QPushButton {"
            "background-color: #4ade80;"
            "border: 3px solid #15803d;"
            "}";
    }


    QSettings settings("ChessPBL", "ChessArena");
    QString theme = settings.value("settings/boardTheme", "Classic Green").toString();
    QString light = "#f0d9b5";
    QString dark = "#769656";
    if (theme == "Ocean Blue") { light = "#dbeafe"; dark = "#2563a6"; }
    else if (theme == "Purple Haze") { light = "#e9d5ff"; dark = "#7e22ce"; }
    else if (theme == "Walnut") { light = "#e7c9a9"; dark = "#70452d"; }

    if ((row + col) % 2 == 0)
        return "QPushButton { background-color:" + light + "; border:none; }";

    return "QPushButton { background-color:" + dark + "; border:none; }";
}


// ============================================================
// CLEAR VALID MOVES
// ============================================================
void ChessBoard::clearValidMoves()
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            validMoves[row][col] =
                false;
        }
    }
}


// ============================================================
// FIND ALL VALID MOVES
// ============================================================
void ChessBoard::calculateValidMoves(
    int row,
    int col)
{
    clearValidMoves();

    Piece piece =
        game.getPiece(
            row,
            col);

    if (piece.isEmpty())
    {
        return;
    }

    for (int targetRow = 0;
         targetRow < 8;
         targetRow++)
    {
        for (int targetCol = 0;
             targetCol < 8;
             targetCol++)
        {
            if (game.isValidMove(
                    row,
                    col,
                    targetRow,
                    targetCol))
            {
                validMoves[targetRow][targetCol] =
                    true;
            }
        }
    }
}


// ============================================================
// UPDATE ENTIRE BOARD
// ============================================================
void ChessBoard::updateBoard()
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            Piece piece =
                game.getPiece(
                    row,
                    col);

            // Only symbol
            squares[row][col]->setText(
                getPieceDisplayText(
                    row,
                    col));
                    squares[row][col]->setToolTip(
            game.getPieceName(row, col));


            // ------------------------------------------------
            // Piece colors
            // ------------------------------------------------

            QString style =
                getSquareStyle(
                    row,
                    col);

            if (piece.color ==
                PieceColor::White)
            {
                style +=
                    "QPushButton {"
                    "color: #ffffff;"
                    "}";
            }
            else if (piece.color ==
                     PieceColor::Black)
            {
                style +=
                    "QPushButton {"
                    "color: #111111;"
                    "}";
            }

            squares[row][col]->setStyleSheet(
                style);
        }
    }

    updateTurnLabel();

    moveLabel->setText(
        "Moves: " +
        QString::number(
            game.getMoveCount()));

    updateClockLabels();
}


// ============================================================
// TURN
// ============================================================
void ChessBoard::updateTurnLabel()
{
    if (game.getCurrentTurn() ==
        PieceColor::White)
    {
        turnLabel->setText(
            "White's Turn");
    }
    else
    {
        turnLabel->setText(
            "Black's Turn");
    }
}


// ============================================================
// HANDLE BOARD CLICK
// ============================================================
