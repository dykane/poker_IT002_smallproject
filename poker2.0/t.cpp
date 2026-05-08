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

void Tactics::choiconao(Player& p, long long to_call, const vector<Card>& board) {
    vector<Card> all_cards = p.hand;
    all_cards.insert(all_cards.end(), board.begin(), board.end());
    int j = all_cards.size();

    auto rand_pct = []() { return uniform_int_distribution<int>(1, 100)(rng); };

    if (j == 2) {
        //preflop
        vector<Card> h = p.hand;
        sort(h.begin(), h.end()); // h[0] <= h[1]
        int rank1 = h[0].rank;
        int rank2 = h[1].rank;
        bool suited = (h[0].suit == h[1].suit);
        bool is_pair = (rank1 == rank2);

        if (is_pair) {
            if (rank2 >= 10) p.raise(to_call, 200); // Đôi lớn (10, J, Q, K, A) -> Tố mạnh
            else p.call(to_call); // Đôi nhỏ -> Theo để chờ set Xám cô
        }
        else if (rank2 >= 13 && rank1 >= 10) {
            // Bài siêu cấp (AK, AQ, KQ) -> Tố thêm
            p.raise(to_call, 150);
        }
        else if (rank2 >= 11 || (suited && rank2 - rank1 <= 4)) {
            // Bài khá (Có J, Q, K, A hoặc đồng chất liên tiếp) -> Theo bài
            p.call(to_call);
        }
        else {
            // Rác -> Bỏ (Trừ khi không ai cược thêm thì check)
            if (to_call == 0) p.call(0);
            else p.fold();
        }
    }
    else {
        //flob turn river
        int str = HandEvaluator::evaluate(all_cards);
        bool draw_flush = check_suited(all_cards, 4);
        bool draw_straight = check_consecutive(all_cards, 4);

        if (str >= 4) {
            // Rất mạnh (Trips, Sảnh, Thùng, Cù lũ, Tứ quý)
            if (rand_pct() <= 30 && j < 7) {
                p.call(to_call); // 30% cơ hội Núp lùm (Slowplay) dụ địch cược thêm
            } else {
                p.raise(to_call, 300); // 70% cơ hội Tố mạnh lấy tiền
            }
        }
        else if (str == 3) {
            // Khá mạnh (2 Đôi)
            p.raise(to_call, 100); // Tố nhẹ để ép các bài yếu hơn
        }
        else if (str == 2) {
            // Trung bình (1 Đôi)
            if (to_call > 600) {
                p.fold(); // Nếu đối thủ cược quá gắt (>600 chip), biết sợ và bỏ bài
            } else {
                p.call(to_call); // Cược nhỏ thì theo
            }
        }
        else {
            // Bài yếu (Mậu thầu - High card)
            if (j < 7 && (draw_flush || draw_straight)) {
                // Đang chờ Sảnh hoặc Thùng
                if (to_call <= 300) p.call(to_call); // Giá rẻ thì mua bài
                else p.fold(); // Đắt quá thì bỏ
            } else {
                // Không có đường phát triển
                if (to_call == 0) p.call(0); // Check nếu miễn phí
                else p.fold(); // Có cược là chạy
            }
        }
    }
}

void Tactics::execute(Player& p, long long to_call, const vector<Card>& board) {
    if (p.is_all_in || !p.active) return;

    if (p.strategy == 1) brainrot(p, to_call); // Always call
    else if (p.strategy == 2) choiantoan(p, to_call); // Chơi an toàn
    else choiconao(p, to_call, board); // Chơi thông minh
}
