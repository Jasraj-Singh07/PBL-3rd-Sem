
#include "dashboard.h"
#include "chessboard.h"
#include "playerstats.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFont>
#include <QMessageBox>
#include <QApplication>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
{
    PlayerStats::ensureDummyData();

    setWindowTitle("Chess Arena - Dashboard");
    setMinimumSize(820, 700);

    setStyleSheet(
        "QWidget { background:#111827; color:#f9fafb; }"
        "QGroupBox { border:1px solid #374151;"
        "border-radius:10px; margin-top:12px;"
        "padding:12px; font-weight:bold; }"
        "QComboBox { background:#1f2937; padding:9px;"
        "border:1px solid #4b5563; border-radius:6px; }"
        "QPushButton { background:#2563eb; color:white;"
        "padding:11px; border-radius:8px; font-weight:bold; }"
        "QPushButton:hover { background:#3b82f6; }"
    );

    QVBoxLayout *main = new QVBoxLayout(this);
    main->setContentsMargins(35, 25, 35, 25);
    main->setSpacing(15);

    titleLabel = new QLabel("♟  CHESS ARENA");
    QFont titleFont;
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    main->addWidget(titleLabel);

    subtitleLabel = new QLabel(
        "Adaptive Chess  |  C++  |  Data Structures & Algorithms");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    main->addWidget(subtitleLabel);

    QGroupBox *statsBox = new QGroupBox("YOUR STATISTICS");
    QHBoxLayout *stats = new QHBoxLayout(statsBox);

    gamesLabel = new QLabel();
    winsLabel = new QLabel();
    lossesLabel = new QLabel();
    drawsLabel = new QLabel();

    for (QLabel *label :
         {gamesLabel, winsLabel, lossesLabel, drawsLabel})
    {
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(
            "background:#1f2937; padding:12px;"
            "border-radius:8px; font-size:14px;");
        stats->addWidget(label);
    }

    main->addWidget(statsBox);

    QGroupBox *setupBox = new QGroupBox("START A GAME");
    QFormLayout *form = new QFormLayout(setupBox);

    modeCombo = new QComboBox();
    modeCombo->addItem("Human vs Human");
    modeCombo->addItem("Human vs AI");

    difficultyCombo = new QComboBox();
    difficultyCombo->addItems(
        {"Recommended", "Easy", "Medium", "Hard"});

    timeCombo = new QComboBox();
    timeCombo->addItem("Bullet - 1 minute", 1);
    timeCombo->addItem("Blitz - 3 minutes", 3);
    timeCombo->addItem("Rapid - 10 minutes", 10);
    timeCombo->setCurrentIndex(2);

    form->addRow("Game mode:", modeCombo);
    form->addRow("AI difficulty:", difficultyCombo);
    form->addRow("Time control:", timeCombo);

    main->addWidget(setupBox);

    recommendationLabel = new QLabel();
    recommendationLabel->setWordWrap(true);
    recommendationLabel->setStyleSheet(
        "background:#1f2937; padding:12px;"
        "border-radius:8px; color:#93c5fd;");
    main->addWidget(recommendationLabel);

    startButton = new QPushButton("START GAME");
    startButton->setMinimumHeight(48);
    main->addWidget(startButton);

    QHBoxLayout *bottom = new QHBoxLayout();

    refreshButton = new QPushButton("Refresh Statistics");
    settingsButton = new QPushButton("Settings");
    exitButton = new QPushButton("Exit");

    bottom->addWidget(refreshButton);
    bottom->addWidget(settingsButton);
    bottom->addWidget(exitButton);
    main->addLayout(bottom);

    connect(startButton, &QPushButton::clicked,
            this, &Dashboard::startSelectedGame);

    connect(refreshButton, &QPushButton::clicked,
            this, &Dashboard::refreshStatistics);

    connect(settingsButton, &QPushButton::clicked,
            this, &Dashboard::openSettings);

    connect(exitButton, &QPushButton::clicked,
            this, &Dashboard::exitApplication);

    connect(modeCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) {
                updateDifficultyOptions();
            });

    connect(difficultyCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) {
                updateDifficultyOptions();
            });

    updateDifficultyOptions();
    refreshStatistics();
}

void Dashboard::updateDifficultyOptions()
{
    bool aiMode = modeCombo->currentText() == "Human vs AI";

    difficultyCombo->setEnabled(aiMode);

    if (!aiMode)
    {
        recommendationLabel->setText(
            "Human vs Human: both players use the same board.");
        return;
    }

    QString chosen = difficultyCombo->currentText();

    if (chosen == "Recommended")
        chosen = PlayerStats::recommendedDifficulty();

    recommendationLabel->setText(
        "Suggested difficulty: " +
        PlayerStats::recommendedDifficulty() +
        "\n" + PlayerStats::recommendationReason() +
        "\nSelected difficulty: " + chosen);
}

void Dashboard::refreshStatistics()
{
    gamesLabel->setText(
        "Games\n" + QString::number(PlayerStats::totalGames()));

    winsLabel->setText(
        "Wins\n" + QString::number(PlayerStats::wins()));

    lossesLabel->setText(
        "Losses\n" + QString::number(PlayerStats::losses()));

    drawsLabel->setText(
        "Draws\n" + QString::number(PlayerStats::draws()));

    updateDifficultyOptions();
}

void Dashboard::startSelectedGame()
{
    QString playerMode = modeCombo->currentText();
    QString difficulty = difficultyCombo->currentText();

    if (difficulty == "Recommended")
        difficulty = PlayerStats::recommendedDifficulty();

    int minutes = timeCombo->currentData().toInt();

    QString timeName = timeCombo->currentText().section(" - ", 0, 0);

    ChessBoard *board = new ChessBoard(
        minutes, 0, timeName, playerMode, difficulty);

    board->setAttribute(Qt::WA_DeleteOnClose);

    connect(board, &QObject::destroyed, this, [this]() {
        refreshStatistics();
        show();
        raise();
        activateWindow();
    });

    hide();
    board->show();
}

void Dashboard::openSettings()
{
    QMessageBox::information(
        this, "Settings",
        "Chess Arena\n\n"
        "• Legal move highlighting\n"
        "• Board coordinates\n"
        "• Saved game history\n"
        "• Adaptive difficulty recommendations");
}

void Dashboard::exitApplication()
{
    QApplication::quit();
}