#ifndef POKER_H_INCLUDED
#define POKER_H_INCLUDED

#include <bits/stdc++.h>

using namespace std;

// --- Enums ---
enum HandRankEnum {
    HIGH_CARD, PAIR, TWO_PAIRS, THREE_OF_A_KIND, STRAIGHT, FLUSH,
    FULL_HOUSE, FOUR_OF_A_KIND, STRAIGHT_FLUSH, ROYAL_FLUSH
};

enum AIType { BRAINROT, CHOICONAO, CHOIANTOAN };

// --- Structs ---
struct Card {
    int rank; // 2-14 (14 là Át)
    char suit; // 'H', 'D', 'C', 'S' (Cơ, Rô, Chuồn, Bích)

    bool operator<(const Card& other) const {
        return rank < other.rank;
    }
    bool operator==(const Card& other) const {
        return rank == other.rank && suit == other.suit;
    }
    string toString() const;
};

struct HandResult {
    HandRankEnum rank;
    vector<int> tie_breakers; // Dùng để so sánh bài bằng nhau (VD: kicker)

    bool operator<(const HandResult& other) const {
        if (rank != other.rank) return rank < other.rank;
        for (size_t i = 0; i < tie_breakers.size(); ++i) {
            if (i >= other.tie_breakers.size()) return false;
            if (tie_breakers[i] != other.tie_breakers[i])
                return tie_breakers[i] < other.tie_breakers[i];
        }
        return false;
    }
};

// --- Forward Declarations ---
class Game;
class Player;

// --- Classes ---
class Deck {
private:
    vector<Card> cards;
    mt19937 rng;
public:
    Deck();
    void shuffle();
    Card draw();
    void removeKnown(const vector<Card>& known_cards); // Hỗ trợ Monte Carlo
};

class Player {
public:
    string name;
    vector<Card> hand;
    long long chip;
    long long init_chip;  // Lưu chip ban đầu
    long long bet_amount;
    bool active;
    AIType ai_type;
    int wins;
    long long chip_won;

    Player(string n, long long c, AIType ai);

    void fold();
    void check();
    void call(long long amount);
    void raise(long long to_call, long long raise_amount);
    void all_in();
    void resetForNewRound();
};

class HandEvaluator {
public:
    static HandResult evaluate(const vector<Card>& hand, const vector<Card>& board);
};

class Tactics {
public:
    static double estimateWinRate(const vector<Card>& hand, const vector<Card>& board);
    static void decide(Player& p, Game& game, long long to_call, long long highest_bet);
private:
    static void decide_brainrot(Player& p, long long to_call, long long highest_bet);
    static void decide_choiconao(Player& p, const vector<Card>& board, long long to_call, long long highest_bet);
    static void decide_choiantoan(Player& p, const vector<Card>& board, long long to_call, long long highest_bet);
};

class Pot {
public:
    long long total_money;
    Pot() : total_money(0) {}
    void collect(long long amount);
    void clear();
};

class BettingRound {
public:
    static void playRound(Game& game);
};

class Statistics {
public:
    static void exportToCSV(const vector<Player>& players, int num_games);
};

class Game {
public:
    vector<Player> players;
    vector<Card> board;
    Deck deck;
    Pot pot;
    int dealer_idx;
    int num_games;
    int current_game;
    long long highest_bet;
    long long current_blind;

    Game(int p_count, const vector<long long>& init_chips, const vector<AIType>& ai_types);

    void playGames(int n_games);
    void initSingleGame();
    void dealPreflop();
    void dealFlop();
    void dealTurn();
    void dealRiver();
    void showdown();
    int countActivePlayers();
    vector<int> getActivePlayers();
};
#endif // POKER_H_INCLUDED
