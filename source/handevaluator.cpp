#include "poker.h"

int HandEvaluator::evaluate(vector<Card> cards) {
    if (cards.empty()) return 0;

    map<int, int> rank_counts;
    map<char, vector<Card>> suits;
    for (Card c : cards) {
        rank_counts[c.rank]++;
        suits[c.suit].push_back(c);
    }

    // 1. Tìm Thùng (Flush)
    vector<Card> flush_cards;
    for (auto const& [suit, s_cards] : suits) {
        if (s_cards.size() >= 5) {
            flush_cards = s_cards;
            // Sắp xếp các lá đồng chất giảm dần
            sort(flush_cards.begin(), flush_cards.end(), [](const Card& a, const Card& b){ return a.rank > b.rank; });
            break;
        }
    }

    // Hàm lambda hỗ trợ tìm Sảnh (Straight)
    auto get_straight_high = [](vector<Card> h) -> int {
        if (h.size() < 5) return 0;
        vector<int> u; // Chứa các rank duy nhất
        for (Card c : h) {
            if (u.empty() || u.back() != c.rank) u.push_back(c.rank);
        }
        if (u.size() < 5) return 0;

        for (size_t i = 0; i <= u.size() - 5; i++) {
            if (u[i] - u[i+4] == 4) return u[i]; // Có sảnh 5 lá
        }
        // Kiểm tra sảnh A-2-3-4-5
        if (u[0] == 14) {
            bool has2=false, has3=false, has4=false, has5=false;
            for (int r : u) {
                if(r==2) has2=true; if(r==3) has3=true;
                if(r==4) has4=true; if(r==5) has5=true;
            }
            if (has2 && has3 && has4 && has5) return 5; // Sảnh kết thúc bằng 5
        }
        return 0;
    };

    // 9. Sảnh đồng chất (Straight Flush)
    int sf_high = 0;
    if (!flush_cards.empty()) sf_high = get_straight_high(flush_cards);
    if (sf_high > 0) return (9 << 20) + (sf_high << 16);

    // Chuẩn bị đếm Tứ quý, Xám cô, Đôi
    vector<int> quads, trips, pairs, singles;
    for (int r = 14; r >= 2; r--) { // Quét từ A xuống 2 để luôn lấy rank to nhất trước
        if (rank_counts[r] == 4) quads.push_back(r);
        else if (rank_counts[r] == 3) trips.push_back(r);
        else if (rank_counts[r] == 2) pairs.push_back(r);
        else if (rank_counts[r] == 1) singles.push_back(r);
    }

    // 8. Tứ quý (Four of a Kind)
    if (!quads.empty()) {
        int q = quads[0], kicker = 0;
        for (int r = 14; r >= 2; r--) {
            if (rank_counts[r] > 0 && r != q) { kicker = r; break; }
        }
        return (8 << 20) + (q << 16) + (kicker << 12);
    }

    // 7. Cù lũ (Full House)
    if (!trips.empty() && (trips.size() > 1 || !pairs.empty())) {
        int t = trips[0], p = 0;
        if (trips.size() > 1) p = trips[1];
        else p = pairs[0];
        return (7 << 20) + (t << 16) + (p << 12);
    }

    // 6. Thùng (Flush)
    if (!flush_cards.empty()) {
        return (6 << 20) + (flush_cards[0].rank << 16) + (flush_cards[1].rank << 12) +
               (flush_cards[2].rank << 8) + (flush_cards[3].rank << 4) + flush_cards[4].rank;
    }

    // 5. Sảnh (Straight)
    vector<Card> all_sorted = cards;
    sort(all_sorted.begin(), all_sorted.end(), [](const Card& a, const Card& b){ return a.rank > b.rank; });
    int straight_high = get_straight_high(all_sorted);
    if (straight_high > 0) return (5 << 20) + (straight_high << 16);

    // 4. Xám cô (Three of a Kind)
    if (!trips.empty()) {
        int t = trips[0];
        vector<int> kickers;
        for (int r = 14; r >= 2; r--) {
            if (rank_counts[r] > 0 && r != t) kickers.push_back(r);
        }
        while(kickers.size() < 2) kickers.push_back(0); // Tránh lỗi
        return (4 << 20) + (t << 16) + (kickers[0] << 12) + (kickers[1] << 8);
    }

    // 3. Hai Đôi (Two Pair)
    if (pairs.size() >= 2) {
        int p1 = pairs[0], p2 = pairs[1], kicker = 0;
        for (int r = 14; r >= 2; r--) {
            if (rank_counts[r] > 0 && r != p1 && r != p2) { kicker = r; break; }
        }
        return (3 << 20) + (p1 << 16) + (p2 << 12) + (kicker << 8);
    }

    // 2. Một Đôi (One Pair)
    if (pairs.size() == 1) {
        int p = pairs[0];
        vector<int> kickers;
        for (int r = 14; r >= 2; r--) {
            if (rank_counts[r] > 0 && r != p) kickers.push_back(r);
        }
        while(kickers.size() < 3) kickers.push_back(0);
        return (2 << 20) + (p << 16) + (kickers[0] << 12) + (kickers[1] << 8) + (kickers[2] << 4);
    }

    // 1. Mậu Thầu (High Card)
    vector<int> kickers;
    for (int r = 14; r >= 2; r--) {
        if (rank_counts[r] > 0) kickers.push_back(r);
    }
    while(kickers.size() < 5) kickers.push_back(0);
    return (1 << 20) + (kickers[0] << 16) + (kickers[1] << 12) + (kickers[2] << 8) + (kickers[3] << 4) + kickers[4];
}
