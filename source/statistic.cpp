#include "poker.h"
#include <fstream>

void Statistics::exportToCSV(const vector<Player>& players, int num_games) {
    ofstream file("poker_statistics.csv");

    if (!file.is_open()) {
        cerr << "Loi: Khong the tao file CSV!\n";
        return;
    }

    // Ghi header
    file << "Nguoi Choi,So Van Thang,Chip Ban Dau,Chip Hien Tai,Chip Thang,Chip Thua,Thay Doi\n";

    long long total_initial = 0;
    long long total_final = 0;

    // Ghi dữ liệu từng người chơi
    for (const auto& p : players) {
        long long chip_loss = p.init_chip - p.chip;
        long long change = p.chip - p.init_chip;

        file << p.name << ","
             << p.wins << ","
             << p.init_chip << ","
             << p.chip << ","
             << p.chip_won << ","
             << chip_loss << ","
             << change << "\n";

        total_initial += p.init_chip;
        total_final += p.chip;
    }

    // Tổng cộng
    file << "TONG CONG,"
         << ","
         << total_initial << ","
         << total_final << ","
         << ","
         << ","
         << (total_final - total_initial) << "\n";

    file.close();
    cout << "\n Thong ke da duoc xuat ra file: poker_statistics.csv\n";
}
