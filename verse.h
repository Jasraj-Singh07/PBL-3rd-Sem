#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QSoundEffect>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QVariantMap>

#include "ai.h"

class Verse : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList pieces READ pieces NOTIFY changed)
    Q_PROPERTY(QString status MEMBER message NOTIFY changed)
    Q_PROPERTY(QStringList log READ log NOTIFY changed)
    Q_PROPERTY(QString difficulty READ difficulty NOTIFY changed)
    Q_PROPERTY(int selected MEMBER selection NOTIFY changed)
    Q_PROPERTY(int hint MEMBER hintSquare NOTIFY changed)
    Q_PROPERTY(int moves READ moves NOTIFY changed)
    Q_PROPERTY(int evaluation READ evaluation NOTIFY changed)
    Q_PROPERTY(QString mode MEMBER mode NOTIFY changed)
    Q_PROPERTY(QString player MEMBER player NOTIFY changed)
    Q_PROPERTY(QString opponent MEMBER opponent NOTIFY changed)
    Q_PROPERTY(QString whitePlayer MEMBER whitePlayer NOTIFY changed)
    Q_PROPERTY(QString blackPlayer MEMBER blackPlayer NOTIFY changed)
    Q_PROPERTY(QString matchDifficulty MEMBER matchDifficulty NOTIFY changed)
    Q_PROPERTY(QVariantMap whiteStats READ whiteStats NOTIFY changed)
    Q_PROPERTY(QVariantMap blackStats READ blackStats NOTIFY changed)
    Q_PROPERTY(QVariantList records READ records NOTIFY changed)
    Q_PROPERTY(QVariantMap recap READ recap NOTIFY changed)
    Q_PROPERTY(QVariantList examples READ examples CONSTANT)
    Q_PROPERTY(QString rivalMemory READ rivalMemory NOTIFY changed)
    Q_PROPERTY(QVariantMap profileStats READ profileStats NOTIFY changed)
    Q_PROPERTY(QUrl avatar READ avatar NOTIFY changed)
    Q_PROPERTY(QString lesson MEMBER lesson NOTIFY changed)
    Q_PROPERTY(int rushScore MEMBER rushScore NOTIFY changed)
    Q_PROPERTY(int localRating READ localRating NOTIFY changed)
    Q_PROPERTY(int rushBest READ rushBest NOTIFY changed)
    Q_PROPERTY(bool rushing MEMBER rushing NOTIFY changed)
    Q_PROPERTY(int rushSeconds MEMBER rushSeconds NOTIFY changed)
    Q_PROPERTY(bool ended MEMBER ended NOTIFY changed)
    Q_PROPERTY(bool busy MEMBER busy NOTIFY changed)
    Q_PROPERTY(bool sound MEMBER sound NOTIFY changed)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY changed)
    Q_PROPERTY(bool canSave READ canSave NOTIFY changed)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY changed)
    Q_PROPERTY(bool hasSave READ hasSave NOTIFY changed)
    Q_PROPERTY(bool replaying MEMBER replaying NOTIFY changed)
    Q_PROPERTY(int replaySize READ replaySize NOTIFY changed)
    Q_PROPERTY(int promotionSquare MEMBER promotionSquare NOTIFY changed)

public:
    explicit Verse(QObject *parent = nullptr);
    QVariantList pieces() const;
    QStringList log() const;
    QString difficulty() const;
    int moves() const { return game.getMoveCount(); }
    int evaluation() const { return -ChessAI::positionValue(game); }
    QVariantMap whiteStats() const { return sideStats(entries, 0); }
    QVariantMap blackStats() const { return sideStats(entries, 1); }
    QVariantList records() const;
    QVariantList examples() const;
    QVariantMap recap() const;
    Q_INVOKABLE QVariantMap recapFor(const QString &id) const;
    QString rivalMemory() const;
    QVariantMap profileStats() const;
    QUrl avatar() const;
    int rushBest() const;
    int localRating() const;
    bool hasSave() const;
    bool canUndo() const { return canChangeMoves() && !entries.isEmpty(); }
    bool canRedo() const { return canChangeMoves() && !undone.isEmpty(); }
    bool canSave() const { return canUndo() || canRedo(); }
    int replaySize() const { return replayEntries.size(); }

    Q_INVOKABLE void start(const QString &newMode);
    Q_INVOKABLE void click(int square);
    Q_INVOKABLE void promote(int type);
    Q_INVOKABLE void cancelPromotion();
    Q_INVOKABLE void step();
    Q_INVOKABLE void suggest();
    Q_INVOKABLE bool save();
    Q_INVOKABLE bool resume();
    Q_INVOKABLE void demo();
    Q_INVOKABLE void practice(int puzzle);
    Q_INVOKABLE void startRush();
    Q_INVOKABLE bool setAvatar(const QUrl &source);
    Q_INVOKABLE void surrender();
    Q_INVOKABLE void draw();
    Q_INVOKABLE void chat(const QString &text);
    Q_INVOKABLE void profile(const QString &name, const QString &guest);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE bool legal(int square) const;
    Q_INVOKABLE bool threat(int square) const;
    Q_INVOKABLE bool openReplay(const QString &id);
    Q_INVOKABLE void seek(int ply);
    Q_INVOKABLE void closeReplay();
    Q_INVOKABLE void setSound(bool enabled);
    Q_INVOKABLE void buttonSound();

signals:
    void changed();
    void matchFinished();
    void pieceCaptured();

private:
    ChessGame game;
    QString mode = "Classic";
    QString player = "Guest";
    QString opponent = "Player 2";
    QString whitePlayer;
    QString blackPlayer;
    QString matchId;
    QString matchDifficulty;
    QString message;
    QString result;
    QStringList comments;
    QVariantList entries;
    QVariantList undone;
    QVariantList replayEntries;
    QVariantMap replayMatch;
    QVariantMap interruptedMatch;
    int selection = -1;
    int hintSquare = -1;
    int promotionSquare = -1;
    bool ended = false;
    bool busy = false;
    bool sound = true;
    bool replaying = false;
    bool training = false;
    bool rushing = false;
    int puzzle = -1;
    int rushScore = 0;
    int rushSeconds = 0;
    QString lesson;
    QTimer rushTimer;
    unsigned revision = 0;
    QTimer reply;
    QElapsedTimer clock;
    QSoundEffect buttonEffect;
    QSoundEffect moveEffect;
    QSoundEffect captureEffect;

    bool versusAI() const { return mode == "Adaptive" || mode == "Nemesis"; }
    bool canChangeMoves() const
    {
        return !busy && !ended && !replaying && puzzle < 0 && promotionSquare < 0;
    }
    bool humanCanMove() const;
    void queueReply();
    void loadPuzzle(int index);
    void solve(int from, int to, PieceType promotion);
    void finishRush();
    void play(int from, int to, PieceType promotion);
    void updatePosition();
    void finish(const QString &newResult, const QString &reason);
    void rememberResult();
    QVariantMap snapshot() const;
    bool restore(const QVariantMap &match, int ply);
    static QVariantMap summarize(const QVariantMap &match);
    static QVariantMap sideStats(const QVariantList &moves, int side);
};
