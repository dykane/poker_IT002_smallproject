#include "poker.h"

int main() {
    srand(time(0));

    int num_players, num_games;

    cout << "So luong nguoi choi (2-10): ";
    cin >> num_players;
    num_players = max(2, min(10, num_players));

    // --- NHẬP SỐ CHIP CHO TỪNG NGƯỜI CHƠI ---
    vector<long long> init_chips(num_players);
    cout << "\n--- NHAP SO CHIP BAN DAU CHO TUNG NGUOI CHOI ---\n";
    for (int i = 0; i < num_players; ++i) {
        cout << "Nhap so chip cho Player_" << i << ": ";
        cin >> init_chips[i];
        if (init_chips[i] < 10) init_chips[i] = 10; // Tối thiểu 10 chip
    }
    cout << "------------------------------------------------------\n";

    cout << "So van choi (n): ";
    cin >> num_games;

    vector<AIType> player_tactics;
    cout << "\n--- CHON CHIEN THUAT (TACTICS) CHO TUNG NGUOI CHOI ---\n";
    cout << "0: BRAINROT   (Choi ngau nhien, thich an theo)\n";
    cout << "1: CHOICONAO  (Dung xac suat Monte Carlo, biet slowplay)\n";
    cout << "2: CHOIANTOAN (Giua von la chinh, gap bet to la chay)\n";

    for (int i = 0; i < num_players; ++i) {
        int choice;
        cout << "Chon AI cho Player_" << i << " (0/1/2): ";
        cin >> choice;

        if (choice == 1) player_tactics.push_back(CHOICONAO);
        else if (choice == 2) player_tactics.push_back(CHOIANTOAN);
        else player_tactics.push_back(BRAINROT);
    }
    cout << "------------------------------------------------------\n";

    Game poker(num_players, init_chips, player_tactics);
    poker.playGames(num_games);

    return 0;
}
