#include "verse.h"

#include <QSettings>
#include <array>
#include <utility>

namespace
{
struct Puzzle
{
    const char *title;
    const char *setup;
    const char *hint;
};

const std::array<Puzzle, 3> puzzles{{
    {"Fool's mate", "f2f3 e7e5 g2g4", "Black to move. Look for an open diagonal to the white king."},
    {"Scholar's mate", "e2e4 e7e5 f1c4 b8c6 d1h5 g8f6", "White to move. The bishop and queen attack the same pawn."},
    {"Smothered mate", "e2e4 e7e5 g1f3 b8c6 f1c4 c6d4 f3e5 d8g5 e5f7 g5g2 h1f1 g2e4 c4e2", "Black to move. The white king is boxed in by its own pieces."}
}};

int square(const QString &move, int offset)
{
    return (8 - move.at(offset + 1).digitValue()) * 8 + move.at(offset).unicode() - 'a';
}
}

void Verse::practice(int index)
{
    if (index < 0 || index >= int(puzzles.size()))
        return;
    start("Classic");
    training = true;
    loadPuzzle(index);
}

void Verse::startRush()
{
    start("Classic");
    training = rushing = true;
    rushSeconds = 60;
    rushScore = 0;
    loadPuzzle(0);
    rushTimer.start();
}

void Verse::loadPuzzle(int index)
{
    puzzle = index;
    const auto &problem = puzzles.at(index);
    game.initializeBoard();
    entries.clear();
    comments.clear();
    ended = false;
    selection = hintSquare = promotionSquare = -1;
    for (const auto &move : QString::fromUtf8(problem.setup).split(' '))
    {
        const int from = square(move, 0);
        const int to = square(move, 2);
        game.movePiece(from / 8, from % 8, to / 8, to % 8);
        entries.append(QVariantMap{{"from", from}, {"to", to}});
    }
    const QString turn = game.getCurrentTurn() == PieceColor::White ? "White" : "Black";
    lesson = rushing ? turn + " to move. Find mate in one." : QString::fromUtf8(problem.hint);
    message = QString::fromUtf8(problem.title);
    emit changed();
}

void Verse::solve(int from, int to, PieceType promotion)
{
    ChessGame candidate = game;
    if (!candidate.movePiece(from / 8, from % 8, to / 8, to % 8, promotion) ||
        !candidate.isCheckmate(candidate.getCurrentTurn()))
    {
        lesson = "That move does not give checkmate. Try again.";
        selection = hintSquare = promotionSquare = -1;
        emit changed();
        return;
    }

    game = std::move(candidate);
    entries.append(QVariantMap{{"from", from}, {"to", to}, {"promotion", int(promotion)}});
    selection = hintSquare = promotionSquare = -1;
    if (sound)
        moveEffect.play();
    if (rushing && ++rushScore < int(puzzles.size()))
        loadPuzzle(puzzle + 1);
    else if (rushing)
        finishRush();
    else
    {
        ended = true;
        lesson = "Solved by checkmate.";
        emit changed();
    }
}

int Verse::rushBest() const
{
    return QSettings().value("training/rushBest", 0).toInt();
}

void Verse::finishRush()
{
    rushTimer.stop();
    if (rushScore > rushBest())
        QSettings().setValue("training/rushBest", rushScore);
    rushing = false;
    ended = true;
    selection = hintSquare = promotionSquare = -1;
    lesson = QString("Rush complete. %1 of %2 puzzles solved.").arg(rushScore).arg(int(puzzles.size()));
    emit changed();
}
