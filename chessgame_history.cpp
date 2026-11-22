#include "chessgame.h"

#include <algorithm>
#include <cstdlib>

ChessGame::ChessGame(PositionOnly, const ChessGame &other)
    : currentTurn(other.currentTurn),
      castlingRights(other.castlingRights),
      enPassantSquare(other.enPassantSquare)
{
    for (int row = 0; row < 8; ++row)
        std::copy_n(other.board[row], 8, board[row]);
}

void ChessGame::applyBoardMove(const Move &move)
{
    const auto [fr, fc, tr, tc, piece, captured, promotion, rights, ep, half] = move;
    board[fr][fc] = {};
    board[tr][tc] = piece;
    if (piece.type == PieceType::Pawn)
    {
        if (tc != fc && captured.isEmpty())
            board[fr][tc] = {};
        if (tr == 0 || tr == 7)
            board[tr][tc].type = promotion;
    }
    if (piece.type == PieceType::King && std::abs(tc - fc) == 2)
    {
        const int rookCol = tc == 6 ? 7 : 0;
        board[fr][tc == 6 ? 5 : 3] = board[fr][rookCol];
        board[fr][rookCol] = {};
    }
}

void ChessGame::applyTurn(const Move &move)
{
    const auto [fr, fc, tr, tc, piece, captured, promotion, rights, ep, half] = move;
    if (piece.type == PieceType::King)
        castlingRights &= ~(piece.color == PieceColor::White ? 3 : 12);
    const auto clearRookRight = [this](int row, int col)
    {
        if ((row == 0 || row == 7) && (col == 0 || col == 7))
            castlingRights &= ~(1 << ((row == 7 ? 0 : 2) + (col == 0)));
    };
    if (piece.type == PieceType::Rook)
        clearRookRight(fr, fc);
    if (captured.type == PieceType::Rook)
        clearRookRight(tr, tc);

    const bool pawn = piece.type == PieceType::Pawn;
    enPassantSquare = pawn && std::abs(fr - tr) == 2 ? ((fr + tr) / 2) * 8 + fc : -1;
    halfmoves = pawn || !captured.isEmpty() ? 0 : halfmoves + 1;
    applyBoardMove(move);
    currentTurn = currentTurn == PieceColor::White ? PieceColor::Black : PieceColor::White;
    positions.push_back(positionKey());
}

bool ChessGame::movePiece(int fr, int fc, int tr, int tc, PieceType promotion)
{
    if (!isValidMove(fr, fc, tr, tc) || !drawReason().isEmpty())
        return false;
    if (promotion != PieceType::Queen && promotion != PieceType::Rook &&
        promotion != PieceType::Bishop && promotion != PieceType::Knight)
        return false;

    Move move(fr, fc, tr, tc, board[fr][fc], board[tr][tc]);
    move.promotion = promotion;
    move.previousRights = castlingRights;
    move.previousEnPassant = enPassantSquare;
    move.previousHalfmoves = halfmoves;
    moveHistory.push(move);
    redoHistory.clear();
    applyTurn(move);
    return true;
}

bool ChessGame::undoMove()
{
    if (moveHistory.isEmpty())
        return false;

    const Move move = moveHistory.pop();
    const auto [fr, fc, tr, tc, piece, captured, promotion, rights, ep, half] = move;
    redoHistory.push(move);
    board[fr][fc] = piece;
    board[tr][tc] = captured;
    if (piece.type == PieceType::Pawn && fc != tc && captured.isEmpty())
        board[fr][tc] = Piece(PieceType::Pawn, currentTurn);
    if (piece.type == PieceType::King && std::abs(tc - fc) == 2)
    {
        const int rookDestination = tc == 6 ? 5 : 3;
        board[fr][tc == 6 ? 7 : 0] = board[fr][rookDestination];
        board[fr][rookDestination] = {};
    }
    currentTurn = piece.color;
    castlingRights = rights;
    enPassantSquare = ep;
    halfmoves = half;
    positions.pop_back();
    return true;
}

bool ChessGame::redoMove()
{
    if (redoHistory.isEmpty())
        return false;
    const Move move = redoHistory.pop();
    moveHistory.push(move);
    applyTurn(move);
    return true;
}

int ChessGame::getMoveCount() const
{
    return moveHistory.size();
}

std::string ChessGame::positionKey() const
{
    std::string key;
    key.reserve(67);
    for (const auto &rank : board)
        for (const Piece &piece : rank)
            key.push_back(static_cast<char>(8 * int(piece.color) + int(piece.type)));
    key.push_back(static_cast<char>(currentTurn));
    key.push_back(static_cast<char>(castlingRights));
    int legalEnPassant = -1;
    if (enPassantSquare >= 0)
    {
        const int row = enPassantSquare / 8;
        const int col = enPassantSquare % 8;
        const int pawnRow = row + (currentTurn == PieceColor::White ? 1 : -1);
        for (const int pawnCol : {col - 1, col + 1})
        {
            if (getPiece(pawnRow, pawnCol).type == PieceType::Pawn &&
                isValidMove(pawnRow, pawnCol, row, col))
                legalEnPassant = enPassantSquare;
        }
    }
    key.push_back(static_cast<char>(legalEnPassant + 1));
    return key;
}

QString ChessGame::drawReason() const
{
    int bishops = 0;
    int knights = 0;
    int bishopColors = 0;
    bool hasMajorMaterial = false;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            switch (board[row][col].type)
            {
            case PieceType::Pawn:
            case PieceType::Rook:
            case PieceType::Queen: hasMajorMaterial = true; break;
            case PieceType::Knight: ++knights; break;
            case PieceType::Bishop:
                ++bishops;
                bishopColors |= 1 << ((row + col) % 2);
                break;
            default: break;
            }
        }
    }
    if (!hasMajorMaterial && (bishops + knights <= 1 || (knights == 0 && bishopColors != 3)))
        return "Insufficient material";
    if (halfmoves >= 100)
        return "Fifty-move rule";
    if (positions.size() >= 5 && std::count(positions.begin(), positions.end(), positions.back()) >= 3)
        return "Threefold repetition";
    return {};
}
