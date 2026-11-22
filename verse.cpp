#include "verse.h"

#include <QFutureWatcher>
#include <QSettings>
#include <QUuid>
#include <QtConcurrentRun>
#include <algorithm>
#include <cmath>
#include <utility>

namespace
{
QString squareName(int square)
{
    return QString(QChar('a' + square % 8)) + QString::number(8 - square / 8);
}
}

Verse::Verse(QObject *parent) : QObject(parent)
{
    QSettings settings;
    player = settings.value("identity/player", player).toString();
    opponent = settings.value("identity/opponent", opponent).toString();
    sound = settings.value("preferences/sound", true).toBool();
    buttonEffect.setSource(QUrl("qrc:/audio/button.wav"));
    moveEffect.setSource(QUrl("qrc:/audio/move.wav"));
    captureEffect.setSource(QUrl("qrc:/audio/capture.wav"));
    for (auto effect : {&buttonEffect, &moveEffect, &captureEffect})
        effect->setVolume(0.45f);
    rushTimer.setInterval(1000);
    connect(&rushTimer, &QTimer::timeout, this, [this]
    {
        if (--rushSeconds <= 0)
            finishRush();
        else
            emit changed();
    });
    reply.setSingleShot(true);
    connect(&reply, &QTimer::timeout, this, &Verse::step);
    start("Classic");
}

QVariantList Verse::pieces() const
{
    QVariantList squares;
    for (int square = 0; square < 64; ++square)
    {
        const Piece piece = game.getPiece(square / 8, square % 8);
        squares.append(QVariantMap{{"square", square}, {"type", int(piece.type)},
            {"white", piece.color == PieceColor::White},
            {"symbol", game.getPieceSymbol(square / 8, square % 8)}});
    }
    return squares;
}

QStringList Verse::log() const
{
    QStringList lines;
    for (const auto &entry : entries)
    {
        const auto move = entry.toMap();
        QString line = squareName(move.value("from").toInt()) + " → " +
            squareName(move.value("to").toInt());
        if (move.contains("quality"))
            line += QString(" · %1% · %2 s")
                .arg(move.value("quality").toDouble(), 0, 'f', 0)
                .arg(move.value("seconds").toDouble(), 0, 'f', 1);
        lines.append(line);
        if (move.contains("reaction"))
            lines.append(move.value("reaction").toString());
    }
    lines.append(comments);
    return lines;
}

bool Verse::humanCanMove() const
{
    return !ended && !busy && !replaying && mode != "Spectator" &&
           !(versusAI() && game.getCurrentTurn() == PieceColor::Black);
}

bool Verse::legal(int square) const
{
    return humanCanMove() && selection >= 0 && square >= 0 && square < 64 &&
           game.isValidMove(selection / 8, selection % 8, square / 8, square % 8);
}

bool Verse::threat(int square) const
{
    const auto enemy = game.getCurrentTurn() == PieceColor::White
        ? PieceColor::Black : PieceColor::White;
    return game.attacked(square / 8, square % 8, enemy);
}

void Verse::start(const QString &newMode)
{
    ++revision;
    reply.stop();
    rushTimer.stop();
    rushing = false;
    rushScore = rushSeconds = 0;
    puzzle = -1;
    lesson.clear();
    replayMatch.clear();
    replayEntries.clear();
    interruptedMatch.clear();
    game.initializeBoard();
    mode = newMode;
    matchDifficulty = difficulty();
    matchId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    whitePlayer = mode == "Spectator" ? QString("White AI") : player;
    blackPlayer = mode == "Classic" ? opponent : mode == "Nemesis" ? QString("Nemesis") : QString("Black AI");
    entries.clear();
    undone.clear();
    comments.clear();
    if (mode == "Nemesis")
        comments.append(rivalMemory());
    result.clear();
    selection = hintSquare = promotionSquare = -1;
    ended = busy = replaying = training = false;
    clock.start();
    updatePosition();
}

void Verse::updatePosition()
{
    const auto side = game.getCurrentTurn();
    const bool white = side == PieceColor::White;
    message = white ? "White to move" : "Black to move";
    if (game.isCheckmate(side))
        finish(white ? "0-1" : "1-0", white ? "Black wins by checkmate" : "White wins by checkmate");
    else if (game.isStalemate(side))
        finish("½-½", "Stalemate");
    else if (const QString reason = game.drawReason(); !reason.isEmpty())
        finish("½-½", reason);
    else
    {
        if (game.isKingInCheck(side))
            message += " · Check";
        emit changed();
    }
}

void Verse::click(int square)
{
    if (!humanCanMove() || promotionSquare >= 0 || square < 0 || square >= 64)
        return;

    if (legal(square))
    {
        if (game.getPiece(selection / 8, selection % 8).type == PieceType::Pawn &&
            (square / 8 == 0 || square / 8 == 7))
        {
            promotionSquare = square;
            emit changed();
        }
        else
            play(selection, square, PieceType::Queen);
        return;
    }

    selection = game.getPiece(square / 8, square % 8).color == game.getCurrentTurn() ? square : -1;
    hintSquare = -1;
    emit changed();
}

void Verse::promote(int type)
{
    if (promotionSquare >= 0 && type >= int(PieceType::Queen) && type <= int(PieceType::Knight))
        play(selection, promotionSquare, static_cast<PieceType>(type));
}

void Verse::cancelPromotion()
{
    promotionSquare = -1;
    emit changed();
}

