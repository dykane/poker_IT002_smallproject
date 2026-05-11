#include "poker.h"

Game::Game(int p_count, const vector<long long>& init_chips, const vector<AIType>& ai_types) {
    for (int i = 0; i < p_count; ++i) {
        players.push_back(Player("Player_" + to_string(i), init_chips[i], ai_types[i]));
    }
    dealer_idx = 0;
    current_game = 0;
    current_blind = 20; // Big blind default
}

int Game::countActivePlayers() {
    int c = 0;
    for (auto& p : players) if (p.active) c++;
    return c;
}

vector<int> Game::getActivePlayers() {
    vector<int> act;
    for (size_t i = 0; i < players.size(); i++)
        if (players[i].active) act.push_back(i);
    return act;
}

void Game::initSingleGame() {
    deck.shuffle();
    board.clear();
    pot.clear();
    highest_bet = current_blind;

    for (auto& p : players) p.resetForNewRound();

    cout << "\n========== VAN " << current_game + 1 << " ==========\n";
    cout << "Dealer: " << players[dealer_idx].name << "\n";
    cout << "--- Dong tien mu (Blinds) ---\n";

    // Đóng Blinds (Small Blind và Big Blind)
    int sb_idx = (dealer_idx + 1) % players.size();
    int bb_idx = (dealer_idx + 2) % players.size();

    if (players[sb_idx].active) {
        players[sb_idx].call(current_blind / 2);
        pot.collect(players[sb_idx].bet_amount);
    }
    if (players[bb_idx].active) {
        players[bb_idx].call(current_blind);
        pot.collect(players[bb_idx].bet_amount);
    }
}

void Game::dealPreflop() {
    cout << "\n[ PREFLOP ]\n";
    for (auto& p : players) {
        if (p.active) {
            p.hand.push_back(deck.draw());
            p.hand.push_back(deck.draw());
        }
    }
    BettingRound::playRound(*this);
}

void Game::dealFlop() {
    cout << "\n[ FLOP ]\n";
    deck.draw(); // Burn card
    board.push_back(deck.draw());
    board.push_back(deck.draw());
    board.push_back(deck.draw());
    cout << "Board: " << board[0].toString() << " " << board[1].toString() << " " << board[2].toString() << "\n";
    highest_bet = 0;
    for(auto& p: players) p.bet_amount = 0;
    BettingRound::playRound(*this);
}

void Game::dealTurn() {
    cout << "\n[ TURN ]\n";
    deck.draw(); // Burn card
    board.push_back(deck.draw());
    cout << "Board += " << board[3].toString() << "\n";
    highest_bet = 0;
    for(auto& p: players) p.bet_amount = 0;
    BettingRound::playRound(*this);
}

void Game::dealRiver() {
    cout << "\n[ RIVER ]\n";
    deck.draw(); // Burn card
    board.push_back(deck.draw());
    cout << "Board += " << board[4].toString() << "\n";
    highest_bet = 0;
    for(auto& p: players) p.bet_amount = 0;
    BettingRound::playRound(*this);
}

void Game::showdown() {
    cout << "\n[ SHOWDOWN ]\n";
    vector<int> active_idx = getActivePlayers();

    if (active_idx.size() == 1) {
        cout << players[active_idx[0]].name << " WIN do tat ca da fold!\n";
        players[active_idx[0]].chip += pot.total_money;
        players[active_idx[0]].chip_won += pot.total_money;
        players[active_idx[0]].wins++;
        return;
    }

    int best_player = active_idx[0];
    HandResult best_res = HandEvaluator::evaluate(players[best_player].hand, board);

    for (size_t i = 1; i < active_idx.size(); ++i) {
        int idx = active_idx[i];
        HandResult res = HandEvaluator::evaluate(players[idx].hand, board);
        if (best_res < res) {
            best_res = res;
            best_player = idx;
        }
    }

    cout << players[best_player].name << " WIN POT " << pot.total_money << " chip!\n";
    players[best_player].chip += pot.total_money;
    players[best_player].chip_won += pot.total_money;
    players[best_player].wins++;
}

void Game::playGames(int n_games) {
    num_games = n_games;
    for (current_game = 0; current_game < num_games; ++current_game) {
        initSingleGame();

        dealPreflop();
        if (countActivePlayers() > 1) dealFlop();
        if (countActivePlayers() > 1) dealTurn();
        if (countActivePlayers() > 1) dealRiver();

        showdown();

        dealer_idx = (dealer_idx + 1) % players.size();
    }

    // --- PHẦN THỐNG KÊ MỚI ---
    cout << "\n==============================================================\n";
    cout << "                 THONG KE SAU " << num_games << " VAN\n";
    cout << "==============================================================\n";

    // In Header của bảng
    cout << left << setw(15) << "Nguoi choi"
         << " | " << setw(12) << "So van thang"
         << " | " << setw(15) << "Chip ban dau"
         << " | " << setw(15) << "Chip hien tai"
         << " | " << setw(12) << "Chip thang"
         << " | " << "Chip thua\n";
    cout << "==============================================================\n";

    // In dữ liệu từng người chơi
    long long total_pot = 0;
    for (auto& p : players) {
        long long chip_loss = p.init_chip - p.chip;
        cout << left << setw(15) << p.name
             << " | " << setw(12) << p.wins
             << " | " << setw(15) << p.init_chip
             << " | " << setw(15) << p.chip
             << " | " << setw(12) << p.chip_won
             << " | " << chip_loss << "\n";
        total_pot += p.chip;
    }
    cout << "==============================================================\n";
    cout << "TONG CHIP: " << total_pot << "\n";
    cout << "==============================================================\n";

    // Xuất ra file CSV
    Statistics::exportToCSV(players, num_games);
}
