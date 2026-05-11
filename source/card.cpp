#include "poker.h"

string Card::toString() const {
    string r = "";
    if (rank <= 10) r = to_string(rank);
    else if (rank == 11) r = "J";
    else if (rank == 12) r = "Q";
    else if (rank == 13) r = "K";
    else if (rank == 14) r = "A";
    return r + suit;
}
