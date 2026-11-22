#include "dashboard.h"
#include "chessboard.h"
#include "playerstats.h"
#include "performancechart.h"
#include "soundeffects.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFont>
#include <QMessageBox>
#include <QApplication>
#include <QHeaderView>
#include <QSettings>
#include <QInputDialog>
#include <QDateTime>
#include <QVector>
#include <QScrollArea>
#include <QFrame>
#include <QGuiApplication>

static QWidget *makeStatCard(const QString &caption, QLabel **valueLabel,
                            const QString &accent)
{
    QFrame *frame = new QFrame;
    frame->setStyleSheet(QString("QFrame{background:#202136;border:1px solid %1;"
                                 "border-radius:14px;}").arg(accent));
    QVBoxLayout *layout = new QVBoxLayout(frame);
    QLabel *value = new QLabel("0");
    value->setStyleSheet(QString("color:%1;font-size:25px;font-weight:800;border:0;").arg(accent));
    QLabel *name = new QLabel(caption);
    name->setStyleSheet("color:#b8b8ca;font-size:12px;border:0;");
    layout->addWidget(value);
    layout->addWidget(name);
    *valueLabel = value;
    return frame;
}

Dashboard::Dashboard(QWidget *parent) : QWidget(parent)
{
    PlayerStats::ensureDummyData();
    setWindowTitle("Chess Arena | Player Hub");
    setMinimumSize(960, 720);
    setStyleSheet(
        "QWidget{background:#11121d;color:#f4f4fb;font-family:'Segoe UI';}"
        "QTabWidget::pane{border:1px solid #303149;border-radius:14px;top:-1px;}"
        "QTabBar::tab{background:#1c1d2d;color:#b9bad0;padding:12px 20px;"
        "margin-right:5px;border-top-left-radius:9px;border-top-right-radius:9px;}"
        "QTabBar::tab:selected{background:#7c3aed;color:white;font-weight:bold;}"
        "QGroupBox{border:1px solid #34354c;border-radius:13px;margin-top:12px;"
        "padding:14px;font-weight:bold;color:#c4b5fd;}"
        "QComboBox,QLineEdit,QSpinBox{background:#202136;border:1px solid #41425c;"
        "border-radius:8px;padding:9px;color:white;}"
        "QPushButton{background:#7c3aed;color:white;border:0;border-radius:9px;"
        "padding:11px 16px;font-weight:700;}"
        "QPushButton:hover{background:#8b5cf6;}"
        "QTableWidget{background:#191a29;gridline-color:#34354c;border:0;"
        "selection-background-color:#49317c;}"
        "QHeaderView::section{background:#292a40;color:#c4b5fd;padding:8px;border:0;}"
        "QCheckBox{spacing:8px;}");

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(22, 18, 22, 20);
    QLabel *brand = new QLabel("♟  CHESS ARENA");
    brand->setStyleSheet("font-size:27px;font-weight:900;color:#c4b5fd;");
    QLabel *tag = new QLabel("YOUR GAME. YOUR DATA. YOUR NEXT LEVEL.");
    tag->setStyleSheet("color:#8b8ca6;letter-spacing:2px;");
    root->addWidget(brand);
    root->addWidget(tag);
    tabs = new QTabWidget;
    root->addWidget(tabs, 1);

    buildHomeTab();
    buildHistoryTab();
    buildProfileTab();
    buildSettingsTab();
    applyTheme(QSettings("ChessPBL", "ChessArena").value("settings/theme", "Charcoal Violet").toString());
    refreshStatistics();
}

