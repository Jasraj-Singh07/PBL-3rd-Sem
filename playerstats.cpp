
#include "playerstats.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QList>

struct GameRecord
{
    QString date;
    QString mode;
    QString difficulty;
    QString result;
    bool demo;
};

static QList<GameRecord> readRecords()
{
    QList<GameRecord> records;
    QFile file(PlayerStats::filePath());

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return records;

    QTextStream in(&file);

    if (!in.atEnd())
        in.readLine(); // Skip CSV header.

    while (!in.atEnd())
    {
        QStringList fields = in.readLine().split(',');

        if (fields.size() < 5)
            continue;

        GameRecord record;
        record.date = fields[0];
        record.mode = fields[1];
        record.difficulty = fields[2];
        record.result = fields[3];
        record.demo = fields[4].trimmed() == "DEMO";

        records.append(record);
    }

    return records;
}

QString PlayerStats::filePath()
{
    return QDir(
        QCoreApplication::applicationDirPath()
    ).filePath("game_history.csv");
}

void PlayerStats::ensureDummyData()
{
    QFile file(filePath());

    // Never overwrite an existing history.
    if (file.exists())
        return;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    out << "date,mode,difficulty,result,type\n";
    out << "2026-09-20,Human vs AI,Easy,WIN,DEMO\n";
    out << "2026-09-21,Human vs AI,Easy,WIN,DEMO\n";
    out << "2026-09-22,Human vs AI,Medium,WIN,DEMO\n";
    out << "2026-09-23,Human vs AI,Medium,LOSS,DEMO\n";
    out << "2026-09-24,Human vs AI,Hard,LOSS,DEMO\n";
    out << "2026-09-25,Human vs Human,Not applicable,DRAW,DEMO\n";
}

bool PlayerStats::addGame(
    const QString &mode,
    const QString &difficulty,
    const QString &result)
{
    QFile file(filePath());

    bool needsHeader = !file.exists() || file.size() == 0;

    if (!file.open(
            QIODevice::WriteOnly |
            QIODevice::Append |
            QIODevice::Text))
        return false;

    QTextStream out(&file);

    if (needsHeader)
        out << "date,mode,difficulty,result,type\n";

    out << QDateTime::currentDateTime().toString(
        "yyyy-MM-dd hh:mm:ss"
    ) << ',' << mode << ',' << difficulty
      << ',' << result << ",REAL\n";

    return true;
}

int PlayerStats::totalGames()
{
    return readRecords().size();
}

int PlayerStats::wins()
{
    int n = 0;
    for (const auto &r : readRecords())
        if (r.result == "WIN") ++n;
    return n;
}

int PlayerStats::losses()
{
    int n = 0;
    for (const auto &r : readRecords())
        if (r.result == "LOSS") ++n;
    return n;
}

int PlayerStats::draws()
{
    int n = 0;
    for (const auto &r : readRecords())
        if (r.result == "DRAW") ++n;
    return n;
}

int PlayerStats::aiGames()
{
    int n = 0;
    for (const auto &r : readRecords())
        if (r.mode == "Human vs AI") ++n;
    return n;
}

int PlayerStats::aiWins()
{
    int n = 0;
    for (const auto &r : readRecords())
        if (r.mode == "Human vs AI" && r.result == "WIN") ++n;
    return n;
}

int PlayerStats::aiLosses()
{
    int n = 0;
    for (const auto &r : readRecords())
        if (r.mode == "Human vs AI" && r.result == "LOSS") ++n;
    return n;
}

int PlayerStats::aiDraws()
{
    int n = 0;
    for (const auto &r : readRecords())
        if (r.mode == "Human vs AI" && r.result == "DRAW") ++n;
    return n;
}

QString PlayerStats::recommendedDifficulty()
{
    int games = aiGames();

    if (games < 3)
        return "Easy";

    double score =
        (aiWins() + 0.5 * aiDraws()) / games;

    if (score >= 0.70) return "Hard";
    if (score >= 0.40) return "Medium";

    return "Easy";
}

QString PlayerStats::recommendationReason()
{
    int games = aiGames();

    if (games < 3)
        return "Play 3 AI games to get a more reliable recommendation.";

    double score =
        (aiWins() + 0.5 * aiDraws()) / games;

    if (score >= 0.70)
        return "You are winning often. Try a stronger opponent.";

    if (score >= 0.40)
        return "Your results are balanced. Medium should be a good challenge.";

    return "Use Easy to practise before increasing difficulty.";
}