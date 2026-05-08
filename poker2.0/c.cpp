#include "poker.h"

Card::Card(int r, char s) : rank(r), suit(s) {}
bool Card::operator<(const Card& other) const {
    return rank < other.rank;
}
