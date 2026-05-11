#include "poker.h"

int main() {
    srand(time(0));

    int num_players, num_games;
    long long init_chips;

    cout << "So luong nguoi choi (2-10): ";
    cin >> num_players;
    num_players = max(2, min(10, num_players));

    cout << "So chip ban dau: ";
    cin >> init_chips;

    cout << "So van choi (n): ";
    cin >> num_games;

    // Đã xóa phần hỏi target_player ở đây

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

    // Cập nhật lời gọi constructor (chỉ còn 3 tham số)
    Game poker(num_players, init_chips, player_tactics);
    poker.playGames(num_games);

    return 0;
}
