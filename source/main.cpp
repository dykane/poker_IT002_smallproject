#include "poker.h"

int main() {
    int n;
    cout << "Nhap so van poker muon mo phong (n): ";
    if (!(cin >> n)) n = 1000;
    Game game;
    game.play_game(n);
    return 0;
}
