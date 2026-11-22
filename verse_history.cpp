#include "verse.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QHash>
#include <QSaveFile>
#include <QStandardPaths>
#include <QSettings>
#include <QUuid>
#include <algorithm>
#include <cmath>
#include <utility>

namespace
{
QString profileKey(const QString &name)
{
    return QString::fromLatin1(name.toCaseFolded().toUtf8().toHex());
}

bool validMode(const QString &mode)
{
    return mode == "Classic" || mode == "Adaptive" || mode == "Nemesis" || mode == "Spectator";
}
}

QVariantMap Verse::sideStats(const QVariantList &line, int side)
{
    QVariantList accuracy;
    QVariantList sense;
    double qualitySum = 0;
    double secondsSum = 0;
    int blunders = 0;
    for (int index = side; index < line.size(); index += 2)
    {
        const auto move = line.at(index).toMap();
        if (!move.contains("quality"))
            continue;
        qualitySum += move.value("quality").toDouble();
        secondsSum += move.value("seconds").toDouble();
        blunders += move.value("blunder").toBool();
        const double average = qualitySum / (accuracy.size() + 1);
        accuracy.append(average);
        sense.append(0.75 * average + 25.0 * (1.0 - double(blunders) / accuracy.size()));
    }
    const int count = int(accuracy.size());
    return {{"moves", count}, {"accuracy", count ? qualitySum / count : 0},
        {"seconds", count ? secondsSum / count : 0}, {"blunders", blunders},
        {"sense", sense.isEmpty() ? 0.0 : sense.last().toDouble()},
        {"accuracySeries", accuracy}, {"senseSeries", sense}};
}

QVariantList Verse::records() const
{
    QVariantList matches;
    const auto stored = QSettings().value("history/matches").toList();
    for (const auto &item : stored)
    {
        auto match = item.toMap();
        const bool white = match.value("whiteHuman").toBool() &&
            profileKey(match.value("white").toString()) == profileKey(player);
        const bool black = match.value("blackHuman").toBool() &&
            profileKey(match.value("black").toString()) == profileKey(player);
        if (!white && !black)
            continue;
        const QString outcome = match.value("result").toString();
        match["outcome"] = outcome == "½-½" ? "Draw" :
            ((white ? outcome == "1-0" : outcome == "0-1") ? "Win" : "Loss");
        match["stats"] = sideStats(match.value("entries").toList(), white ? 0 : 1);
        matches.append(match);
    }
    return matches;
}

