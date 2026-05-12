#include "poker.h"

// Hàm estimateWinRate giữ nguyên như cũ
double Tactics::estimateWinRate(const vector<Card>& hand, const vector<Card>& board) {
    int wins = 0;
    int simulations = 100;

    for (int i = 0; i < simulations; ++i) {
        Deck sim_deck;
        vector<Card> known = hand;
        known.insert(known.end(), board.begin(), board.end());
        sim_deck.removeKnown(known);
        sim_deck.shuffle();

        vector<Card> sim_board = board;
        while (sim_board.size() < 5) sim_board.push_back(sim_deck.draw());

        vector<Card> opp_hand = {sim_deck.draw(), sim_deck.draw()};

        HandResult my_res = HandEvaluator::evaluate(hand, sim_board);
        HandResult opp_res = HandEvaluator::evaluate(opp_hand, sim_board);

        if (!(my_res < opp_res)) wins++;
    }
    return (double)wins / simulations;
}

void Tactics::decide(Player& p, Game& game, long long to_call, long long highest_bet) {
    if (p.chip == 0) return; // All-in rồi không làm gì

    if (p.ai_type == BRAINROT) decide_brainrot(p, game.board, to_call, highest_bet);
    else if (p.ai_type == CHOICONAO) decide_choiconao(p, game.board, to_call, highest_bet);
    else if (p.ai_type == CHOIANTOAN) decide_choiantoan(p, game.board, to_call, highest_bet);
}

void Tactics::decide_brainrot(Player& p, const vector<Card>& board, long long to_call, long long highest_bet) {
    // PREFLOP
    if (board.empty()) {
        if (to_call == 0) {
            p.check();
        } else {
            // Preflop: random call hoặc raise
            if (p.chip > to_call + 10 && rand() % 3 == 0) {
                p.raise(to_call, 10);
            } else {
                p.call(to_call);
            }
        }
        return;
    }

    // POSTFLOP - Logic mới cho BRAINROT
    double win_rate = estimateWinRate(p.hand, board);

    // --- Win rate > 80%: Raise mạnh ---
    if (win_rate > 0.80) {
        long long strong_raise = highest_bet > 0 ? highest_bet * 2 : 200;

        if (p.chip > to_call + strong_raise) {
            p.raise(to_call, strong_raise);
        } else if (p.chip > to_call) {
            // Nếu không đủ raise mạnh, thì all-in
            p.all_in();
        } else {
            // Nếu vừa đủ to_call, call
            p.call(to_call);
        }
        return;
    }

    // --- Win rate 65%-80%: Raise nhẹ ---
    if (win_rate > 0.65) {
        long long light_raise = highest_bet > 0 ? highest_bet / 2 : 50;

        if (p.chip > to_call + light_raise) {
            p.raise(to_call, light_raise);
        } else {
            p.call(to_call);
        }
        return;
    }

    // --- Win rate 70% trở lên: All-in khi có bot khác bet ---
    if (win_rate > 0.70) {
        if (to_call > 0) {
            // Có bot khác đã bet
            if (p.chip > to_call) {
                p.all_in();
            } else {
                p.call(to_call);
            }
        } else {
            // Không ai bet, check
            p.check();
        }
        return;
    }

    // --- Win rate < 70%: Call hoặc Check ---
    if (to_call == 0) {
        p.check();
    } else if (to_call <= p.chip / 5) {
        // Nếu to_call nhỏ (dưới 20% chip), call
        p.call(to_call);
    } else {
        // Nếu to_call quá lớn, fold
        p.fold();
    }
}

void Tactics::decide_choiconao(Player& p, const vector<Card>& board, long long to_call, long long highest_bet) {
    // PREFLOP
    if (board.empty()) {
        int r1 = p.hand[0].rank, r2 = p.hand[1].rank;
        bool is_pair = (r1 == r2);
        bool is_high = (r1 >= 10 && r2 >= 10);
        bool is_suited = (p.hand[0].suit == p.hand[1].suit);

        if (is_pair || is_high) {
            if (to_call == 0 || to_call <= 50) p.raise(to_call, 50);
            else p.call(to_call);
        } else if (r1 >= 13 || r2 >= 13 || is_suited) {
            if (to_call > p.chip / 10) p.fold();
            else {
                if (to_call == 0) p.check();
                else p.call(to_call);
            }
        } else {
            if (to_call > 0) p.fold();
            else p.check();
        }
        return;
    }

    // POSTFLOP
    double win_rate = estimateWinRate(p.hand, board);
    HandResult current_power = HandEvaluator::evaluate(p.hand, board);

    if (current_power.rank >= THREE_OF_A_KIND || win_rate > 0.7) {
        if (rand() % 3 == 0) {
            if (to_call == 0) p.check(); else p.call(to_call);
        } else p.raise(to_call, highest_bet > 0 ? highest_bet : 100);
        return;
    }

    if (win_rate > 0.4) {
        if (to_call > p.chip / 4) p.fold();
        else {
            if (to_call == 0) p.check();
            else p.call(to_call);
        }
        return;
    }

    if (to_call == 0) p.check();
    else p.fold();
}

void Tactics::decide_choiantoan(Player& p, const vector<Card>& board, long long to_call, long long highest_bet) {
    long long limit = p.chip / 2;

    if (to_call > limit) {
        p.fold();
        return;
    }

    double win_rate = estimateWinRate(p.hand, board);

    // --- LOGIC MỚI: Khi win rate > 85% (0.85) ---
    if (win_rate > 0.85) {
        // Raise nhẹ dưới hoặc bằng 70% số tiền của nó
        long long safe_raise = (p.chip * 70) / 100;  // 70% chip hiện tại
        long long actual_raise = min(safe_raise, p.chip - to_call);

        if (actual_raise > 0 && p.chip > to_call + actual_raise) {
            p.raise(to_call, actual_raise);
        } else {
            // Nếu không đủ để raise an toàn, thì call
            if (to_call == 0) p.check();
            else p.call(to_call);
        }
        return;
    }

    // Logic cũ: Khi win rate > 75%
    if (win_rate > 0.75 && limit > to_call) {
        long long safe_raise = min((long long)50, limit - to_call);
        if (safe_raise > 0) p.raise(to_call, safe_raise);
        else p.call(to_call);
    } else {
        if (to_call == 0) p.check();
        else p.call(to_call);
    }
}
