#include "poker.h"

mt19937 Tactics::rng(time(0));
bool Tactics::check_consecutive(vector<Card>cards,int req){
    if(cards.size()<req) return false;
    sort(cards.begin(),cards.end());
    int max_consec=1,current=1;
    for(size_t i=1;i<cards.size();i++) {
        if(cards[i].rank==cards[i-1].rank) continue;
        if(cards[i].rank==cards[i-1].rank+1){
            current++;
            max_consec=max(max_consec,current);
        } else {
            current=1;
        }
    }
    return max_consec>=req;
}
bool Tactics::check_suited(vector<Card> cards,int req) {
    map<char,int> m;
    for(Card c:cards){
        m[c.suit]++;
        if(m[c.suit]>=req) return true;
    }
    return false;
}
void Tactics::brainrot(Player& p,long long calling){
    if(calling>0) p.call(calling);
    else p.call(0);
}

void Tactics::choiantoan(Player& p, long long calling) {
    if (p.bet_amount + calling > p.chip / 2) {
        p.fold();
    } else {
        p.call(calling);
    }
}

void Tactics::choiconao(Player& p,long long calling,const vector<Card>& board){
    vector<Card> all_cards=p.hand;
    all_cards.insert(all_cards.end(),board.begin(),board.end());
    int j=all_cards.size();
    auto rand_pct=[](){ return uniform_int_distribution<int>(1, 100)(rng); };
    if (j==2){
        vector<Card> h=p.hand;
        sort(h.begin(),h.end()); // h[0] <= h[1]
        int diff=h[1].rank-h[0].rank;
        bool suited=(h[0].suit==h[1].suit);
        if(diff>4){
            if(h[1].rank==14)p.all_in();
            else if(h[1].rank==13&&rand_pct()>=20) p.call(calling); // >= 80% theo
            else if(h[1].rank==12&&rand_pct()>=50) p.call(calling); // >= 50% theo
            else if(h[1].rank==11&&rand_pct()>=75) p.call(calling); // >= 25% theo
            else p.fold();
        } else{
            if(suited){
                if(h[1].rank==14) p.all_in();
                else if(h[1].rank==13&&rand_pct()>=20) p.call(calling);
                else if(h[1].rank==12&&rand_pct()>=50) p.call(calling);
                else if(h[1].rank==11&&rand_pct()>=75) p.call(calling);
                else p.fold();
            } else{
                if(h[1].rank < 10) p.fold();
                else if(h[1].rank>=11) p.call(calling);
                else p.fold(); // Cho trường hợp bằng 10 không nằm trong rule, default fold
            }
        }
    }
    else if(j==5){
        int str=HandEvaluator::evaluate(all_cards);
        if(check_suited(all_cards,4)||check_consecutive(all_cards,4)||str>=2) p.call(calling);
        else p.fold();
    }
    else if(j==6){
        int str=HandEvaluator::evaluate(all_cards);
        if(check_suited(all_cards,4)||check_consecutive(all_cards,4)||str>=3) p.call(calling);
        else if(str==2) { if (rand_pct()<=50) p.call(calling); else p.fold(); }
        else p.fold();
    }
    else if(j==7){
        int str=HandEvaluator::evaluate(all_cards);
        if(str>=3) p.call(calling);
        else if(str==2) { if (rand_pct()<=20) p.call(calling); else p.fold(); }
        else p.fold();
    }
}
void Tactics::execute(Player& p,int strategy_type,long long calling,const vector<Card>& board) {
    if(p.is_all_in||!p.active) return;
    if(strategy_type==1) brainrot(p,calling); // Player 1, 2
    else if(strategy_type==2) choiantoan(p,calling); // Player 3, 4
    else choiconao(p,calling,board); // Player 5
}
