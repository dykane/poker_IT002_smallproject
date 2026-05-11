#include "poker.h"

Player::Player(string n, long long c, AIType ai) {
    name = n;
    chip = c;
    ai_type = ai;
    bet_amount = 0;
    active = true;
    wins = 0;
}

void Player::fold() {
    active = false;
    cout << "   -> [" << name << "] FOLD.\n";
}

// Thêm hàm check()
void Player::check() {
    cout << "   -> [" << name << "] CHECK.\n";
}

// Cập nhật lại hàm call()
void Player::call(long long amount) {
    long long actual_call = min(chip, amount);
    chip -= actual_call;
    bet_amount += actual_call;
    cout << "   -> [" << name << "] CALL " << actual_call << " chips. (Remaining: " << chip << ")\n";
}

void Player::raise(long long to_call, long long raise_amount) {
    long long total_needed = to_call + raise_amount;
    if (chip <= total_needed) {
        all_in();
        return;
    }
    chip -= total_needed;
    bet_amount += total_needed;
    cout << "   -> [" << name << "] RAISE " << raise_amount << " (Total added: " << total_needed << ", Remaining: " << chip << ")\n";
}

void Player::all_in() {
    cout << "   -> [" << name << "] ALL-IN with " << chip << " chips!\n";
    bet_amount += chip;
    chip = 0;
}

void Player::resetForNewRound() {
    hand.clear();
    bet_amount = 0;
    if (chip > 0) active = true;
    else active = false; // Loại nếu hết tiền
}
