#include "./uno.h"

#include <iostream>
using namespace std;

class Uno_Game : public Uno {

public:

    Uno_Game(size_t player_max_num = 8) : Uno(player_max_num) {
        srand(time(NULL));
    }

    // 重载 玩家出牌
    size_t choose_card(size_t player, const std::vector<Card>& player_cards) override {
        return random() % player_cards.size(); // [0, n-1]
    }

    // 重载 玩家选色
    size_t choose_color(size_t player) override {
        return random() % 4 + 1; // [1,4]
    }

    // 重载 消息分发
    // 此接口当前用于概况所有事件
    // 后续会被扩展
    // 各个事件将会有单独的接口
    void msg(size_t player_id, const std::string& text) override {
        std::cout << "to " << player_id << ": " << text << "\n";
    }
};

int main() {
    Uno_Game(8).game_loop();
}
