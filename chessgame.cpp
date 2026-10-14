#include "chessgame.h"

ChessGame::ChessGame()
{
    initializeBoard();
}
void ChessGame::initializeBoard()
{
    currentTurn = PieceColor::White;

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            board[row][col] = Piece();
        }
    }

    // =========================
    // BLACK BACK ROW
    // =========================

    board[0][0] =
        Piece(PieceType::Rook, PieceColor::Black);

    board[0][1] =
        Piece(PieceType::Knight, PieceColor::Black);

    board[0][2] =
        Piece(PieceType::Bishop, PieceColor::Black);

    board[0][3] =
        Piece(PieceType::Queen, PieceColor::Black);

    board[0][4] =
        Piece(PieceType::King, PieceColor::Black);

    board[0][5] =
        Piece(PieceType::Bishop, PieceColor::Black);

    board[0][6] =
        Piece(PieceType::Knight, PieceColor::Black);

    board[0][7] =
        Piece(PieceType::Rook, PieceColor::Black);


    // =========================
    // BLACK PAWNS
    // =========================

    for (int col = 0; col < 8; col++)
    {
        board[1][col] =
            Piece(PieceType::Pawn, PieceColor::Black);
    }


    // =========================
    // WHITE BACK ROW
    // =========================

    board[7][0] =
        Piece(PieceType::Rook, PieceColor::White);

    board[7][1] =
        Piece(PieceType::Knight, PieceColor::White);

    board[7][2] =
        Piece(PieceType::Bishop, PieceColor::White);

    board[7][3] =
        Piece(PieceType::Queen, PieceColor::White);

    board[7][4] =
        Piece(PieceType::King, PieceColor::White);

    board[7][5] =
        Piece(PieceType::Bishop, PieceColor::White);

    board[7][6] =
        Piece(PieceType::Knight, PieceColor::White);

    board[7][7] =
        Piece(PieceType::Rook, PieceColor::White);


    // =========================
    // WHITE PAWNS
    // =========================

    for (int col = 0; col < 8; col++)
    {
        board[6][col] =
            Piece(PieceType::Pawn, PieceColor::White);
    }

    moveHistory.clear();
}


// =====================================================
// GET PIECE
// =====================================================
Piece ChessGame::getPiece(
    int row,
    int col) const
{
    if (!isInsideBoard(row, col))
    {
        return Piece();
    }

    return board[row][col];
}


// =====================================================
// CURRENT TURN
// =====================================================
PieceColor ChessGame::getCurrentTurn() const
{
    return currentTurn;
}


// =====================================================
// BOARD BOUNDARY
// =====================================================
bool ChessGame::isInsideBoard(
    int row,
    int col) const
{
    return row >= 0 &&
           row < 8 &&
           col >= 0 &&
           col < 8;
}


// =====================================================
// PIECE SYMBOL
// =====================================================
QString ChessGame::getPieceSymbol(
    int row,
    int col) const
{
    if (!isInsideBoard(row, col))
    {
        return "";
    }

    Piece piece = board[row][col];

    if (piece.isEmpty())
    {
        return "";
    }

    if (piece.color == PieceColor::White)
    {
        switch (piece.type)
        {
        case PieceType::King:
            return "♔";

        case PieceType::Queen:
            return "♕";

        case PieceType::Rook:
            return "♖";

        case PieceType::Bishop:
            return "♗";

        case PieceType::Knight:
            return "♘";

        case PieceType::Pawn:
            return "♙";

        default:
            return "";
        }
    }

    switch (piece.type)
    {
    case PieceType::King:
        return "♚";

    case PieceType::Queen:
        return "♛";

    case PieceType::Rook:
        return "♜";

    case PieceType::Bishop:
        return "♝";

    case PieceType::Knight:
        return "♞";

    case PieceType::Pawn:
        return "♟";

    default:
        return "";
    }
}


// =====================================================
// PIECE NAME
// =====================================================
QString ChessGame::getPieceName(
    int row,
    int col) const
{
    if (!isInsideBoard(row, col))
    {
        return "";
    }

    Piece piece = board[row][col];

    if (piece.isEmpty())
    {
        return "";
    }

    switch (piece.type)
    {
    case PieceType::King:
        return "King";

    case PieceType::Queen:
        return "Queen";

    case PieceType::Rook:
        return "Rook";

    case PieceType::Bishop:
        return "Bishop";

    case PieceType::Knight:
        return "Knight";

    case PieceType::Pawn:
        return "Pawn";

    default:
        return "";
    }
}


// =====================================================
// PATH CLEAR
// =====================================================
