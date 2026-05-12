#include "poker.h"
#include <fstream>

// Lưu lịch sử chip theo từng vòng chơi
static vector<vector<long long>> chip_history;

void Statistics::recordRound(const vector<Player>& players) {
    vector<long long> round_chips;
    for (const auto& p : players) {
        round_chips.push_back(p.chip);
    }
    chip_history.push_back(round_chips);
}

void Statistics::exportToCSV(const vector<Player>& players, int num_games) {
    // File thống kê chung
    ofstream file("poker_statistics.csv");

    if (!file.is_open()) {
        cerr << "Loi: Khong the tao file CSV!\n";
        return;
    }

    file << "Nguoi Choi,So Van Thang,Chip Ban Dau,Chip Hien Tai,Chip Thang,Chip Thua,Thay Doi\n";

    long long total_initial = 0;
    long long total_final = 0;

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

    file << "TONG CONG,," << total_initial << "," << total_final << ",,,\n";
    file.close();

    // File lịch sử chi tiết (chip theo từng vòng)
    ofstream history_file("poker_history.csv");
    if (!history_file.is_open()) {
        cerr << "Loi: Khong the tao file history!\n";
        return;
    }

    // Header: Van,Player_0,Player_1,Player_2,...
    history_file << "Van";
    for (const auto& p : players) {
        history_file << "," << p.name;
    }
    history_file << "\n";

    // Dữ liệu: số vòng, chip của mỗi người
    for (size_t round = 0; round < chip_history.size(); ++round) {
        history_file << round + 1;
        for (long long chip : chip_history[round]) {
            history_file << "," << chip;
        }
        history_file << "\n";
    }

    history_file.close();
    cout << "\n✓ Thong ke da duoc xuat ra file: poker_statistics.csv\n";
    cout << "✓ Lich su chi tiet da duoc xuat ra file: poker_history.csv\n";
}
