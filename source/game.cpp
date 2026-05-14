#include "poker.h"
#include <iostream>
#include <iomanip>
#include <map>
using namespace std;

string aiTypeToString(AIType type) {
    if (type == BRAINROT) return "BRAINROT";
    if (type == CHOICONAO) return "CHOICONAO";
    if (type == CHOIANTOAN) return "CHOIANTOAN";
    return "UNKNOWN";
}

Game::Game(int p_count, const vector<long long>& init_chips, const vector<AIType>& ai_types) {
    for (int i = 0; i < p_count; ++i) {
        players.push_back(Player("Player_" + to_string(i), init_chips[i], ai_types[i]));
    }

    dealer_idx = 0;
    current_game = 0;
    current_blind = 20;
}

int Game::countActivePlayers() {
    int c = 0;

    for (auto& p : players) {
        if (p.active) c++;
    }

    return c;
}

int Game::countPlayersWithChips() {
    int c = 0;

    for (auto& p : players) {
        if (p.chip > 0) c++;
    }

    return c;
}

int Game::getFinalWinnerIndex() {
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i].chip > 0) return (int)i;
    }

    return -1;
}

int Game::getNextPlayerWithChips(int start_idx) {
    int n = players.size();

    for (int step = 0; step < n; ++step) {
        int idx = (start_idx + step) % n;

        if (players[idx].chip > 0) {
            return idx;
        }
    }

    return -1;
}

vector<int> Game::getActivePlayers() {
    vector<int> act;

    for (size_t i = 0; i < players.size(); i++) {
        if (players[i].active) act.push_back(i);
    }

    return act;
}

void Game::updateBustStatus() {
    for (auto& p : players) {
        if (p.chip <= 0 && !p.busted) {
            p.busted = true;
            p.bust_hand = current_game + 1;
        }
    }
}

void Game::initSingleGame() {
    deck.shuffle();
    board.clear();
    pot.clear();
    highest_bet = current_blind;

    for (auto& p : players) {
        p.resetForNewRound();

        if (p.chip > 0) {
            p.hands_played++;
        }
    }

    cout << "\n========== VAN " << current_game + 1 << " ==========\n";
    cout << "Dealer: " << players[dealer_idx].name << "\n";
    cout << "--- Dong tien mu (Blinds) ---\n";

    int sb_idx = getNextPlayerWithChips(dealer_idx + 1);
    int bb_idx = getNextPlayerWithChips(sb_idx + 1);

    if (sb_idx == -1 || bb_idx == -1 || sb_idx == bb_idx) {
        return;
    }

    players[sb_idx].call(current_blind / 2);
    pot.collect(players[sb_idx].bet_amount);

    players[bb_idx].call(current_blind);
    pot.collect(players[bb_idx].bet_amount);
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

    deck.draw();

    board.push_back(deck.draw());
    board.push_back(deck.draw());
    board.push_back(deck.draw());

    cout << "Board: "
         << board[0].toString() << " "
         << board[1].toString() << " "
         << board[2].toString() << "\n";

    highest_bet = 0;

    for (auto& p : players) {
        p.bet_amount = 0;
    }

    BettingRound::playRound(*this);
}

void Game::dealTurn() {
    cout << "\n[ TURN ]\n";

    deck.draw();

    board.push_back(deck.draw());

    cout << "Board += " << board[3].toString() << "\n";

    highest_bet = 0;

    for (auto& p : players) {
        p.bet_amount = 0;
    }

    BettingRound::playRound(*this);
}

void Game::dealRiver() {
    cout << "\n[ RIVER ]\n";

    deck.draw();

    board.push_back(deck.draw());

    cout << "Board += " << board[4].toString() << "\n";

    highest_bet = 0;

    for (auto& p : players) {
        p.bet_amount = 0;
    }

    BettingRound::playRound(*this);
}

