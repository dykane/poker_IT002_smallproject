#ifndef POKER_H_INCLUDED
#define POKER_H_INCLUDED

#include <bits/stdc++.h>

using namespace std;

// 1. Card
class Card {
public:
    int rank;
    char suit;
    Card(int r, char s);
    bool operator<(const Card& other) const;
};

// 2. Deck
class Deck {
private:
    vector<Card> cards;
public:
    Deck();
    void shuffle_deck();
    Card draw();
};

// 3. Player
class Player {
public:
    int id;
    vector<Card> hand;
    long long chip;
    long long bet_amount;
    bool active;
    bool is_all_in;
    int strategy; // 1: brainrot, 2: choiantoan, 3: choiconao
    Player(int _id, long long initial_chip, int _strategy);
    void reset_for_new_hand();
    void fold();
    void call(long long amount);
    void raise(long long calling, long long raise_amount);
    void all_in();
};

// 5. HandEvaluator
class HandEvaluator {
public:
    static int evaluate(vector<Card> cards);
};

// 4. Tactics (AI)
class Tactics {
public:
    static mt19937 rng;
    static void execute(Player& p, long long to_call, const vector<Card>& board);
private:
    static void brainrot(Player& p, long long calling);
    static void choiantoan(Player& p, long long calling);
    static void choiconao(Player& p, long long to_call, const vector<Card>& board);
    static bool check_consecutive(vector<Card> cards, int req);
    static bool check_suited(vector<Card> cards, int req);
};

// 6. BettingRound
class BettingRound {
public:
    static void play(vector<Player>& players, vector<Card>& board, long long& pot);
};

// 7. Pot
class Pot {
public:
    long long total;
    Pot();
    void collect_bets(vector<Player>& players);
    void reset();
};

// 8. Game
class Game {
private:
    vector<Player> players;
    Deck deck;
    vector<Card> board;
    Pot pot;
public:
    Game();
    void setup_players();
    void play_game(int num_games);
    void play_hand();
    void determine_winners(vector<Player*>& active_players);
};

#endif // POKER_H_INCLUDED
