
#include "ai.h"

#include <cstdlib>
#include <random>
#include <algorithm>

void ChessAI::collectMoves(
    const ChessGame &game,
    Move moves[],
    int &count)
{
    count = 0;

    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            Piece p = game.getPiece(r, c);

            if (p.isEmpty() ||
                p.color != game.getCurrentTurn())
                continue;

            for (int tr = 0; tr < 8; ++tr)
            {
                for (int tc = 0; tc < 8; ++tc)
                {
                    if (count < 256 &&
                        game.isValidMove(r, c, tr, tc))
                    {
                        Move move(r, c, tr, tc, p, game.getPiece(tr, tc));
                        moves[count++] = move;
                        if (p.type == PieceType::Pawn && (tr == 0 || tr == 7))
                        {
                            for (auto promotion : {PieceType::Rook, PieceType::Bishop, PieceType::Knight})
                            {
                                if (count == 256)
                                    break;
                                move.promotion = promotion;
                                moves[count++] = move;
                            }
                        }
                    }
                }
            }
        }
    }
}

int ChessAI::pieceValue(PieceType type)
{
    switch (type)
    {
    case PieceType::Pawn:   return 100;
    case PieceType::Knight: return 320;
    case PieceType::Bishop: return 330;
    case PieceType::Rook:   return 500;
    case PieceType::Queen:  return 900;
    case PieceType::King:   return 20000;
    default:                return 0;
    }
}

// Positive scores favour Black; negative scores favour White.
int ChessAI::evaluate(const ChessGame &game)
{
    int score = 0;

    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            Piece p = game.getPiece(r, c);

            if (p.isEmpty())
                continue;

            int value = pieceValue(p.type);

            // Small centre bonus encourages development.
            if (p.type != PieceType::King)
            {
                value += (3 - std::abs(3 - r));
                value += (3 - std::abs(3 - c));
            }

            score +=
                (p.color == PieceColor::Black)
                ? value : -value;
        }
    }

    return score;
}

int ChessAI::minimax(
    ChessGame game,
    int depth,
    int alpha,
    int beta)
{
    if (game.isCheckmate(game.getCurrentTurn()))
        return game.getCurrentTurn() == PieceColor::Black ? -100000 + (10-depth) : 100000 - (10-depth);
    if (!game.drawReason().isEmpty() || game.isStalemate(game.getCurrentTurn())) return 0;
    if (depth == 0) return evaluate(game);

    Move moves[256];
    int count = 0;

    collectMoves(game, moves, count);

    if (count == 0)
        return evaluate(game);

    bool maximizing =
        game.getCurrentTurn() == PieceColor::Black;

    int best = maximizing ? -1000000 : 1000000;

    for (int i = 0; i < count; ++i)
    {
        ChessGame next = game;

        if (!next.movePiece(
                moves[i].fromRow,
                moves[i].fromCol,
                moves[i].toRow,
                moves[i].toCol, moves[i].promotion))
            continue;

        int value = minimax(
            next, depth - 1, alpha, beta
        );

        if (maximizing)
        {
            if (value > best)
                best = value;

            if (best > alpha)
                alpha = best;
        }
        else
        {
            if (value < best)
                best = value;

            if (best < beta)
                beta = best;
        }

        if (beta <= alpha)
            break;
    }

    return best;
}

bool ChessAI::chooseMove(
    const ChessGame &game,
    const QString &difficulty,
    Move &bestMove)
{
    if (!game.drawReason().isEmpty())
        return false;
    Move moves[256];
    int count = 0;
    collectMoves(game, moves, count);
    if (count == 0)
        return false;

    // Easy: choose a random legal move.
    if (difficulty == "Easy")
    {
        thread_local std::mt19937 random(std::random_device{}());
        bestMove = moves[std::uniform_int_distribution<int>(0, count - 1)(random)];
        return true;
    }

    // Medium searches 2 plies; Hard searches 3.
    int depth =
        (difficulty == "Hard") ? 3 : 2;

    const bool maximizing = game.getCurrentTurn() == PieceColor::Black;
    int bestScore = maximizing ? -1000000 : 1000000;
    int bestIndex = 0;

    for (int i = 0; i < count; ++i)
    {
        ChessGame next = game;

        if (!next.movePiece(
                moves[i].fromRow,
                moves[i].fromCol,
                moves[i].toRow,
                moves[i].toCol, moves[i].promotion))
            continue;

        int score = minimax(
            next, depth - 1, -1000000, 1000000
        );

        if (maximizing ? score > bestScore : score < bestScore)
        {
            bestScore = score;
            bestIndex = i;
        }
    }

    bestMove = moves[bestIndex];
    return true;
}

int ChessAI::moveLoss(const ChessGame &game, const ChessGame &played)
{
    const int direction = game.getCurrentTurn() == PieceColor::Black ? 1 : -1;
    const int actual = direction * minimax(played, 1, -1000000, 1000000);
    Move best;
    if (!chooseMove(game, "Medium", best))
        return 0;
    ChessGame expected = game;
    expected.movePiece(best.fromRow, best.fromCol, best.toRow, best.toCol, best.promotion);
    return std::max(0, direction * minimax(expected, 1, -1000000, 1000000) - actual);
}