void Dashboard::buildHomeTab()
{
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);
    QLabel *headline = new QLabel("Ready for your next match?");
    headline->setStyleSheet("font-size:23px;font-weight:800;color:#f5f3ff;");
    layout->addWidget(headline);

    QHBoxLayout *cards = new QHBoxLayout;
    cards->addWidget(makeStatCard("GAMES PLAYED", &gamesLabel, "#a78bfa"));
    cards->addWidget(makeStatCard("WINS", &winsLabel, "#34d399"));
    cards->addWidget(makeStatCard("LOSSES", &lossesLabel, "#fb7185"));
    cards->addWidget(makeStatCard("DRAWS", &drawsLabel, "#22d3ee"));
    layout->addLayout(cards);

    QGroupBox *chartBox = new QGroupBox("PERFORMANCE OVER YOUR RECENT GAMES");
    QVBoxLayout *chartLayout = new QVBoxLayout(chartBox);
    chart = new PerformanceChart;
    chartLayout->addWidget(chart);
    layout->addWidget(chartBox);

    QGroupBox *startBox = new QGroupBox("QUEUE UP");
    QFormLayout *form = new QFormLayout(startBox);
    modeCombo = new QComboBox;
    modeCombo->addItems({"Human vs AI", "Human vs Human"});
    difficultyCombo = new QComboBox;
    difficultyCombo->addItems({"Recommended", "Easy", "Medium", "Hard"});
    timeCombo = new QComboBox;
    timeCombo->addItem("Bullet - 1 minute", 1);
    timeCombo->addItem("Blitz - 3 minutes", 3);
    timeCombo->addItem("Rapid - 10 minutes", 10);
    timeCombo->setCurrentIndex(2);
    form->addRow("Game mode", modeCombo);
    form->addRow("AI difficulty", difficultyCombo);
    form->addRow("Time control", timeCombo);
    layout->addWidget(startBox);

    recommendationLabel = new QLabel;
    recommendationLabel->setWordWrap(true);
    recommendationLabel->setStyleSheet("background:#202136;color:#c4b5fd;padding:12px;border-radius:9px;");
    layout->addWidget(recommendationLabel);

    QHBoxLayout *buttons = new QHBoxLayout;
    startButton = new QPushButton("♟  START PLAYING");
    QPushButton *history = new QPushButton("VIEW HISTORY");
    QPushButton *refresh = new QPushButton("REFRESH");
    buttons->addWidget(startButton, 2);
    buttons->addWidget(history);
    buttons->addWidget(refresh);
    layout->addLayout(buttons);

    connect(startButton, &QPushButton::clicked, this, &Dashboard::startSelectedGame);
    connect(history, &QPushButton::clicked, this, [this](){ playUiSound(); tabs->setCurrentIndex(1); });
    connect(refresh, &QPushButton::clicked, this, &Dashboard::refreshStatistics);
    connect(modeCombo, &QComboBox::currentTextChanged, this, [this](){ updateDifficultyOptions(); });
    connect(difficultyCombo, &QComboBox::currentTextChanged, this, [this](){ updateDifficultyOptions(); });
    tabs->addTab(page, "✦  Home");
}

void Dashboard::buildHistoryTab()
{
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);
    QLabel *heading = new QLabel("MATCH HISTORY");
    heading->setStyleSheet("font-size:22px;font-weight:800;color:#c4b5fd;");
    layout->addWidget(heading);
    historyTable = new QTableWidget(0, 7);
    historyTable->setHorizontalHeaderLabels({"Date", "Mode", "Opponent level", "Result", "Accuracy", "Avg move", "Data"});
    historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(historyTable);
    tabs->addTab(page, "▤  History");
}

void Dashboard::buildProfileTab()
{
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);
    QLabel *heading = new QLabel("PLAYER PROFILE");
    heading->setStyleSheet("font-size:22px;font-weight:800;color:#c4b5fd;");
    layout->addWidget(heading);
    nameEdit = new QLineEdit(PlayerStats::playerName());
    nameEdit->setPlaceholderText("Enter player name");
    layout->addWidget(new QLabel("Display name"));
    layout->addWidget(nameEdit);
    profileSummary = new QLabel;
    profileSummary->setWordWrap(true);
    profileSummary->setStyleSheet("background:#202136;padding:20px;border-radius:12px;font-size:15px;");
    layout->addWidget(profileSummary);
    QPushButton *saveName = new QPushButton("SAVE PROFILE");
    layout->addWidget(saveName);
    layout->addStretch();
    connect(saveName, &QPushButton::clicked, this, [this](){
        playUiSound();
        PlayerStats::setPlayerName(nameEdit->text());
        refreshStatistics();
        QMessageBox::information(this, "Profile", "Player profile saved.");
    });
    tabs->addTab(page, "☺  Profile");
}

