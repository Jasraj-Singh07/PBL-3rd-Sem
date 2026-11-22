#ifndef MOVEHISTORY_H
#define MOVEHISTORY_H
#include "move.h"
#include <vector>
class MoveHistory {
    std::vector<Move> history;
public:
    MoveHistory() = default;
    bool push(Move move);
    Move pop();
    Move peek() const;
    bool isEmpty() const;
    bool isFull() const;
    int size() const;
    void clear();
};
#endif
