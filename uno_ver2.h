#include "./card.h"

#include <string>
#include <vector>

class Uno {

public:

    Uno(size_t players_max_num = 8);

    // 游戏循环
    void game_loop();

    // 玩家出牌接口 需要重载
    virtual size_t choose_card(size_t player, const std::vector<Card>& player_cards) = 0;
    
    // 玩家选色接口 需要重载
    virtual size_t choose_color(size_t player) = 0;

    // 消息接口 需要重载
    virtual void msg(const std::string& to, const std::string& text) = 0;

protected:

    // 在卡池中抽出一张卡
    Card get_card();

    // 显示某玩家可出的牌
    std::vector<Card> get_can_push(size_t player_id);

    // 获取下一个玩家的编号
    size_t get_next_player();

protected:

    size_t players_max_num = 0; // 玩家数量

    std::vector<Card> cards_waiting; // 备用卡牌
    std::vector<Card> cards_showing; // 已打出的牌

    struct Player {
        std::string name;        // 玩家名
        std::vector<Card> cards; // 玩家手牌
    };

    std::vector<Player> players; // 玩家

    // 参考牌
    Card reference_card = { 0, 0 };
    // 若当前无参考牌 则color == black

    // 顺逆位
    bool direct = false;

    // 当前玩家
    size_t current_player = 0;

    // 使用异常结束对局
    struct one_player_win { size_t who; }; // 玩家胜利
    struct one_player_quit { size_t who; }; // 玩家退出
    struct cards_is_empty {};              // 卡已用尽
};
