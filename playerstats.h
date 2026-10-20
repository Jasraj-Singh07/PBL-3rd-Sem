
#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H

#include <QString>

class PlayerStats
{
public:
    static QString filePath();
    static void ensureDummyData();

    static bool addGame(
        const QString &mode,
        const QString &difficulty,
        const QString &result
    );

    static int totalGames();
    static int wins();
    static int losses();
    static int draws();

    static int aiGames();
    static int aiWins();
    static int aiLosses();
    static int aiDraws();

    static QString recommendedDifficulty();
    static QString recommendationReason();
};

#endif