#include "playerstats.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QStringList>
#include <QFileInfo>
#include <algorithm>

static QString csvEscape(QString value)
{
    value.replace("\"", "\"\"");
    if (value.contains(',') || value.contains('"') ||
        value.contains('\n') || value.contains('\r'))
        value = "\"" + value + "\"";
    return value;
}

static QStringList parseCsvLine(const QString &line)
{
    QStringList fields;
    QString current;
    bool quoted = false;
    for (int i = 0; i < line.size(); ++i)
    {
        QChar ch = line[i];
        if (ch == '"')
        {
            if (quoted && i + 1 < line.size() && line[i + 1] == '"')
            {
                current += '"';
                ++i;
            }
            else quoted = !quoted;
        }
        else if (ch == ',' && !quoted)
        {
            fields << current;
            current.clear();
        }
        else current += ch;
    }
    fields << current;
    return fields;
}

QString PlayerStats::filePath()
{
    // Keep the same location used by the Phase 2 version so existing
    // player history is continued rather than silently starting over.
    return QDir(QCoreApplication::applicationDirPath())
        .filePath("game_history.csv");
}

QList<GameRecord> PlayerStats::history()
{
    QList<GameRecord> records;
    QFile file(filePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return records;

    QTextStream in(&file);
    if (in.atEnd()) return records;
    QString header = in.readLine();
    Q_UNUSED(header);

    while (!in.atEnd())
    {
        QStringList f = parseCsvLine(in.readLine());
        if (f.size() < 5) continue;

        GameRecord r;
        r.date = f.value(0);
        r.mode = f.value(1);
        r.difficulty = f.value(2);
        r.result = f.value(3);
        r.type = f.value(4);
        r.accuracy = f.value(5).toDouble();
        r.averageMoveTime = f.value(6).toDouble();
        r.blunders = f.value(7).toInt();
        r.moves = f.value(8).toInt();
        r.durationSeconds = f.value(9).toInt();
        records.append(r);
    }
    return records;
}

void PlayerStats::ensureDummyData()
{
    QFile file(filePath());
    if (file.exists() && file.size() > 0)
        return; // Never overwrite the user's saved history.

    QList<GameRecord> demo;
    const QString dates[] = {
        "2026-10-20 18:12", "2026-10-21 17:40", "2026-10-22 19:05",
        "2026-10-23 16:22", "2026-10-24 20:10", "2026-10-25 18:35",
        "2026-10-26 17:18", "2026-10-27 19:44", "2026-10-28 18:09",
        "2026-10-29 20:30", "2026-10-30 16:50", "2026-10-31 18:00",
        "2026-11-01 19:15", "2026-11-02 17:32", "2026-11-03 20:05"
    };
    const QString results[] = {
        "WIN","LOSS","WIN","DRAW","LOSS","WIN","WIN","LOSS",
        "WIN","DRAW","WIN","LOSS","WIN","WIN","DRAW"
    };
    const double accuracy[] = {
        84, 61, 89, 73, 58, 91, 86, 64, 93, 76, 88, 67, 95, 90, 79
    };
    const double moveTime[] = {
        8.2, 12.4, 7.6, 10.1, 14.8, 6.9, 8.0, 13.2,
        6.4, 9.7, 7.1, 11.9, 5.8, 6.6, 9.2
    };
    const int blunders[] = {1,4,0,2,5,0,1,3,0,2,1,3,0,0,1};

    for (int i = 0; i < 15; ++i)
    {
        GameRecord r;
        r.date = dates[i];
        r.mode = (i == 3 || i == 9) ? "Human vs Human" : "Human vs AI";
        r.difficulty = r.mode == "Human vs AI" ?
                       (i < 5 ? "Easy" : (i < 10 ? "Medium" : "Hard")) :
                       "Not applicable";
        r.result = results[i];
        r.type = "DEMO";
        r.accuracy = accuracy[i];
        r.averageMoveTime = moveTime[i];
        r.blunders = blunders[i];
        r.moves = 24 + (i * 7) % 35;
        r.durationSeconds = 180 + (i * 53) % 500;
        demo.append(r);
    }
    for (const GameRecord &r : demo)
        addGame(r);
}

bool PlayerStats::addGame(const QString &mode,
                          const QString &difficulty,
                          const QString &result)
{
    GameRecord r;
    r.date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    r.mode = mode;
    r.difficulty = difficulty;
    r.result = result;
    r.type = "REAL";
    // These are provisional estimates until per-move engine evaluation is added.
    r.accuracy = result == "WIN" ? 82.0 : (result == "DRAW" ? 74.0 : 66.0);
    r.averageMoveTime = 8.0;
    r.blunders = result == "LOSS" ? 2 : 1;
    return addGame(r);
}

bool PlayerStats::addGame(const GameRecord &record)
{
    QFile file(filePath());
    bool headerNeeded = !file.exists() || file.size() == 0;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return false;

    QTextStream out(&file);
    if (headerNeeded)
        out << "date,mode,difficulty,result,type,accuracy,avg_move_time,blunders,moves,duration_seconds\n";

    out << csvEscape(record.date) << ','
        << csvEscape(record.mode) << ','
        << csvEscape(record.difficulty) << ','
        << csvEscape(record.result) << ','
        << csvEscape(record.type) << ','
        << record.accuracy << ','
        << record.averageMoveTime << ','
        << record.blunders << ','
        << record.moves << ','
        << record.durationSeconds << '\n';
    return true;
}

static int countResult(const QString &result, bool aiOnly = false)
{
    int count = 0;
    for (const GameRecord &r : PlayerStats::history())
        if (r.result == result && (!aiOnly || r.mode == "Human vs AI"))
            ++count;
    return count;
}

int PlayerStats::totalGames() { return history().size(); }
int PlayerStats::wins() { return countResult("WIN"); }
int PlayerStats::losses() { return countResult("LOSS"); }
int PlayerStats::draws() { return countResult("DRAW"); }
int PlayerStats::aiGames()
{
    int n = 0;
    for (const GameRecord &r : history()) if (r.mode == "Human vs AI") ++n;
    return n;
}
int PlayerStats::aiWins() { return countResult("WIN", true); }
int PlayerStats::aiLosses() { return countResult("LOSS", true); }
int PlayerStats::aiDraws() { return countResult("DRAW", true); }

double PlayerStats::averageAccuracy()
{
    QList<GameRecord> h = history();
    if (h.isEmpty()) return 0;
    double sum = 0;
    for (const GameRecord &r : h) sum += r.accuracy;
    return sum / h.size();
}

double PlayerStats::averageMoveTime()
{
    QList<GameRecord> h = history();
    if (h.isEmpty()) return 0;
    double sum = 0;
    for (const GameRecord &r : h) sum += r.averageMoveTime;
    return sum / h.size();
}

int PlayerStats::totalBlunders()
{
    int sum = 0;
    for (const GameRecord &r : history()) sum += r.blunders;
    return sum;
}

QString PlayerStats::recommendedDifficulty()
{
    QList<GameRecord> h = history();
    QList<GameRecord> ai;
    for (const GameRecord &r : h)
        if (r.mode == "Human vs AI" && r.type == "REAL") ai.append(r);

    // Real data takes priority. Demo data provides a starter recommendation.
    if (ai.isEmpty())
        for (const GameRecord &r : h)
            if (r.mode == "Human vs AI") ai.append(r);

    if (ai.isEmpty()) return "Easy";

    int start = qMax(0, ai.size() - 8);
    double accuracy = 0, avgTime = 0, blunders = 0, score = 0;
    int n = 0;
    for (int i = start; i < ai.size(); ++i)
    {
        const GameRecord &r = ai[i];
        accuracy += r.accuracy;
        avgTime += r.averageMoveTime;
        blunders += r.blunders;
        score += r.result == "WIN" ? 1.0 : (r.result == "DRAW" ? 0.5 : 0.0);
        ++n;
    }
    accuracy /= n;
    avgTime /= n;
    blunders /= n;
    score /= n;

    if (accuracy >= 85 && blunders <= 1.0 && score >= 0.60)
        return "Hard";
    if (accuracy >= 68 && blunders <= 3.0 && avgTime <= 15.0)
        return "Medium";
    return "Easy";
}

QString PlayerStats::recommendationReason()
{
    return QString("Based on recent games: %1% average accuracy, %2s average move time and %3 total blunders.")
        .arg(averageAccuracy(), 0, 'f', 1)
        .arg(averageMoveTime(), 0, 'f', 1)
        .arg(totalBlunders());
}

QString PlayerStats::playerName()
{
    QSettings settings("ChessPBL", "ChessArena");
    return settings.value("profile/name", "Player One").toString();
}

void PlayerStats::setPlayerName(const QString &name)
{
    QSettings settings("ChessPBL", "ChessArena");
    settings.setValue("profile/name", name.trimmed().isEmpty() ? "Player One" : name.trimmed());
}
