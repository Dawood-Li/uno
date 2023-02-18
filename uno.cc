#include "./uno.h"

#include <algorithm>

// 根据玩家数量初始化n套卡
// 玩家[2,4] 1套卡
// 玩家[5,8] 2套卡
Uno::Uno(size_t players_max_num) :players_max_num(players_max_num) {
    int loop = players_max_num <= 4 ? 1 : 2;
    cards_waiting.reserve(108 * loop);
    for (int k = 0; k < loop; k++) {   
        for (int i = 0; i < 4; i++) {
            // 此时i对应4种颜色
            cards_waiting.emplace_back(i, 0);       // 0一张
            for (int j = 1; j <= 12; j++) { // 其余卡牌各2张
                cards_waiting.emplace_back(i, j);
                cards_waiting.emplace_back(i, j);
            }
            // 此时i对应黑牌两种类型各4张
            cards_waiting.emplace_back(4, 12); // 黑色+4
            cards_waiting.emplace_back(4, 13); // 黑色变色
        }
    }

    // 打乱
    random_shuffle(cards_waiting.begin(), cards_waiting.end());

    // 每人7张牌
    players.resize(players_max_num);
    for (auto& player : players) {
        for (int i = 0; i < 7; i++) {
            player.cards.push_back(get_card());
        }
    }

    // 随机选择第一个玩家
    current_player = random() % players_max_num;
}

// 抽一张卡
// 若无卡可抽 抛出异常来结束游戏
Card Uno::get_card() {
    
    // 若牌堆无牌 则使用弃牌 并打乱
    if (cards_waiting.size() == 0) {
        
        // 若桌面上也无牌 牌全在玩家们手里 此时游戏结束
        if (cards_showing.size() == 0) {
            throw cards_is_empty{};
        }

        // 将桌面上的牌重新洗牌后插入牌堆重新使用
        cards_waiting.swap(cards_showing);
        random_shuffle(cards_waiting.begin(), cards_waiting.end());
    }

    // 抽牌
    auto card = std::move(cards_waiting.back());
    cards_waiting.pop_back();

    return card;
}

// 检查一个玩家可以出的牌
std::vector<Card> Uno::get_can_push(size_t player_id) {

    std::vector<Card> can_push;

    // 无参考牌 则所有手牌都可出
    if (reference_card.color == 0) {
        can_push = players[player_id].cards;
        return can_push;
    }

    // 根据手牌 检查是否有牌可出
    for (const auto& card : players[player_id].cards) {
        if (card.color == reference_card.color || card.type == reference_card.type) {
            can_push.push_back(card);
        }
    }

    // 若已经有牌可出 返回
    if (can_push.size() != 0) {
        return can_push;
    }
    
    // 开始循环抽牌 直至有牌可出
    while (1) {

        auto card = get_card();
        players[player_id].cards.push_back(card);

        // 判断是否可用
        // 颜色相同可用
        // 是数字牌且数字相等可用
        if(card.color == reference_card.color ||
            reference_card.type >= 0 &&
            reference_card.type <= 10 &&
            card.type == reference_card.type)
        {
            can_push.push_back(card);
            break;
        }
    }
    
    return can_push;
}

// 下一个玩家 考虑顺逆序和边界
size_t Uno::get_next_player() {
    return direct ? 
        (current_player ? current_player - 1 : players_max_num - 1) :
        (current_player+1 != players_max_num ? current_player + 1 : 0);
}

// 游戏循环
void Uno::game_loop() {
    try {
        while (1) {

            // 列出可出的牌
            auto can_push = get_can_push(current_player);

            // 玩家选牌 将交互抽象出去
            int choise = choose_card(current_player, can_push);

            // 找到对应的牌
            auto card = find_if(players[current_player].cards.begin(), players[current_player].cards.end(), [&] (const Card& card) {
                return card.color == can_push[choise].color && card.type == can_push[choise].type;
            });

            // 出牌，修改参考牌
            reference_card = *card;
            cards_showing.push_back(std::move(*card));
            players[current_player].cards.erase(card);

            msg("log", "玩家"+std::to_string(current_player)+"出牌："+cards_showing.back().to_string()+"剩余"+std::to_string(players[current_player].cards.size()));

            if (players[current_player].cards.size() == 0) {
                throw one_player_win{ current_player };
                return;
            } else if (players[current_player].cards.size() == 1) {
                msg("log", "玩家"+std::to_string(current_player)+"UNO");
            }

            // 功能牌判定
            if (cards_showing.back().color == 0) {
                if (cards_showing.back().type == 12) { // +4
                    auto next = get_next_player();
                    players[next].cards.push_back(get_card());
                    players[next].cards.push_back(get_card());
                    players[next].cards.push_back(get_card());
                    players[next].cards.push_back(get_card());
                    msg("log", "玩家"+std::to_string(next)+"罚抽4张");
                } else if (cards_showing.back().type == 13) { // 变色

                    // 将交互抽象出去
                    reference_card.color = choose_color(current_player);
                }

            } else if (cards_showing.back().color >= 1 && cards_showing.back().color <= 4) {

                switch (cards_showing.back().type) {
                case 10: // flip
                    // 排序切换
                    direct = !direct;
                    msg("log", (direct ? "逆序" : "顺序"));
                    break;
                case 11: // pass
                    current_player = get_next_player();
                    msg("log", "玩家"+std::to_string(current_player)+"被pass");
                    break;
                case 12: // +2
                    auto next = get_next_player();
                    players[next].cards.push_back(get_card());
                    players[next].cards.push_back(get_card());
                    msg("log", "玩家"+std::to_string(next)+"罚抽4张");
                    break;
                }
            }

            current_player = get_next_player();
        }
    } catch(one_player_win &e) {
        msg("log", "玩家"+std::to_string(current_player)+"胜利");
    } catch(cards_is_empty &e) {
        msg("log", "卡牌用尽！");
    }
}