void Dashboard::buildSettingsTab()
{
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);
    QLabel *heading = new QLabel("PERSONALIZE YOUR ARENA");
    heading->setStyleSheet("font-size:22px;font-weight:800;color:#c4b5fd;");
    layout->addWidget(heading);
    QSettings settings("ChessPBL", "ChessArena");
    QGroupBox *group = new QGroupBox("GAME EXPERIENCE");
    QFormLayout *form = new QFormLayout(group);
    themeCombo = new QComboBox;
    themeCombo->addItems({"Charcoal Violet", "Midnight Blue", "Forest Green", "Warm Classic"});
    themeCombo->setCurrentText(settings.value("settings/theme", "Charcoal Violet").toString());
    boardThemeCombo = new QComboBox;
    boardThemeCombo->addItems({"Classic Green", "Ocean Blue", "Purple Haze", "Walnut"});
    boardThemeCombo->setCurrentText(settings.value("settings/boardTheme", "Classic Green").toString());
    soundCheck = new QCheckBox("Enable interface sounds");
    soundCheck->setChecked(settings.value("settings/sound", true).toBool());
    animationsCheck = new QCheckBox("Enable capture reactions / animations");
    animationsCheck->setChecked(settings.value("settings/animations", true).toBool());
    hintsCheck = new QCheckBox("Show move hints");
    hintsCheck->setChecked(settings.value("settings/hints", true).toBool());
    form->addRow("Dashboard theme", themeCombo);
    form->addRow("Chessboard theme", boardThemeCombo);
    form->addRow(soundCheck);
    form->addRow(animationsCheck);
    form->addRow(hintsCheck);
    layout->addWidget(group);
    QPushButton *save = new QPushButton("SAVE SETTINGS");
    layout->addWidget(save);
    QLabel *note = new QLabel("Settings are stored locally on this computer. Some options apply to newly opened games.");
    note->setWordWrap(true);
    note->setStyleSheet("color:#9293aa;");
    layout->addWidget(note);
    layout->addStretch();
    connect(save, &QPushButton::clicked, this, &Dashboard::saveSettings);
    tabs->addTab(page, "⚙  Settings");
}

void Dashboard::playUiSound()
{
    QSettings settings("ChessPBL", "ChessArena");
    if (settings.value("settings/sound", true).toBool())
        SoundEffects::button();
}

void Dashboard::applyTheme(const QString &theme)
{
    QString background = "#11121d", accent = "#7c3aed";
    if (theme == "Midnight Blue") { background = "#0b1424"; accent = "#2563eb"; }
    else if (theme == "Forest Green") { background = "#101d19"; accent = "#059669"; }
    else if (theme == "Warm Classic") { background = "#211b18"; accent = "#b7791f"; }
    setStyleSheet(QString("QWidget{background:%1;color:#f4f4fb;font-family:'Segoe UI';}"
                          "QTabWidget::pane{border:1px solid #34354c;border-radius:14px;}"
                          "QTabBar::tab{background:#202136;color:#c7c7d8;padding:12px 20px;}"
                          "QTabBar::tab:selected{background:%2;color:white;}"
                          "QPushButton{background:%2;color:white;border:0;border-radius:9px;padding:11px;font-weight:700;}"
                          "QLineEdit,QComboBox{background:#202136;border:1px solid #41425c;border-radius:8px;padding:9px;color:white;}")
                    .arg(background, accent));
}

