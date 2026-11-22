#include "movehistory.h"
bool MoveHistory::push(Move move) { history.push_back(move); return true; }
Move MoveHistory::pop() { if(isEmpty()) return {}; Move m=history.back(); history.pop_back(); return m; }
Move MoveHistory::peek() const { return isEmpty()?Move{}:history.back(); }
bool MoveHistory::isEmpty() const { return history.empty(); }
bool MoveHistory::isFull() const { return false; }
int MoveHistory::size() const { return static_cast<int>(history.size()); }
void MoveHistory::clear() { history.clear(); }
