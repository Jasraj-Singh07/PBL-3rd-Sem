#include "chessboard.h"

#include <QFont>

void ChessBoard::createHeader(QVBoxLayout *mainLayout)
{
    QHBoxLayout *header = new QHBoxLayout();

    modeLabel = new QLabel(modeName + " Mode");
    QFont modeFont;
    modeFont.setPointSize(18);
    modeFont.setBold(true);
    modeLabel->setFont(modeFont);
    header->addWidget(modeLabel);
    header->addStretch();

    dashboardButton = new QPushButton("⌂ Dashboard");
    settingsButton = new QPushButton("⚙ Settings");
    dashboardButton->setFixedSize(120, 38);
    settingsButton->setFixedSize(110, 38);
    header->addWidget(dashboardButton);
    header->addWidget(settingsButton);
    mainLayout->addLayout(header);

    connect(dashboardButton, &QPushButton::clicked,
            this, &ChessBoard::returnToDashboard);
    connect(settingsButton, &QPushButton::clicked,
            this, &ChessBoard::openSettings);
}

void ChessBoard::createClockPanel(QVBoxLayout *mainLayout)
{
    QHBoxLayout *clocks = new QHBoxLayout();
    whiteClockLabel = new QLabel();
    blackClockLabel = new QLabel();
    whiteElapsedLabel = new QLabel();
    blackElapsedLabel = new QLabel();

    whiteClockLabel->setAlignment(Qt::AlignCenter);
    blackClockLabel->setAlignment(Qt::AlignCenter);
    whiteElapsedLabel->setAlignment(Qt::AlignCenter);
    blackElapsedLabel->setAlignment(Qt::AlignCenter);

    QFont clockFont;
    clockFont.setPointSize(22);
    clockFont.setBold(true);
    whiteClockLabel->setFont(clockFont);
    blackClockLabel->setFont(clockFont);

    whiteClockLabel->setStyleSheet(
        "QLabel { background-color: #f8fafc; color: #111827;"
        "border-radius: 8px; padding: 6px; }");
    blackClockLabel->setStyleSheet(
        "QLabel { background-color: #374151; color: white;"
        "border-radius: 8px; padding: 6px; }");

    clocks->addWidget(whiteClockLabel);
    clocks->addWidget(blackClockLabel);
    mainLayout->addLayout(clocks);

    QHBoxLayout *elapsed = new QHBoxLayout();
    elapsed->addWidget(whiteElapsedLabel);
    elapsed->addWidget(blackElapsedLabel);
    mainLayout->addLayout(elapsed);

    turnLabel = new QLabel("White's Turn");
    QFont turnFont;
    turnFont.setPointSize(18);
    turnFont.setBold(true);
    turnLabel->setFont(turnFont);
    turnLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(turnLabel);
}

void ChessBoard::createBoardGrid(QVBoxLayout *mainLayout)
{
    QGridLayout *boardLayout = new QGridLayout();
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(15, 5, 15, 5);

    boardLayout->addWidget(createCoordinateLabel(""), 0, 0);
    for (int col = 0; col < 8; ++col)
        boardLayout->addWidget(
            createCoordinateLabel(QString(QChar('a' + col))), 0, col + 1);
    boardLayout->addWidget(createCoordinateLabel(""), 0, 9);

    for (int row = 0; row < 8; ++row)
    {
        int rank = 8 - row;
        boardLayout->addWidget(
            createCoordinateLabel(QString::number(rank)), row + 1, 0);

        for (int col = 0; col < 8; ++col)
        {
            squares[row][col] = new QPushButton();
            squares[row][col]->setFixedSize(78, 78);

            QFont pieceFont;
            pieceFont.setPointSize(36);
            squares[row][col]->setFont(pieceFont);
            squares[row][col]->setFocusPolicy(Qt::NoFocus);
            squares[row][col]->setProperty("row", row);
            squares[row][col]->setProperty("col", col);

            connect(squares[row][col], &QPushButton::clicked,
                    this, &ChessBoard::handleSquareClicked);
            boardLayout->addWidget(squares[row][col], row + 1, col + 1);
        }

        boardLayout->addWidget(
            createCoordinateLabel(QString::number(rank)), row + 1, 9);
    }

    boardLayout->addWidget(createCoordinateLabel(""), 9, 0);
    for (int col = 0; col < 8; ++col)
        boardLayout->addWidget(
            createCoordinateLabel(QString(QChar('a' + col))), 9, col + 1);
    boardLayout->addWidget(createCoordinateLabel(""), 9, 9);

    mainLayout->addLayout(boardLayout);
}

void ChessBoard::createBottomControls(QVBoxLayout *mainLayout)
{
    moveLabel = new QLabel("Moves: 0");
    moveLabel->setAlignment(Qt::AlignCenter);

    QFont moveFont;
    moveFont.setPointSize(12);
    moveFont.setBold(true);
    moveLabel->setFont(moveFont);
    mainLayout->addWidget(moveLabel);

    undoButton = new QPushButton("↶ Undo Last Move");
    undoButton->setFixedHeight(40);
    mainLayout->addWidget(undoButton);

    QLabel *infoLabel = new QLabel("Select a piece to see all valid moves.");
    infoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(infoLabel);

    connect(undoButton, &QPushButton::clicked,
            this, &ChessBoard::undoLastMove);
}
