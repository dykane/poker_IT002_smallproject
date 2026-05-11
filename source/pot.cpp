#include "poker.h"

void Pot::collect(long long amount) {
    total_money += amount;
}

void Pot::clear() {
    total_money = 0;
}
