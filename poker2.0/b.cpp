#include "poker.h"

void BettingRound::play(vector<Player>& players, vector<Card>& board, long long& pot) {
    bool done = false;
    while (!done) {
        done = true;
        long long max_bet = 0;
        for (Player& p : players) if (p.active) max_bet = max(max_bet, p.bet_amount);

        for (int i = 0; i < players.size(); i++) {
            if (!players[i].active || players[i].is_all_in) continue;

            long long to_call = max_bet - players[i].bet_amount;

            long long old_bet = players[i].bet_amount;
            Tactics::execute(players[i], to_call, board);

            if (players[i].bet_amount > old_bet) {
                // If someone raised (or all-in higher), we need to loop again
                if (players[i].bet_amount > max_bet) {
                    max_bet = players[i].bet_amount;
                    done = false;
                }
            }
        }
    }
}
