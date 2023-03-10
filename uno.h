#include "./card.h"

#include <string>
#include <vector>

class Uno {

public:

    Uno(size_t players_max_num = 8);

    // 游戏循环 用于开始游戏
    void game_loop();

    // 接口 玩家出牌 需要重载
    virtual size_t choose_card(size_t player, const std::vector<Card>& player_cards) = 0;
    
    // 接口 玩家选色 需要重载
    virtual size_t choose_color(size_t player) = 0;

    // 接口 消息分发 需要重载
    virtual void msg(size_t player_id, const std::string& text) = 0;
    // 此接口当前用于概况所有事件
    // 后续会被扩展
    // 各个事件将会有单独的接口

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
        std::vector<Card> cards; // 玩家手牌
        // std::string name;        // 玩家名
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
