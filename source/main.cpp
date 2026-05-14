#include "poker.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <streambuf>
using namespace std;

// Dùng để tắt output chi tiết khi chạy nhiều case
class NullBuffer : public streambuf {
public:
    int overflow(int c) override {
        return c;
    }
};

struct StrategyAggregate {
    int bot_count_total = 0;
    int case_wins = 0;
    int total_hands = 0;
    int total_small_wins = 0;
    long long total_profit = 0;
    int total_survived = 0;
    int total_busted = 0;
};

int findCaseWinnerIndex(const vector<Player>& players) {
    int winner_idx = -1;
    long long best_chip = -1;

    for (int i = 0; i < (int)players.size(); ++i) {
        if (players[i].chip > best_chip) {
            best_chip = players[i].chip;
            winner_idx = i;
        }
    }

    return winner_idx;
}

void runSingleCase(
    int num_players,
    const vector<long long>& init_chips,
    const vector<AIType>& player_tactics,
    int max_games
) {
    Game poker(num_players, init_chips, player_tactics);
    poker.playGames(max_games);
}

void runMultipleCases(
    int num_players,
    const vector<long long>& init_chips,
    const vector<AIType>& player_tactics,
    int num_cases,
    int max_games_per_case
) {
    map<AIType, StrategyAggregate> aggregate;

    NullBuffer null_buffer;
    streambuf* original_cout_buffer = cout.rdbuf();

    for (int case_idx = 1; case_idx <= num_cases; ++case_idx) {
        Game poker(num_players, init_chips, player_tactics);

        // Tắt output chi tiết trong từng case để terminal không bị ngập
        cout.rdbuf(&null_buffer);
        poker.playGames(max_games_per_case);
        cout.rdbuf(original_cout_buffer);

        int winner_idx = findCaseWinnerIndex(poker.players);

        if (winner_idx != -1) {
            AIType winner_strategy = poker.players[winner_idx].ai_type;
            aggregate[winner_strategy].case_wins++;
        }

        for (const auto& p : poker.players) {
            StrategyAggregate& s = aggregate[p.ai_type];

            s.bot_count_total++;
            s.total_hands += p.hands_played;
            s.total_small_wins += p.wins;
            s.total_profit += (p.chip - p.init_chip);

            if (p.busted) {
                s.total_busted++;
            } else {
                s.total_survived++;
            }
        }

        cout << "Finished case " << case_idx << "/" << num_cases << "\n";
    }

    cout << "\n====================================================================================================\n";
    cout << "                              MULTI-CASE STRATEGY RESULT\n";
    cout << "====================================================================================================\n";
    cout << "Number of cases          : " << num_cases << "\n";
    cout << "Max hands per case       : " << max_games_per_case << "\n";
    cout << "Total player-case records: " << num_cases * num_players << "\n";
    cout << "====================================================================================================\n";

    cout << left
         << setw(14) << "Strategy"
         << " | " << setw(10) << "Case Wins"
         << " | " << setw(13) << "Final WR"
         << " | " << setw(12) << "Total Hands"
         << " | " << setw(13) << "Hand Wins"
         << " | " << setw(12) << "Hand WR"
         << " | " << setw(14) << "Avg Profit"
         << " | " << setw(10) << "Survived"
         << " | " << "Busted\n";

    cout << "----------------------------------------------------------------------------------------------------\n";

    for (const auto& item : aggregate) {
        AIType type = item.first;
        const StrategyAggregate& s = item.second;

        double final_wr = 0.0;
        double hand_wr = 0.0;
        double avg_profit = 0.0;

        if (num_cases > 0) {
            final_wr = (double)s.case_wins * 100.0 / num_cases;
        }

        if (s.total_hands > 0) {
            hand_wr = (double)s.total_small_wins * 100.0 / s.total_hands;
        }

        if (s.bot_count_total > 0) {
            avg_profit = (double)s.total_profit / s.bot_count_total;
        }

        cout << left
             << setw(14) << aiTypeToString(type)
             << " | " << setw(10) << s.case_wins
             << " | " << fixed << setprecision(2) << setw(12) << final_wr << "%"
             << " | " << setw(12) << s.total_hands
             << " | " << setw(13) << s.total_small_wins
             << " | " << fixed << setprecision(2) << setw(11) << hand_wr << "%"
             << " | " << fixed << setprecision(2) << setw(14) << avg_profit
             << " | " << setw(10) << s.total_survived
             << " | " << s.total_busted << "\n";
    }

    cout << "====================================================================================================\n";

    cout << "\nGiai thich chi so:\n";
    cout << "- Final WR  = so case ma strategy thang chung cuoc / tong so case.\n";
    cout << "- Hand WR   = tong so van nho thang cua strategy / tong so van strategy da tham gia.\n";
    cout << "- Avg Profit = tong loi nhuan cua tat ca bot cung strategy / tong so bot-case cua strategy.\n";
}

int main() {
    srand(time(0));

    int num_players;
    cout << "So luong nguoi choi (2-10): ";
    cin >> num_players;
    num_players = max(2, min(10, num_players));

    vector<long long> init_chips(num_players);

    cout << "\n--- NHAP SO CHIP BAN DAU CHO TUNG NGUOI CHOI ---\n";
    for (int i = 0; i < num_players; ++i) {
        cout << "Nhap so chip cho Player_" << i << ": ";
        cin >> init_chips[i];

        if (init_chips[i] < 10) {
            init_chips[i] = 10;
        }
    }
    cout << "------------------------------------------------------\n";

    vector<AIType> player_tactics;

    cout << "\n--- CHON CHIEN THUAT (TACTICS) CHO TUNG NGUOI CHOI ---\n";
    cout << "0: BRAINROT   (Choi ngau nhien, thich an theo)\n";
    cout << "1: CHOICONAO  (Dung xac suat Monte Carlo, biet slowplay)\n";
    cout << "2: CHOIANTOAN (Giu von la chinh, gap bet to la chay)\n";

    for (int i = 0; i < num_players; ++i) {
        int choice;

        cout << "Chon AI cho Player_" << i << " (0/1/2): ";
        cin >> choice;

        if (choice == 1) {
            player_tactics.push_back(CHOICONAO);
        } else if (choice == 2) {
            player_tactics.push_back(CHOIANTOAN);
        } else {
            player_tactics.push_back(BRAINROT);
        }
    }

    cout << "------------------------------------------------------\n";

    int mode;

    cout << "\n--- CHON CHE DO CHAY ---\n";
    cout << "1: Chay 1 simulation case\n";
    cout << "2: Chay nhieu simulation cases\n";
    cout << "Lua chon: ";
    cin >> mode;

    if (mode == 2) {
        int num_cases;
        int max_games_per_case;

        cout << "Nhap so simulation cases: ";
        cin >> num_cases;

        cout << "Nhap so van toi da moi case: ";
        cin >> max_games_per_case;

        if (num_cases < 1) {
            num_cases = 1;
        }

        if (max_games_per_case < 1) {
            max_games_per_case = 1;
        }

        runMultipleCases(
            num_players,
            init_chips,
            player_tactics,
            num_cases,
            max_games_per_case
        );
    } else {
        int max_games;

        cout << "So van toi da (n): ";
        cin >> max_games;

        if (max_games < 1) {
            max_games = 1;
        }

        runSingleCase(
            num_players,
            init_chips,
            player_tactics,
            max_games
        );
    }

    return 0;
}