QVariantList Verse::examples() const
{
    struct Sample
    {
        QString name;
        QString mode;
        QString outcome;
        QString reason;
        QString sequence;
    };
    const QList<Sample> samples{
        {"Meteor", "Classic", "0-1", "Black wins by checkmate", "f2f3 e7e5 g2g4 d8h4"},
        {"Nova", "Adaptive", "1-0", "White wins by checkmate", "e2e4 e7e5 f1c4 b8c6 d1h5 g8f6 h5f7"},
        {"Quasar", "Nemesis", "0-1", "Black wins by checkmate", "e2e4 e7e5 g1f3 b8c6 f1c4 c6d4 f3e5 d8g5 e5f7 g5g2 h1f1 g2e4 c4e2 d4f3"},
        {"Vega", "Classic", "½-½", "Threefold repetition", "g1f3 g8f6 f3g1 f6g8 g1f3 g8f6 f3g1 f6g8"},
        {"Orion", "Classic", "1-0", "Black resigned", "e2e4 e7e5 g1f3 b8c6 f1c4 g8f6 e1g1"},
        {"Cosmo", "Adaptive", "½-½", "Draw by agreement", "d2d4 d7d5 c2c4 e7e6 g1f3 g8f6"},
        {"Comet", "Classic", "½-½", "Draw by agreement", "e2e4 a7a6 e4e5 d7d5 e5d6 c7d6"},
        {"Atlas", "Nemesis", "0-1", "White resigned", "e2e4 c7c5 g1f3 d7d6 d2d4 c5d4 f3d4 g8f6"},
        {"Pulsar", "Classic", "½-½", "Draw by agreement", "c2c4 e7e5 b1c3 g8f6 g2g3 f8b4"},
        {"Luna", "Classic", "1-0", "Black resigned", "d2d4 g8f6 c2c4 g7g6 b1c3 f8g7 e2e4"},
        {"Pluto", "Adaptive", "½-½", "Draw by agreement", "e2e4 e7e6 d2d4 d7d5 b1c3 g8f6"},
        {"Sol", "Classic", "0-1", "White resigned", "e2e4 d7d5 e4d5 d8d5 b1c3 d5a5"}
    };
    QVariantList games;
    for (qsizetype index = 0; index < samples.size(); ++index)
    {
        const auto &sample = samples.at(index);
        QVariantList line;
        const auto square = [](const QString &move, int offset)
        {
            return (8 - move.at(offset + 1).digitValue()) * 8 + move.at(offset).unicode() - 'a';
        };
        for (const auto &move : sample.sequence.split(' '))
        {
            const int number = int(line.size());
            const int quality = (number + int(index)) % 6 == 2
                ? 31 + int(index) : 73 + (number * 11 + int(index) * 7) % 28;
            line.append(QVariantMap{{"from", square(move, 0)}, {"to", square(move, 2)},
                {"quality", quality}, {"seconds", 2.0 + ((number * 19 + int(index) * 11) % 123) / 10.0},
                {"blunder", quality < 60}});
        }
        games.append(QVariantMap{{"id", "demo-" + QString::number(index)}, {"white", "Demo " + sample.name},
            {"black", sample.mode == "Classic" ? QString("Demo Orbit") : QString("Demo AI")},
            {"mode", sample.mode}, {"date", QDate(2026, 11, 20).addDays(-index).toString(Qt::ISODate)},
            {"status", sample.reason}, {"result", sample.outcome}, {"outcome", "DEMO"},
            {"entries", line}, {"training", true}, {"stats", sideStats(line, 0)}});
    }
    return games;
}

QVariantMap Verse::recap() const
{
    return result.isEmpty() ? QVariantMap{} : summarize(snapshot());
}

QVariantMap Verse::recapFor(const QString &id) const
{
    auto matches = records();
    matches.append(examples());
    for (const auto &entry : matches)
    {
        const auto match = entry.toMap();
        if (match.value("id").toString() == id)
            return summarize(match);
    }
    return {};
}

QVariantMap Verse::summarize(const QVariantMap &match)
{
    const auto line = match.value("entries").toList();
    const auto white = sideStats(line, 0);
    const auto black = sideStats(line, 1);
    const QString outcome = match.value("result").toString();
    int weakest = -1;
    double lowest = 101;
    for (qsizetype ply = 0; ply < line.size(); ++ply)
    {
        const auto move = line.at(ply).toMap();
        if (move.contains("quality") && move.value("quality").toDouble() < lowest)
        {
            lowest = move.value("quality").toDouble();
            weakest = int(ply);
        }
    }
    const auto notation = [](int square)
    {
        return QString(QChar('a' + square % 8)) + QString::number(8 - square / 8);
    };
    QString moment;
    if (weakest >= 0)
    {
        const auto move = line.at(weakest).toMap();
        moment = QString("%1 played %2 → %3 on turn %4 (%5% estimated accuracy).")
            .arg(weakest % 2 ? "Black" : "White")
            .arg(notation(move.value("from").toInt()), notation(move.value("to").toInt()))
            .arg(weakest / 2 + 1).arg(lowest, 0, 'f', 0);
    }

    const int blunders = white.value("blunders").toInt() + black.value("blunders").toInt();
    const QString roast = line.isEmpty() ? "The pieces arrived. The plot did not." :
        outcome == "½-½" ? "Two players entered. The scoreboard chose diplomacy." :
        blunders >= 3 ? "That board had more plot twists than a season finale." :
        blunders ? "Somewhere, a chess coach just dropped their coffee." :
        "The board got cooked on low heat. Very professional.";
    return {{"status", match.value("status")}, {"roast", roast}, {"moment", moment},
        {"white", white}, {"black", black}, {"plies", int(line.size())},
        {"demo", match.value("outcome").toString() == "DEMO"}};
}

