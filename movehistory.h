#ifndef MOVEHISTORY_H
#define MOVEHISTORY_H

#include "move.h"

class MoveHistory
{
private:
    static const int MAX_MOVES = 100;

    Move history[MAX_MOVES];

    int topIndex;

public:
    MoveHistory();

    bool push(Move move);
    Move pop();
    Move peek() const;

    bool isEmpty() const;
    bool isFull() const;

    int size() const;

    void clear();
};

#endif