void Dashboard::saveSettings()
{
    playUiSound();
    QSettings settings("ChessPBL", "ChessArena");
    settings.setValue("settings/theme", themeCombo->currentText());
    settings.setValue("settings/boardTheme", boardThemeCombo->currentText());
    settings.setValue("settings/sound", soundCheck->isChecked());
    settings.setValue("settings/animations", animationsCheck->isChecked());
    settings.setValue("settings/hints", hintsCheck->isChecked());
    applyTheme(themeCombo->currentText());
    QMessageBox::information(this, "Settings", "Your preferences have been saved.");
}

void Dashboard::updateDifficultyOptions()
{
    bool aiMode = modeCombo->currentText() == "Human vs AI";
    difficultyCombo->setEnabled(aiMode);
    if (!aiMode)
        recommendationLabel->setText("Local match: both players use the same board.");
    else
        recommendationLabel->setText("✨ Adaptive recommendation: " +
            PlayerStats::recommendedDifficulty() + "\n" +
            PlayerStats::recommendationReason() +
            "\nChoose Recommended to use this level automatically.");
}

void Dashboard::refreshStatistics()
{
    gamesLabel->setText(QString::number(PlayerStats::totalGames()));
    winsLabel->setText(QString::number(PlayerStats::wins()));
    lossesLabel->setText(QString::number(PlayerStats::losses()));
    drawsLabel->setText(QString::number(PlayerStats::draws()));
    accuracyLabel = nullptr;

    QVector<double> accuracy, sense;
    QList<GameRecord> records = PlayerStats::history();
    int start = qMax(0, records.size() - 10);
    for (int i = start; i < records.size(); ++i)
    {
        accuracy.append(records[i].accuracy);
        sense.append(qBound(0.0, records[i].accuracy - records[i].blunders * 4.0 + 5.0, 100.0));
    }
    chart->setValues(accuracy, sense);

    historyTable->setRowCount(records.size());
    for (int row = 0; row < records.size(); ++row)
    {
        const GameRecord &r = records[records.size() - 1 - row];
        QStringList values = {r.date, r.mode, r.difficulty, r.result,
                              QString::number(r.accuracy, 'f', 1) + "%",
                              QString::number(r.averageMoveTime, 'f', 1) + "s",
                              r.type};
        for (int col = 0; col < values.size(); ++col)
            historyTable->setItem(row, col, new QTableWidgetItem(values[col]));
    }

    if (profileSummary)
        profileSummary->setText(
            "Player: " + PlayerStats::playerName() +
            "\n\nGames played: " + QString::number(PlayerStats::totalGames()) +
            "\nWin rate: " + QString::number(PlayerStats::totalGames() ?
                100.0 * PlayerStats::wins() / PlayerStats::totalGames() : 0.0, 'f', 1) + "%" +
            "\nAverage accuracy: " + QString::number(PlayerStats::averageAccuracy(), 'f', 1) + "%" +
            "\nAverage move time: " + QString::number(PlayerStats::averageMoveTime(), 'f', 1) + " seconds" +
            "\nBlunders recorded: " + QString::number(PlayerStats::totalBlunders()) +
            "\n\nDemo records are labelled DEMO; your completed matches are labelled REAL.");
    updateDifficultyOptions();
}

void Dashboard::startSelectedGame()
{
    playUiSound();
    QString playerMode = modeCombo->currentText();
    QString difficulty = difficultyCombo->currentText();
    if (difficulty == "Recommended")
        difficulty = PlayerStats::recommendedDifficulty();
    int minutes = timeCombo->currentData().toInt();
    QString timeName = timeCombo->currentText().section(" - ", 0, 0);

    ChessBoard *board = new ChessBoard(minutes, 0, timeName, playerMode, difficulty);
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

void Dashboard::exitApplication()
{
    playUiSound();
    QApplication::quit();
}
