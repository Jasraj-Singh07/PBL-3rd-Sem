#include "movehistory.h"

MoveHistory::MoveHistory()
{
    topIndex = -1;
}

bool MoveHistory::push(Move move)
{
    if (isFull())
    {
        return false;
    }

    topIndex++;

    history[topIndex] = move;

    return true;
}

Move MoveHistory::pop()
{
    if (isEmpty())
    {
        return Move();
    }

    Move move = history[topIndex];

    topIndex--;

    return move;
}

Move MoveHistory::peek() const
{
    if (isEmpty())
    {
        return Move();
    }

    return history[topIndex];
}

bool MoveHistory::isEmpty() const
{
    return topIndex == -1;
}

bool MoveHistory::isFull() const
{
    return topIndex == MAX_MOVES - 1;
}

int MoveHistory::size() const
{
    return topIndex + 1;
}

void MoveHistory::clear()
{
    topIndex = -1;
}