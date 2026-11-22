#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>

class PerformanceChart;

class Dashboard : public QWidget
{
    Q_OBJECT
public:
    explicit Dashboard(QWidget *parent = nullptr);

private slots:
    void startSelectedGame();
    void refreshStatistics();
    void saveSettings();
    void exitApplication();

private:
    QTabWidget *tabs;
    QLabel *gamesLabel;
    QLabel *winsLabel;
    QLabel *lossesLabel;
    QLabel *drawsLabel;
    QLabel *accuracyLabel;
    QLabel *recommendationLabel;
    QLabel *profileSummary;
    QTableWidget *historyTable;
    QLineEdit *nameEdit;
    QComboBox *modeCombo;
    QComboBox *difficultyCombo;
    QComboBox *timeCombo;
    QComboBox *themeCombo;
    QComboBox *boardThemeCombo;
    QCheckBox *soundCheck;
    QCheckBox *animationsCheck;
    QCheckBox *hintsCheck;
    QPushButton *startButton;
    PerformanceChart *chart;

    void buildHomeTab();
    void buildHistoryTab();
    void buildProfileTab();
    void buildSettingsTab();
    void updateDifficultyOptions();
    void playUiSound();
    void applyTheme(const QString &theme);
};

#endif
