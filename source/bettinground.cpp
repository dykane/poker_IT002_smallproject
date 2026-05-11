#include "poker.h"

void BettingRound::playRound(Game& game) {
    int active_count = game.countActivePlayers();
    if (active_count <= 1) return; // Không cần bet nếu chỉ còn 1 người

    bool round_over = false;
    int curr_idx = (game.dealer_idx + 1) % game.players.size();
    if (game.board.empty()) { // Preflop: Người đánh đầu tiên sau Big Blind
        curr_idx = (game.dealer_idx + 3) % game.players.size();
    }

    // Đặt biến xác nhận đã có cơ hội hành động
    vector<bool> acted(game.players.size(), false);

    while (!round_over) {
        Player& p = game.players[curr_idx];

        if (p.active && p.chip > 0) {
            // --- PHẦN CẬP NHẬT: In ra thông tin và lá bài của người chơi ---
            cout << "Turn: " << p.name << " (Chips: " << p.chip << ") | Hand: [ ";
            for (const auto& c : p.hand) {
                cout << c.toString() << " ";
            }
            cout << "]\n";
            // ---------------------------------------------------------------

            long long to_call = game.highest_bet - p.bet_amount;
            if (to_call < 0) to_call = 0;

            long long bet_before = p.bet_amount;
            Tactics::decide(p, game, to_call, game.highest_bet);

            long long bet_added = p.bet_amount - bet_before;
            game.pot.collect(bet_added);

            if (p.bet_amount > game.highest_bet) {
                game.highest_bet = p.bet_amount;
                // Nếu có raise, reset trạng thái acted của những người khác
                for (size_t i = 0; i < acted.size(); i++) acted[i] = false;
            }
        }

        acted[curr_idx] = true;

        // Kiểm tra kết thúc vòng
        round_over = true;
        int active_players_with_chips = 0;

        for (size_t i = 0; i < game.players.size(); i++) {
            if (game.players[i].active) {
                if (game.players[i].chip > 0) active_players_with_chips++;
                if (game.players[i].chip > 0 && (!acted[i] || game.players[i].bet_amount < game.highest_bet)) {
                    round_over = false;
                }
            }
        }

        if (active_players_with_chips <= 1) round_over = true;
        curr_idx = (curr_idx + 1) % game.players.size();
    }

    cout << "--- HET VONG CUOC. POT: " << game.pot.total_money << " ---\n";
}