QString Verse::rivalMemory() const
{
    int wins = 0;
    int losses = 0;
    int draws = 0;
    for (const auto &record : records())
    {
        const auto match = record.toMap();
        if (match.value("mode").toString() != "Nemesis")
            continue;
        const auto outcome = match.value("outcome").toString();
        wins += outcome == "Win";
        losses += outcome == "Loss";
        draws += outcome == "Draw";
    }
    if (!(wins || losses || draws))
        return "Nemesis: First meeting.";
    return QString("Nemesis record  ·  %1 wins  /  %2 draws  /  %3 losses")
        .arg(wins).arg(draws).arg(losses);
}

QVariantMap Verse::profileStats() const
{
    const auto matches = records();
    int wins = 0;
    int draws = 0;
    int count = 0;
    int blunders = 0;
    double quality = 0;
    double seconds = 0;
    int recentGames = 0;
    int recentMoves = 0;
    int recentBlunders = 0;
    double recentQuality = 0;
    double recentSeconds = 0;
    for (const auto &entry : matches)
    {
        const auto match = entry.toMap();
        wins += match.value("outcome").toString() == "Win";
        draws += match.value("outcome").toString() == "Draw";
        const auto stats = match.value("stats").toMap();
        const int moves = stats.value("moves").toInt();
        const int errors = stats.value("blunders").toInt();
        count += moves;
        blunders += errors;
        quality += stats.value("accuracy").toDouble() * moves;
        seconds += stats.value("seconds").toDouble() * moves;
        if (moves && recentGames < 8)
        {
            ++recentGames;
            recentMoves += moves;
            recentBlunders += errors;
            recentQuality += stats.value("accuracy").toDouble() * moves;
            recentSeconds += stats.value("seconds").toDouble() * moves;
        }
    }

    double level = 65;
    if (recentMoves)
    {
        level = recentQuality / recentMoves - 45.0 * recentBlunders / recentMoves
                - 0.45 * std::clamp(recentSeconds / recentMoves - 8.0, 0.0, 30.0);
    }
    return {{"games", int(matches.size())}, {"wins", wins}, {"draws", draws},
        {"losses", int(matches.size()) - wins - draws}, {"ratedMoves", count},
        {"accuracy", count ? quality / count : 0}, {"seconds", count ? seconds / count : 0},
        {"blunders", blunders}, {"difficulty", QString(level >= 82 ? "Hard" : level >= 55 ? "Medium" : "Easy")}};
}

int Verse::localRating() const
{
    QHash<QString, double> ratings;
    const auto matches = QSettings().value("history/matches").toList();
    for (auto it = matches.crbegin(); it != matches.crend(); ++it)
    {
        const auto match = it->toMap();
        if (!match.value("whiteHuman").toBool() || !match.value("blackHuman").toBool())
            continue;
        const QString white = profileKey(match.value("white").toString());
        const QString black = profileKey(match.value("black").toString());
        if (white == black || match.value("entries").toList().isEmpty())
            continue;
        const double whiteRating = ratings.value(white, 1200);
        const double blackRating = ratings.value(black, 1200);
        const QString result = match.value("result").toString();
        const double actual = result == "1-0" ? 1.0 : result == "0-1" ? 0.0 : 0.5;
        const double expected = 1.0 / (1.0 + std::pow(10.0, (blackRating - whiteRating) / 400.0));
        const double change = 24.0 * (actual - expected);
        ratings[white] = whiteRating + change;
        ratings[black] = blackRating - change;
    }
    return int(std::lround(ratings.value(profileKey(player), 1200)));
}

QString Verse::difficulty() const
{
    return profileStats().value("difficulty").toString();
}

