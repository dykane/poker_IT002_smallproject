#include "poker.h"

Player::Player(int _id, long long initial_chip, int _strategy){
    id=_id;
    chip=initial_chip;
    bet_amount=0;
    active=true;
    is_all_in=false;
    strategy=_strategy;
}

void Player::reset_for_new_hand(){
    hand.clear();
    bet_amount=0;
    if(chip>0)active=true;
    else active=false;
    is_all_in=(chip==0);
}

void Player::fold(){
    active=false;
}

void Player::call(long long amount){
    long long callin=min(chip,amount);
    chip-=callin;
    bet_amount+=callin;
    if (chip==0) is_all_in = true;
}

void Player::raise(long long calling,long long raise_amount){
    long long total=calling+raise_amount;
    if(total>=chip){
        all_in();
    } else{
        chip-=total;
        bet_amount+=total;
    }
}

void Player::all_in(){
    bet_amount+=chip;
    chip=0;
    is_all_in=true;
}
