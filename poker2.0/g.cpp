#include "poker.h"
#include <fstream>
using namespace std;

vector<int> wins(11, 0); // wins[1] đến wins[10]

void Game::setup_players() {
    players.clear();
    int num_players;
    cout << "Nhap so nguoi choi (1-10): ";
    cin >> num_players;
    if (num_players < 1 || num_players > 10) num_players = 5;

    cout << "Nhap chip ban dau cho tung nguoi choi (cach nhau boi dau cach): ";
    vector<long long> chips(num_players);
    for (int i = 0; i < num_players; i++) {
        cin >> chips[i];
    }

    cout << "Nhap strategy cho tung nguoi choi (1=always_call, 2=choiantoan, 3=choiconao): ";
    vector<int> strategies(num_players);
    for (int i = 0; i < num_players; i++) {
        cin >> strategies[i];
        if (strategies[i] < 1 || strategies[i] > 3) strategies[i] = 1;
    }

    for (int i = 0; i < num_players; i++) {
        players.push_back(Player(i + 1, chips[i], strategies[i]));
    }

    cout << "\n=== CAU HINH NGUOI CHOI ===\n";
    for (int i = 0; i < num_players; i++) {
        string strat_name = (players[i].strategy == 1) ? "Always Call" :
                           (players[i].strategy == 2) ? "Choi An Toan" : "Choi Thong Minh";
        cout << "Player " << players[i].id << ": " << chips[i] << " chips, Strategy: " << strat_name << endl;
    }
    cout << "============================\n\n";
}

Game::Game() {}

void Game::determine_winners(vector<Player*>& active_players) {
    if (active_players.size() == 1) {
        active_players[0]->chip += pot.total;
        return;
    }

    int best_score = -1;
    vector<Player*> winners;

    for (Player* p : active_players) {
        vector<Card> full_hand = p->hand;
        full_hand.insert(full_hand.end(), board.begin(), board.end());
        int score = HandEvaluator::evaluate(full_hand);

        if (score > best_score) {
            best_score = score;
            winners.clear();
            winners.push_back(p);
        } else if (score == best_score) {
            winners.push_back(p); // Split pot đơn giản
        }
    }

    long long split_amount = pot.total / winners.size();
    for (Player* w : winners) w->chip += split_amount;
}

void Game::play_hand() {
    deck = Deck();
    deck.shuffle_deck();
    board.clear();
    pot.reset();

    // Reset players cho ván mới (KHÔNG reset chip về initial để chơi thật)
    for(Player& p : players) {
        p.reset_for_new_hand();
        if (p.chip > 0) {
            p.hand.push_back(deck.draw());
            p.hand.push_back(deck.draw());
        }
    }

    if (players.empty()) return;

    // Preflop blinds (chỉ nếu có đủ người)
    int num_active = 0;
    for (Player& p : players) if (p.chip > 0) num_active++;
    if (num_active >= 2) {
        players[0].call(10); // Small blind
        if (players.size() > 1) players[1].call(20); // Big blind
    }

    vector<string> stages = {"Preflop", "Flop", "Turn", "River"};
    for(int i=0; i<4; i++) {
        if (i == 1) {
            board.push_back(deck.draw());
            board.push_back(deck.draw());
            board.push_back(deck.draw());
        }
        else if (i == 2 || i == 3) {
            board.push_back(deck.draw());
        }

        BettingRound::play(players, board, pot.total);
        pot.collect_bets(players);

        int active_count = 0;
        for(Player& p : players) if(p.active && p.chip > 0) active_count++;
        if(active_count <= 1) break;
    }

    vector<Player*> active_players;
    for(Player& p : players) if(p.active && p.chip > 0) active_players.push_back(&p);
    if (!active_players.empty()) {
        determine_winners(active_players);
    }
}

void Game::play_game(int num_games) {
    setup_players();

    int n;
    cout << "Nhap so van poker muon mo phong: ";
    if (!(cin >> n)) n = 1000;

    ofstream outFile("poker_results.csv");
    outFile << "Game,Winner1,Winner2,Winner3,Winner4,Winner5,Winner6,Winner7,Winner8,Winner9,Winner10" << endl;

    for (int i = 0; i < n; i++) {
        play_hand();

        // Tìm người có nhiều chip nhất sau ván đấu để đếm win
        long long max_c = -1;
        int winner_id = -1;
        for(Player& p : players) {
            if(p.chip > max_c) {
                max_c = p.chip;
                winner_id = p.id;
            }
        }

        if (winner_id != -1) {
            wins[winner_id]++;

            // Ghi CSV
            outFile << (i+1);
            for (int j = 1; j <= 10; j++) {
                outFile << "," << (j == winner_id ? 1 : 0);
            }
            outFile << endl;
        }
    }
    outFile.close();

    cout << "\n=== KET QUA CUOI CUNG (sau " << n << " van) ===\n";
    for (Player& p : players) {
        string strat_name = (p.strategy == 1) ? "Always Call" :
                           (p.strategy == 2) ? "Choi An Toan" : "Choi Thong Minh";
        cout << "Player " << p.id << " (" << strat_name << "): " << wins[p.id] << " thang ("
             << fixed << setprecision(1) << (double)wins[p.id]*100/n << "%)\n";
    }
    cout << "======================================\n";
}
