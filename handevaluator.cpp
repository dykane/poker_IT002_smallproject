#include "poker.h"

int HandEvaluator::evaluate(vector<Card> cards){
    if(cards.empty()) return 1;
    sort(cards.begin(),cards.end());
    map<int,int>rank_counts;
    map<char,int>suit_counts;
    for(Card c:cards){
        rank_counts[c.rank]++;
        suit_counts[c.suit]++;
    }
    bool is_flush=false;
    for(auto const&[suit, count]:suit_counts){
        if(count >= 5)is_flush=true;
    }
    bool is_straight=false;
    vector<int> unique_ranks;
    for(auto const&[r, c]:rank_counts) unique_ranks.push_back(r);
    sort(unique_ranks.begin(),unique_ranks.end());
    int consec=1;
    for(size_t i=1;i<unique_ranks.size();i++){
        if(unique_ranks[i]==unique_ranks[i-1]+1){
            consec++;
            if(consec>=5)is_straight=true;
        } else{
            consec=1;
        }
    }
    // Check A-2-3-4-5 straight
    if (rank_counts.count(14)&&rank_counts.count(2)&&rank_counts.count(3)&&rank_counts.count(4)&&rank_counts.count(5)){
        is_straight=true;
    }
    int max_freq=0;
    int pairs=0;
    int trips=0;
    for (auto const&[r, count]:rank_counts){
        max_freq = max(max_freq, count);
        if (count == 2) pairs++;
        if (count == 3) trips++;
    }
    if (max_freq==4) return 8; // Quads
    if ((trips>=1&&pairs>=1)||trips>=2) return 7; // Full House
    if (is_flush) return 6; // Flush
    if (is_straight) return 5; // Straight
    if (trips>=1) return 4; // Trips
    if (pairs>=2) return 3; // Two Pair
    if (pairs==1) return 2; // Pair
    return 1; // High Card
}
