#include "./uno.h"

#include <iostream>
using namespace std;

class Uno_Game : public Uno {

public:

    Uno_Game(size_t player_max_num = 8) : Uno(player_max_num) {
        srand(time(NULL));
    }

    // 玩家出牌接口 需要重载
    size_t choose_card(size_t player, const std::vector<Card>& player_cards) override {
        return random() % player_cards.size(); // [0, n-1]
    }

    // 玩家选色接口 需要重载
    size_t choose_color(size_t player) override {
        return random() % 4 + 1; // [1,4]
    }

    // 消息接口 需要重载
    void msg(const std::string& to, const std::string& text) override {
        // std::cout << "to " << to << ": " << text << "\n";

        if (to == "log") {
            std::cout << text << "\n";
        }

        else if (to == "all") {
            // send to all player
        }
        else if (to == "player_1") {
            // send to player_1
        }
    }

};

int main() {
    Uno_Game(8).game_loop();
}
