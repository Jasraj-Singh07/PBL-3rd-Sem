#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H

#include <QString>
#include <QList>

struct GameRecord
{
    QString date;
    QString mode;
    QString difficulty;
    QString result;
    QString type;          // DEMO or REAL
    double accuracy = 0.0;  // estimated game-level metric for now
    double averageMoveTime = 0.0;
    int blunders = 0;
    int moves = 0;
    int durationSeconds = 0;
};

class PlayerStats
{
public:
    static QString filePath();
    static void ensureDummyData();

    static bool addGame(const QString &mode,
                        const QString &difficulty,
                        const QString &result);

    static bool addGame(const GameRecord &record);
    static QList<GameRecord> history();
    static int totalGames();
    static int wins();
    static int losses();
    static int draws();
    static int aiGames();
    static int aiWins();
    static int aiLosses();
    static int aiDraws();

    static double averageAccuracy();
    static double averageMoveTime();
    static int totalBlunders();
    static QString recommendedDifficulty();
    static QString recommendationReason();

    static QString playerName();
    static void setPlayerName(const QString &name);
};

#endif
