
#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>

#include "chessgame.h"

class ChessBoard : public QWidget
{
    Q_OBJECT

public:
    explicit ChessBoard(
        int minutes,
        int increment,
        const QString &modeName,
        const QString &playerMode = "Human vs Human",
        const QString &difficulty = "Medium",
        QWidget *parent = nullptr);

private slots:
    void handleSquareClicked();
    void undoLastMove();
    void updateClock();
    void returnToDashboard();
    void openSettings();

private:
    ChessGame game;

    QPushButton *squares[8][8];

    QLabel *turnLabel;
    QLabel *moveLabel;
    QLabel *modeLabel;

    QLabel *whiteClockLabel;
    QLabel *blackClockLabel;
    QLabel *whiteElapsedLabel;
    QLabel *blackElapsedLabel;

    QPushButton *undoButton;
    QPushButton *dashboardButton;
    QPushButton *settingsButton;

    QTimer *clockTimer;

    int selectedRow;
    int selectedCol;

    bool validMoves[8][8];

    int initialTimeSeconds;
    int whiteRemainingSeconds;
    int blackRemainingSeconds;
    int whiteElapsedSeconds;
    int blackElapsedSeconds;

    QString modeName;
    QString playerMode;
    QString difficulty;

    bool resultSaved;
    bool aiMoveInProgress;

    void setupBoard();
    void createHeader(QVBoxLayout *mainLayout);
    void createClockPanel(QVBoxLayout *mainLayout);
    void createBoardGrid(QVBoxLayout *mainLayout);
    void createBottomControls(QVBoxLayout *mainLayout);

    void updateBoard();
    void updateTurnLabel();
    void clearValidMoves();
    void calculateValidMoves(int row, int col);

    QString getSquareStyle(int row, int col) const;
    QString getPieceDisplayText(int row, int col) const;
    QString formatTime(int seconds) const;

    void updateClockLabels();
    void handleTimeOut();

    QLabel *createCoordinateLabel(const QString &text);

    void makeAIMove();
    void checkForGameEnd();
    void saveResult(const QString &result);
};

#endif