void Game::showdown() {
    cout << "\n[ SHOWDOWN ]\n";

    vector<int> active_idx = getActivePlayers();

    if (active_idx.empty()) {
        cout << "Khong co nguoi choi active.\n";
        return;
    }

    if (active_idx.size() == 1) {
        int winner = active_idx[0];

        cout << players[winner].name << " WIN do tat ca da fold!\n";

        players[winner].chip += pot.total_money;
        players[winner].chip_won += pot.total_money;
        players[winner].wins++;

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

    cout << players[best_player].name << " WIN POT "
         << pot.total_money << " chip!\n";

    players[best_player].chip += pot.total_money;
    players[best_player].chip_won += pot.total_money;
    players[best_player].wins++;
}

void Game::printFinalStatistics(int actual_games, int max_games) {
    cout << "\n====================================================================================================================\n";
    cout << "                                      TOURNAMENT-STYLE SIMULATION RESULT\n";
    cout << "====================================================================================================================\n";
    cout << "Max hands input : " << max_games << "\n";
    cout << "Actual hands    : " << actual_games << "\n";

    if (countPlayersWithChips() == 1) {
        int winner = getFinalWinnerIndex();

        cout << "End reason      : Only one player still has chips\n";
        cout << "Final winner    : " << players[winner].name
             << " (" << aiTypeToString(players[winner].ai_type) << ")\n";
    } else {
        cout << "End reason      : Reached max hands limit\n";
    }

    cout << "====================================================================================================================\n";

    cout << left
         << setw(12) << "Player"
         << " | " << setw(12) << "Strategy"
         << " | " << setw(12) << "Hands"
         << " | " << setw(8) << "Wins"
         << " | " << setw(10) << "Hand WR"
         << " | " << setw(12) << "Init Chip"
         << " | " << setw(12) << "Final Chip"
         << " | " << setw(12) << "Profit"
         << " | " << setw(12) << "Bust Hand"
         << " | " << "Status\n";

    cout << "--------------------------------------------------------------------------------------------------------------------\n";

    long long total_chip = 0;

    for (auto& p : players) {
        double hand_wr = 0.0;

        if (p.hands_played > 0) {
            hand_wr = (double)p.wins * 100.0 / p.hands_played;
        }

        long long profit = p.chip - p.init_chip;

        cout << left
             << setw(12) << p.name
             << " | " << setw(12) << aiTypeToString(p.ai_type)
             << " | " << setw(12) << p.hands_played
             << " | " << setw(8) << p.wins
             << " | " << fixed << setprecision(2) << setw(9) << hand_wr << "%"
             << " | " << setw(12) << p.init_chip
             << " | " << setw(12) << p.chip
             << " | " << setw(12) << profit;

        if (p.busted) {
            cout << " | " << setw(12) << p.bust_hand
                 << " | BUSTED\n";
        } else {
            cout << " | " << setw(12) << "SURVIVED"
                 << " | ALIVE\n";
        }

        total_chip += p.chip;
    }

    cout << "====================================================================================================================\n";
    cout << "TONG CHIP: " << total_chip << "\n";
    cout << "====================================================================================================================\n";

    struct StrategyStats {
    int bot_count = 0;
    int total_hands = 0;
    int total_wins = 0;
    long long total_init_chip = 0;
    long long total_final_chip = 0;
    long long total_profit = 0;
    int busted_count = 0;
    int survived_count = 0;
};

map<AIType, StrategyStats> strategy_stats;

for (const auto& p : players) {
    StrategyStats& s = strategy_stats[p.ai_type];

    s.bot_count++;
    s.total_hands += p.hands_played;
    s.total_wins += p.wins;
    s.total_init_chip += p.init_chip;
    s.total_final_chip += p.chip;
    s.total_profit += (p.chip - p.init_chip);

    if (p.busted) {
        s.busted_count++;
    } else {
        s.survived_count++;
    }
}

cout << "\n========================================================================================\n";
cout << "                         AVERAGE RESULT BY STRATEGY\n";
cout << "========================================================================================\n";

cout << left
     << setw(14) << "Strategy"
     << " | " << setw(9) << "Bot Count"
     << " | " << setw(12) << "Total Hands"
     << " | " << setw(10) << "Total Wins"
     << " | " << setw(12) << "Avg WR"
     << " | " << setw(14) << "Total Profit"
     << " | " << setw(10) << "Survived"
     << " | " << "Busted\n";

cout << "----------------------------------------------------------------------------------------\n";

for (const auto& item : strategy_stats) {
    AIType type = item.first;
    const StrategyStats& s = item.second;

    double avg_wr = 0.0;

    if (s.total_hands > 0) {
        avg_wr = (double)s.total_wins * 100.0 / s.total_hands;
    }

    cout << left
         << setw(14) << aiTypeToString(type)
         << " | " << setw(9) << s.bot_count
         << " | " << setw(12) << s.total_hands
         << " | " << setw(10) << s.total_wins
         << " | " << fixed << setprecision(2) << setw(11) << avg_wr << "%"
         << " | " << setw(14) << s.total_profit
         << " | " << setw(10) << s.survived_count
         << " | " << s.busted_count << "\n";
}

cout << "========================================================================================\n";
}

void Game::playGames(int n_games) {
    num_games = n_games;

    for (current_game = 0; current_game < num_games; ++current_game) {
        if (countPlayersWithChips() <= 1) {
            break;
        }

        initSingleGame();

        dealPreflop();

        if (countActivePlayers() > 1) {
            dealFlop();
        }

        if (countActivePlayers() > 1) {
            dealTurn();
        }

        if (countActivePlayers() > 1) {
            dealRiver();
        }

        showdown();
        updateBustStatus();

        int next_dealer = getNextPlayerWithChips(dealer_idx + 1);

        if (next_dealer != -1) {
            dealer_idx = next_dealer;
        }
    }

    int actual_games = current_game;

    printFinalStatistics(actual_games, num_games);
    Statistics::exportToCSV(players, actual_games, num_games);
}