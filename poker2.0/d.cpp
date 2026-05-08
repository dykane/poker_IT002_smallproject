#include "poker.h"

Deck::Deck() {
    char suits[]={'H','D','C','S'};
    for(char s:suits) {
        for(int r=2;r<=14;++r){
            cards.push_back(Card(r,s));
        }
    }
}
void Deck::shuffle_deck(){
    shuffle(cards.begin(),cards.end(),Tactics::rng);
}
Card Deck::draw(){
    if(cards.empty())throw runtime_error("Deck is empty");
    Card c=cards.back();
    cards.pop_back();
    return c;
}
