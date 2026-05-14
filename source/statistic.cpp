#include "poker.h"
#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

void Statistics::exportToCSV(const vector<Player>& players, int actual_games, int max_games) {
    ofstream file("poker_statistics.csv");

    if (!file.is_open()) {
        cerr << "Loi: Khong the tao file CSV!\n";
        return;
    }

    file << "Max Hands,Actual Hands\n";
    file << max_games << "," << actual_games << "\n\n";

    file << "Nguoi Choi,Strategy,Hands Played,Wins,Hand Winrate (%),Chip Ban Dau,Chip Hien Tai,Profit,Chip Thang,Bust Hand,Status\n";

    long long total_initial = 0;
    long long total_final = 0;

    for (const auto& p : players) {
        double hand_wr = 0.0;

        if (p.hands_played > 0) {
            hand_wr = (double)p.wins * 100.0 / p.hands_played;
        }

        long long profit = p.chip - p.init_chip;

        file << p.name << ","
             << aiTypeToString(p.ai_type) << ","
             << p.hands_played << ","
             << p.wins << ","
             << fixed << setprecision(2) << hand_wr << ","
             << p.init_chip << ","
             << p.chip << ","
             << profit << ","
             << p.chip_won << ",";

        if (p.busted) {
            file << p.bust_hand << ",BUSTED\n";
        } else {
            file << "SURVIVED,ALIVE\n";
        }

        total_initial += p.init_chip;
        total_final += p.chip;
    }

    file << "\nTONG CONG,,,,,"
         << total_initial << ","
         << total_final << ","
         << (total_final - total_initial)
         << ",,,\n";

    file.close();

    cout << "\nThong ke da duoc xuat ra file: poker_statistics.csv\n";
}