void Verse::profile(const QString &name, const QString &guest)
{
    const QString primary = name.simplified().left(40);
    const QString secondary = guest.simplified().left(40);
    if (primary.isEmpty() || secondary.isEmpty() || profileKey(primary) == profileKey(secondary))
        return;
    player = primary;
    opponent = secondary;
    QSettings settings;
    settings.setValue("identity/player", player);
    settings.setValue("identity/opponent", opponent);
    emit changed();
}

QUrl Verse::avatar() const
{
    const QString path = QSettings().value("avatars/" + profileKey(player)).toString();
    return QFileInfo::exists(path) ? QUrl::fromLocalFile(path) : QUrl();
}

bool Verse::setAvatar(const QUrl &source)
{
    if (!source.isLocalFile())
        return false;
    QImageReader reader(source.toLocalFile());
    reader.setAutoTransform(true);
    const QSize dimensions = reader.size();
    if (!dimensions.isValid() || qint64(dimensions.width()) * dimensions.height() > 32000000)
        return false;
    const QImage image = reader.read().scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    if (image.isNull())
        return false;
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/avatars";
    if (!QDir().mkpath(directory))
        return false;
    const QString path = directory + "/" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".png";
    QSaveFile output(path);
    if (!output.open(QIODevice::WriteOnly) || !image.save(&output, "PNG") || !output.commit())
        return false;
    QSettings settings;
    const QString key = "avatars/" + profileKey(player);
    const QString old = settings.value(key).toString();
    settings.setValue(key, path);
    settings.sync();
    if (settings.status() != QSettings::NoError)
        return false;
    if (QFileInfo(old).absolutePath() == directory)
        QFile::remove(old);
    emit changed();
    return true;
}

QVariantMap Verse::snapshot() const
{
    return {{"id", matchId}, {"mode", mode}, {"white", whitePlayer}, {"black", blackPlayer},
        {"whiteHuman", mode != "Spectator"}, {"blackHuman", mode == "Classic"},
        {"difficulty", matchDifficulty}, {"entries", entries}, {"redo", undone}, {"comments", comments},
        {"result", result}, {"status", message}, {"training", training},
        {"date", QDateTime::currentDateTime().toString(Qt::ISODate)}};
}

void Verse::rememberResult()
{
    if (replaying)
        return;
    QSettings settings;
    const QString session = "sessions/" + profileKey(mode == "Spectator" ? player : whitePlayer);
    if (settings.value(session).toMap().value("id").toString() == matchId)
        settings.remove(session);
    if (training || mode == "Spectator")
        return;
    auto matches = settings.value("history/matches").toList();
    for (const auto &entry : matches)
        if (entry.toMap().value("id").toString() == matchId)
            return;
    matches.prepend(snapshot());
    settings.setValue("history/matches", matches);
}

bool Verse::hasSave() const
{
    QSettings settings;
    return settings.contains("sessions/" + profileKey(player)) ||
           settings.contains("save/" + player + "/moves");
}

bool Verse::save()
{
    if (!canSave())
        return false;
    QSettings settings;
    settings.setValue("sessions/" + profileKey(mode == "Spectator" ? player : whitePlayer), snapshot());
    settings.sync();
    emit changed();
    return settings.status() == QSettings::NoError;
}

