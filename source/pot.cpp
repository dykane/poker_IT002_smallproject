#include "poker.h"

Pot::Pot() : total(0) {}
void Pot::collect_bets(vector<Player>& players) {
    for (Player& p : players) {
        total += p.bet_amount;
        p.bet_amount = 0; // Reset bet cho vòng sau
    }
}
void Pot::reset() {
    total = 0;
}
