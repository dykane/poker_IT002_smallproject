#include "poker.h"
#include<iostream>

Deck::Deck() {
    random_device rd;
    rng = mt19937(rd());
    string suits = "HDCS";
    for (char s : suits) {
        for (int r = 2; r <= 14; ++r) {
            cards.push_back({r, s});
        }
    }
}

void Deck::shuffle() {
    cards.clear();
    string suits = "HDCS";
    for (char s : suits) {
        for (int r = 2; r <= 14; ++r) cards.push_back({r, s});
    }
    std::shuffle(cards.begin(), cards.end(), rng);
}

Card Deck::draw() {
    if (cards.empty()) return {2, 'H'}; // Fallback
    Card c = cards.back();
    cards.pop_back();
    return c;
}

void Deck::removeKnown(const vector<Card>& known_cards) {
    auto it = remove_if(cards.begin(), cards.end(), [&](const Card& c) {
        for (const auto& kc : known_cards) if (c == kc) return true;
        return false;
    });
    cards.erase(it, cards.end());
}