bool Verse::restore(const QVariantMap &match, int ply)
{
    const QString restoredMode = match.value("mode").toString();
    const auto line = match.value("entries").toList();
    if (!validMode(restoredMode) || ply < 0 || ply > line.size())
        return false;

    ChessGame reconstructed;
    const auto apply = [&reconstructed](const QVariant &entry)
    {
        const auto move = entry.toMap();
        const int from = move.value("from", -1).toInt();
        const int to = move.value("to", -1).toInt();
        const auto promotion = static_cast<PieceType>(move.value("promotion", int(PieceType::Queen)).toInt());
        return from >= 0 && from < 64 && to >= 0 && to < 64 &&
            reconstructed.movePiece(from / 8, from % 8, to / 8, to % 8, promotion);
    };
    for (int index = 0; index < ply; ++index)
        if (!apply(line.at(index)))
            return false;

    const auto redo = ply == line.size() ? match.value("redo").toList() : QVariantList{};
    for (auto it = redo.crbegin(); it != redo.crend(); ++it)
        if (!apply(*it))
            return false;
    for (qsizetype index = 0; index < redo.size(); ++index)
        reconstructed.undoMove();

    ++revision;
    reply.stop();
    rushTimer.stop();
    rushing = false;
    rushSeconds = 0;
    puzzle = -1;
    lesson.clear();
    game = std::move(reconstructed);
    mode = restoredMode;
    whitePlayer = match.value("white").toString();
    blackPlayer = match.value("black").toString();
    matchId = match.value("id").toString();
    matchDifficulty = match.value("difficulty", "Medium").toString();
    entries = line.mid(0, ply);
    comments = match.value("comments").toStringList();
    training = match.value("training").toBool();
    result = ply == line.size() ? match.value("result").toString() : QString();
    message = result.isEmpty()
        ? (game.getCurrentTurn() == PieceColor::White ? "White to move" : "Black to move")
        : match.value("status").toString();
    if (result.isEmpty() && game.isKingInCheck(game.getCurrentTurn()))
        message += " · Check";
    undone = redo;
    selection = hintSquare = promotionSquare = -1;
    ended = !result.isEmpty();
    busy = false;
    clock.restart();
    return true;
}

bool Verse::resume()
{
    QSettings settings;
    const QString key = "sessions/" + profileKey(player);
    auto match = settings.value(key).toMap();
    if (match.isEmpty())
    {
        // Earlier builds stored a coordinate-only move list by display name.
        const QString legacy = "save/" + player;
        if (!settings.contains(legacy + "/moves"))
            return false;
        QVariantList moves;
        for (const auto &encoded : settings.value(legacy + "/moves").toStringList())
        {
            const auto parts = encoded.split(',');
            bool fromValid = false;
            bool toValid = false;
            if (parts.size() != 2)
                return false;
            const int from = parts.at(0).toInt(&fromValid);
            const int to = parts.at(1).toInt(&toValid);
            if (!fromValid || !toValid)
                return false;
            moves.append(QVariantMap{{"from", from}, {"to", to}});
        }
        const QString oldMode = settings.value(legacy + "/mode", "Classic").toString();
        match = {{"id", QUuid::createUuid().toString(QUuid::WithoutBraces)}, {"mode", oldMode}, {"entries", moves},
            {"white", player}, {"black", oldMode == "Classic" ? opponent : QString("Black AI")},
            {"difficulty", difficulty()}, {"status", settings.value(legacy + "/message")},
            {"result", settings.value(legacy + "/ended").toBool() ? QString("Archived") : QString()}};
    }

    if (!restore(match, match.value("entries").toList().size()))
        return false;
    replaying = false;
    replayEntries.clear();
    replayMatch.clear();
    interruptedMatch.clear();
    if (!ended && game.isGameOver())
        updatePosition();
    else
        emit changed();
    queueReply();
    return true;
}

bool Verse::openReplay(const QString &id)
{
    if (busy || promotionSquare >= 0 || puzzle >= 0)
        return false;
    auto available = records();
    available.append(examples());
    for (const auto &record : available)
    {
        const auto match = record.toMap();
        if (match.value("id").toString() != id)
            continue;
        const auto previous = snapshot();
        if (!restore(match, 0))
            return false;
        if (!replaying)
            interruptedMatch = previous;
        replayMatch = match;
        replayEntries = match.value("entries").toList();
        replaying = true;
        comments.clear();
        emit changed();
        return true;
    }
    return false;
}

void Verse::seek(int ply)
{
    if (replaying && restore(replayMatch, ply))
    {
        comments.clear();
        emit changed();
    }
}

void Verse::closeReplay()
{
    if (!replaying || !restore(interruptedMatch, interruptedMatch.value("entries").toList().size()))
        return;
    replaying = false;
    interruptedMatch.clear();
    replayMatch.clear();
    replayEntries.clear();
    emit changed();
    queueReply();
}
