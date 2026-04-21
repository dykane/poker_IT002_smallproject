#include "poker.h"

Game::Game() {
    for (int i = 1; i <= 5; ++i) {
        players.push_back(Player(i, 5000));
    }
}

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

    // Mỗi ván reset chip về 5000 để đảm bảo test sòng phẳng hoặc có thể bỏ qua nếu chơi tính loại trừ.
    // Dựa theo prompt, giả định test AI qua N ván. Ta cho họ 5000 mỗi đầu ván.
    for(Player& p : players) {
        p.chip = 5000;
        p.reset_for_new_hand();
        p.hand.push_back(deck.draw());
        p.hand.push_back(deck.draw());
    }

    // Preflop blinds
    players[0].call(10); // Small blind
    players[1].call(20); // Big blind

    vector<string> stages = {"Preflop", "Flop", "Turn", "River"};
    for(int i=0; i<4; i++) {
        if (i == 1) { board.push_back(deck.draw()); board.push_back(deck.draw()); board.push_back(deck.draw()); }
        else if (i == 2 || i == 3) { board.push_back(deck.draw()); }

        BettingRound::play(players, board, pot.total);
        pot.collect_bets(players);

        int active_count = 0;
        for(Player& p : players) if(p.active) active_count++;
        if(active_count <= 1) break;
    }

    vector<Player*> active_players;
    for(Player& p : players) if(p.active) active_players.push_back(&p);
    determine_winners(active_players);
}
void Game::play_game(int num_games) {
    int p5_wins = 0;
    int p4_wins = 0;
    int p3_wins = 0;
    int p2_wins = 0;
    int p1_wins = 0;
    for (int i = 0; i < num_games; i++) {
        play_hand();
        // Tìm người có nhiều chip nhất sau ván đấu để đếm win
        long long max_c = 0;
        int winner_id = -1;
        for(Player& p : players) {
            if(p.chip > max_c) {
                max_c = p.chip;
                winner_id = p.id;
            }
        }
        if (winner_id == 5) p5_wins++;
        if (winner_id == 4) p4_wins++;
        if (winner_id == 3) p3_wins++;
        if (winner_id == 2) p2_wins++;
        if (winner_id == 1) p1_wins++;
    }
    cout<<"nguoi choi thu 5 (Choi Co Nao) thang: " << p5_wins << " van.\n";
    cout<<"nguoi choi thu 4 (Choi An Toan) thang: " << p4_wins << " van.\n";
    cout<<"nguoi choi thu 3 (Choi An Toan) thang: " << p3_wins << " van.\n";
    cout<<"nguoi choi thu 2 (always call) thang: " << p2_wins << " van.\n";
    cout<<"nguoi choi thu 1 (always call) thang: " << p1_wins << " van.\n";
}
//#include "poker.h"
//
//Game::Game(){
//    for(int i=1;i<=5;++i) {
//        players.push_back(Player(i, 5000));
//    }
//}
////ham xac dinh nguoi thang
//void Game::determine_winners(vector<Player*>& active_players) {
//    if (active_players.size() == 1) {
//        active_players[0]->chip += pot.total;
//        return;
//    }
//    int best_score = -1;
//    vector<Player*> winners;
//    for (Player* p : active_players) {
//        vector<Card> full_hand = p->hand;
//        full_hand.insert(full_hand.end(), board.begin(), board.end());
//        int score = HandEvaluator::evaluate(full_hand);
//        if (score > best_score) {
//            best_score = score;
//            winners.clear();
//            winners.push_back(p);
//        } else if (score == best_score) {
//            winners.push_back(p);
//        }
//    }
//    long long split_amount = pot.total / winners.size();
//    for (Player* w : winners) w->chip += split_amount;
//}
//// Chơi 1 hand
//void Game::play_hand() {
//    deck = Deck();
//    deck.shuffle_deck();
//    board.clear();
//    pot.reset();
//    for(Player& p : players) {
//        p.reset_for_new_hand();
//        if (p.active) {
//            p.hand.push_back(deck.draw());
//            p.hand.push_back(deck.draw());
//        }
//    }
//    int blind_count = 0;
//    for(Player& p : players) {
//        if(p.active) {
//            if(blind_count == 0) p.call(10);
//            else if(blind_count == 1) p.call(20);
//            blind_count++;
//            if(blind_count == 2) break;
//        }
//    }
//    vector<string> stages = {"Preflop", "Flop", "Turn", "River"};
//    for(int i=0; i<4; i++) {
//        if (i == 1) { board.push_back(deck.draw()); board.push_back(deck.draw()); board.push_back(deck.draw()); }
//        else if (i == 2 || i == 3) { board.push_back(deck.draw()); }
//
//        BettingRound::play(players, board, pot.total);
//        pot.collect_bets(players);
//
//        int active_count = 0;
//        for(Player& p : players) if(p.active && !p.is_all_in) active_count++;
//
//        if(active_count <= 1 && i != 3) {
//            continue;
//        }
//    }
//    vector<Player*> active_players;
//    for(Player& p : players) if(p.active) active_players.push_back(&p);
//    if (!active_players.empty()) {
//        determine_winners(active_players);
//    }
//}
//// choi n van game
//void Game::play_game(int num_games) {
//    // Tạo mảng lưu số trận thắng của 5 người chơi, khởi tạo bằng 0
//    vector<int> wins(5, 0);
//    for (int i = 0; i < num_games; i++) {
//        for (Player& p : players) {
//            p.chip = 5000;
//        }
//        int active_players_count = 5;
//        // Thêm một giới hạn loop (ví dụ 10000 hands) để tránh trường hợp AI hòa nhau vô tận
//        int hand_count = 0;
//        while (active_players_count > 1 && hand_count < 10000) {
//            play_hand();
//            active_players_count = 0;
//            for (Player& p : players) {
//                if (p.chip > 0) active_players_count++;
//            }
//            hand_count++;
//        }
//        // Ván kết thúc, tìm người có số chip lớn nhất (người gom hết tiền)
//        int winner_id = -1;
//        long long max_chips = 0;
//        for(Player& p : players) {
//            if(p.chip > max_chips) {
//                max_chips = p.chip;
//                winner_id = p.id;
//            }
//        }
//        // Cộng 1 trận thắng cho người đó
//        if (winner_id != -1) {
//            wins[winner_id - 1]++;
//        }
//    }
//    for (int i = 0; i < 5; i++) {
//        string strategy_name = "";
//        if (i == 0 || i == 1) strategy_name = " (Brainrot)";
//        else if (i == 2 || i == 3) strategy_name = " (Choi an toan)";
//        else if (i == 4) strategy_name = " (Choi co nao)";
//        cout << "Nguoi choi " << (i + 1) << strategy_name << " thang:\t" << wins[i] << " van.\n";
//    }
//    cout << "======================================\n";
//}