void Verse::play(int from, int to, PieceType promotion)
{
    if (puzzle >= 0)
    {
        solve(from, to, promotion);
        return;
    }

    const ChessGame previous = game;
    const Piece piece = previous.getPiece(from / 8, from % 8);
    const bool capture = !previous.getPiece(to / 8, to % 8).isEmpty() ||
        (piece.type == PieceType::Pawn && from % 8 != to % 8);
    const double seconds = clock.elapsed() / 1000.0;
    if (!game.movePiece(from / 8, from % 8, to / 8, to % 8, promotion))
        return;

    selection = hintSquare = promotionSquare = -1;
    undone.clear();
    if (sound)
        (capture ? captureEffect : moveEffect).play();
    busy = true;
    auto *analysis = new QFutureWatcher<int>(this);
    connect(analysis, &QFutureWatcher<int>::finished, this,
        [this, analysis, currentRevision = revision, from, to, promotion, seconds]
        {
            const int loss = analysis->result();
            analysis->deleteLater();
            if (currentRevision != revision)
                return;
            QVariantMap entry{{"from", from}, {"to", to}, {"promotion", int(promotion)},
                {"quality", 100 * std::exp(-loss / 400.0)}, {"seconds", seconds}, {"blunder", loss >= 200}};
            if (mode == "Nemesis" && entries.size() % 2 == 0)
                entry["reaction"] = QString(loss >= 200 ? "Nemesis: I'll take that." : "Nemesis: Your move made me think.");
            entries.append(entry);
            busy = false;
            clock.restart();
            updatePosition();
            queueReply();
        });
    analysis->setFuture(QtConcurrent::run([previous, played = game] { return ChessAI::moveLoss(previous, played); }));
    emit changed();
    if (capture)
        emit pieceCaptured();
}

void Verse::queueReply()
{
    if (versusAI() && !ended && !replaying && game.getCurrentTurn() == PieceColor::Black)
        reply.start(240);
}

void Verse::step()
{
    if (busy || ended || replaying ||
        (mode != "Spectator" && !(versusAI() && game.getCurrentTurn() == PieceColor::Black)))
        return;
    busy = true;
    auto *search = new QFutureWatcher<Move>(this);
    connect(search, &QFutureWatcher<Move>::finished, this, [this, search, currentRevision = revision]
    {
        const Move move = search->result();
        search->deleteLater();
        if (currentRevision != revision)
            return;
        busy = false;
        if (move.fromRow >= 0)
            play(move.fromRow * 8 + move.fromCol, move.toRow * 8 + move.toCol, move.promotion);
        else
            updatePosition();
    });
    search->setFuture(QtConcurrent::run([position = game, difficulty = matchDifficulty]
    {
        Move move;
        ChessAI::chooseMove(position, difficulty, move);
        return move;
    }));
    emit changed();
}

void Verse::suggest()
{
    if (!humanCanMove() || promotionSquare >= 0 || rushing)
        return;
    busy = true;
    auto *search = new QFutureWatcher<Move>(this);
    connect(search, &QFutureWatcher<Move>::finished, this, [this, search, currentRevision = revision]
    {
        const Move move = search->result();
        search->deleteLater();
        if (currentRevision != revision)
            return;
        busy = false;
        if (move.fromRow >= 0)
        {
            selection = move.fromRow * 8 + move.fromCol;
            hintSquare = move.toRow * 8 + move.toCol;
        }
        emit changed();
    });
    search->setFuture(QtConcurrent::run([position = game]
    {
        Move move;
        ChessAI::chooseMove(position, "Hard", move);
        return move;
    }));
    emit changed();
}

void Verse::undo()
{
    if (!canUndo())
        return;
    reply.stop();
    do
    {
        game.undoMove();
        undone.append(entries.takeLast());
    } while (versusAI() && game.getCurrentTurn() == PieceColor::Black && !entries.isEmpty());
    selection = hintSquare = promotionSquare = -1;
    clock.restart();
    updatePosition();
}

void Verse::redo()
{
    if (!canRedo())
        return;
    do
    {
        game.redoMove();
        entries.append(undone.takeLast());
    } while (versusAI() && game.getCurrentTurn() == PieceColor::Black && !undone.isEmpty());
    selection = hintSquare = promotionSquare = -1;
    clock.restart();
    updatePosition();
    queueReply();
}

void Verse::surrender()
{
    if (puzzle < 0 && humanCanMove())
    {
        const bool white = game.getCurrentTurn() == PieceColor::White;
        finish(white ? "0-1" : "1-0", white ? "White resigned" : "Black resigned");
    }
}

void Verse::draw()
{
    if (puzzle >= 0 || !humanCanMove())
        return;
    if (versusAI() && evaluation() < -100)
        chat("AI declined the draw.");
    else
        finish("½-½", "Draw by agreement");
}

void Verse::chat(const QString &text)
{
    if (!text.trimmed().isEmpty())
    {
        comments.append(player + ": " + text.trimmed().left(300));
        emit changed();
    }
}

void Verse::demo()
{
    start("Classic");
    training = true;
    for (auto [from, to] : {std::pair{52, 36}, {12, 28}, {62, 45}, {1, 18}, {61, 34}, {6, 21}})
    {
        game.movePiece(from / 8, from % 8, to / 8, to % 8);
        entries.append(QVariantMap{{"from", from}, {"to", to}, {"promotion", int(PieceType::Queen)}});
    }
    updatePosition();
}

void Verse::finish(const QString &newResult, const QString &reason)
{
    ended = true;
    message = reason;
    result = newResult;
    reply.stop();
    rememberResult();
    emit changed();
    emit matchFinished();
}

void Verse::setSound(bool enabled)
{
    sound = enabled;
    QSettings().setValue("preferences/sound", enabled);
    emit changed();
}

void Verse::buttonSound()
{
    if (sound)
        buttonEffect.play();
}
