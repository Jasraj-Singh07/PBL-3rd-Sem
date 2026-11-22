
#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);

private slots:
    void startSelectedGame();
    void refreshStatistics();
    void openSettings();
    void exitApplication();

private:
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLabel *gamesLabel;
    QLabel *winsLabel;
    QLabel *lossesLabel;
    QLabel *drawsLabel;
    QLabel *recommendationLabel;

    QComboBox *modeCombo;
    QComboBox *difficultyCombo;
    QComboBox *timeCombo;

    QPushButton *startButton;
    QPushButton *refreshButton;
    QPushButton *settingsButton;
    QPushButton *exitButton;

    void updateDifficultyOptions();
};

#endif