
#include "chessboard.h"
#include <QFont>

ChessBoard::ChessBoard(
    int minutes,
    int increment,
    const QString &modeName,
    const QString &playerMode,
    const QString &difficulty,
    QWidget *parent)
    : QWidget(parent)
{
    Q_UNUSED(increment);

    selectedRow = -1;
    selectedCol = -1;

    this->modeName = modeName;
    this->playerMode = playerMode;
    this->difficulty = difficulty;

    resultSaved = false;
    aiMoveInProgress = false;

    initialTimeSeconds = minutes * 60;

    whiteRemainingSeconds = initialTimeSeconds;
    blackRemainingSeconds = initialTimeSeconds;

    whiteElapsedSeconds = 0;
    blackElapsedSeconds = 0;

    clockTimer = new QTimer(this);

    connect(clockTimer, &QTimer::timeout,
            this, &ChessBoard::updateClock);

    clearValidMoves();
    setupBoard();
    updateBoard();

    setAttribute(Qt::WA_DeleteOnClose);

    clockTimer->start(1000);
}

// ============================================================
// CREATE SMALL COORDINATE LABEL
// ============================================================

QLabel *ChessBoard::createCoordinateLabel(
    const QString &text)
{
    QLabel *label =
        new QLabel(text);

    label->setAlignment(
        Qt::AlignCenter);

    QFont font;
    font.setPointSize(10);
    font.setBold(true);

    label->setFont(font);

    label->setStyleSheet(
        "QLabel {"
        "color: #cbd5e1;"
        "background-color: transparent;"
        "}");

    return label;
}


// ============================================================
// SETUP COMPLETE CHESS BOARD
// ============================================================
void ChessBoard::setupBoard()
{
    setWindowTitle("Chess PBL - " + modeName);
    setFixedSize(900, 900);
    setStyleSheet(
        "QWidget { background-color: #111827; color: white; }"
        "QPushButton { border: none; }"
        "QPushButton:hover { border: 2px solid #facc15; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(8);

    createHeader(mainLayout);
    createClockPanel(mainLayout);
    createBoardGrid(mainLayout);
    createBottomControls(mainLayout);
}
