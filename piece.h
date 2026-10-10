#ifndef PIECE_H
#define PIECE_H

enum class PieceType
{
    None,
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn
};

enum class PieceColor
{
    None,
    White,
    Black
};

struct Piece
{
    PieceType type;
    PieceColor color;

    Piece()
    {
        type = PieceType::None;
        color = PieceColor::None;
    }

    Piece(PieceType t, PieceColor c)
    {
        type = t;
        color = c;
    }

    bool isEmpty() const
    {
        return type == PieceType::None;
    }
};

#endif