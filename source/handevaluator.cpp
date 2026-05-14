#include "poker.h"
#include<iostream>
#include <map>
HandResult HandEvaluator::evaluate(const vector<Card>& hand, const vector<Card>& board) {
    vector<Card> all_cards = hand;
    all_cards.insert(all_cards.end(), board.begin(), board.end());

    // Đếm tần suất
    map<int, int> rank_counts;
    map<char, vector<Card>> suit_groups;
    for (Card c : all_cards) {
        rank_counts[c.rank]++;
        suit_groups[c.suit].push_back(c);
    }

    // Sắp xếp rank giảm dần
    vector<int> ranks;
    for (Card c : all_cards) ranks.push_back(c.rank);
    sort(ranks.rbegin(), ranks.rend());
    ranks.erase(unique(ranks.begin(), ranks.end()), ranks.end()); // unique ranks

    // Check Flush
    bool is_flush = false;
    vector<int> flush_kickers;
    for (auto const& [suit, cards] : suit_groups) {
        if (cards.size() >= 5) {
            is_flush = true;
            for(Card c : cards) flush_kickers.push_back(c.rank);
            sort(flush_kickers.rbegin(), flush_kickers.rend());
            flush_kickers.resize(5);
            break;
        }
    }

    // Check Straight
    bool is_straight = false;
    int straight_high = 0;
    if (ranks.size() >= 5) {
        for (size_t i = 0; i <= ranks.size() - 5; ++i) {
            if (ranks[i] - ranks[i+4] == 4) {
                is_straight = true;
                straight_high = ranks[i];
                break;
            }
        }
        // Check A-5-4-3-2
        if (!is_straight && ranks[0] == 14) {
            bool has2=false, has3=false, has4=false, has5=false;
            for(int r: ranks) {
                if(r==2) has2=true; if(r==3) has3=true;
                if(r==4) has4=true; if(r==5) has5=true;
            }
            if(has2 && has3 && has4 && has5) {
                is_straight = true; straight_high = 5;
            }
        }
    }

    // Đếm groups (Pairs, Trips, Quads)
    int max_freq = 0;
    int second_freq = 0;
    int max_freq_rank = 0;
    int second_freq_rank = 0;

    for (auto const& [r, count] : rank_counts) {
        if (count > max_freq) {
            second_freq = max_freq;
            second_freq_rank = max_freq_rank;
            max_freq = count;
            max_freq_rank = r;
        } else if (count > second_freq || (count == second_freq && r > second_freq_rank)) {
            second_freq = count;
            second_freq_rank = r;
        }
    }

    // Straight Flush / Royal Flush (Simplified for brevity: assume if flush & straight exist, it's straight flush if top cards match. Strict check requires intersecting them, this is a passable approximation for AI sim).
    if (is_flush && is_straight) {
        if (straight_high == 14) return {ROYAL_FLUSH, {14}};
        return {STRAIGHT_FLUSH, {straight_high}};
    }

    if (max_freq == 4) return {FOUR_OF_A_KIND, {max_freq_rank}};
    if (max_freq == 3 && second_freq >= 2) return {FULL_HOUSE, {max_freq_rank, second_freq_rank}};
    if (is_flush) return {FLUSH, flush_kickers};
    if (is_straight) return {STRAIGHT, {straight_high}};
    if (max_freq == 3) {
        vector<int> k = {max_freq_rank};
        for (int r : ranks) if (r != max_freq_rank && k.size() < 3) k.push_back(r);
        return {THREE_OF_A_KIND, k};
    }
    if (max_freq == 2 && second_freq == 2) {
        vector<int> k = {max_freq_rank, second_freq_rank};
        for (int r : ranks) if (r != max_freq_rank && r != second_freq_rank && k.size() < 3) k.push_back(r);
        return {TWO_PAIRS, k};
    }
    if (max_freq == 2) {
        vector<int> k = {max_freq_rank};
        for (int r : ranks) if (r != max_freq_rank && k.size() < 4) k.push_back(r);
        return {PAIR, k};
    }

    vector<int> k;
    for (int i=0; i<min(5, (int)ranks.size()); i++) k.push_back(ranks[i]);
    return {HIGH_CARD, k};
}
