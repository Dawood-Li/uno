/*
    uno

    ver 1
    完成了uno牌机制的本地模拟

    ver 1.1
    目标：加入一个延迟，用于响应玩家操作。

    ver2 
    目标：将其封装成一个uno game对象
*/

#include "./card.h"

#include <iostream>
#include <algorithm>

using namespace std;

#define endl "\n"

// 玩家数量
int player_num = 4;

// 待抽牌堆
std::vector<Card> cards;

// 弃牌堆
std::vector<Card> cards2;

// 玩家手牌
vector<vector<Card>> players;

// 参考牌
Card reference_card = { 0, 0 };
// 若当前无参考牌 则color == black

// 顺逆位
bool direct = false;

// 抽卡
Card get_card() {
    // 若牌堆无牌 则使用弃牌 并打乱
    if (cards.size() == 0) {
        cards.swap(cards2);
        random_shuffle(cards.begin(), cards.end());
    }

    // 抽牌
    auto card = move(cards.back());
    cards.pop_back();

    return card;
}

// 初始化
void init(int player_num) {
    
    cards.clear();
    cards2.clear();

    // 4人以下只用一副牌即可 4人以上则需要2副牌
    int loop = player_num <= 4 ? 1 : 2;
    for (int k = 0; k < loop; k++) {
    
        for (int i = 0; i < 4; i++) {
            
            // 此时i对应4种颜色
            cards.emplace_back(i, 0);       // 0一张
            for (int j = 1; j <= 12; j++) { // 其余卡牌各2张
                cards.emplace_back(i, j);
                cards.emplace_back(i, j);
            }
            // 此时i对应黑牌两种类型各4张
            cards.emplace_back(4, 12); // 黑色+4
            cards.emplace_back(4, 13); // 黑色变色
        }
    }

    // 打乱
    random_shuffle(cards.begin(), cards.end());

    // 每人7张牌
    players.resize(player_num);
    for (auto& player : players) {
        for (int i = 0; i < 7; i++) {
            player.push_back(get_card());
        }
    }
}

// 抽牌
vector<Card> get_can_push(vector<Card>& player) {

    vector<Card> can_push;

    // 无参考牌 则所有手牌都可出
    if (reference_card.color == 0) {
        can_push = player;
        return can_push;
    }

    // 根据手牌 检查是否有牌可出
    for (const auto& card : player) {
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
        player.push_back(card);

        // cout << "抽牌: " << card.to_string() << endl;

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

int main() {

    srand(time(NULL));

    init(4);

    cout << "size: " << cards.size() << endl;

    // 获取下家下标
    // 考虑顺逆序
    // 考虑越界
    auto get_next = [] (int cur) -> int {
        return direct ? 
            (cur ? cur - 1 : player_num - 1) :
            (cur+1 != player_num ? cur + 1 : 0);
    };

    for (int cur = 0;;) {

        cout << "玩家" << cur << endl;

        // cout << "参考牌:" << reference_card.to_string() << endl;

        auto& player = players[cur];

        // cout << "手牌:" << endl;
        for (auto& card : player) {
            // cout << card.to_string() << endl;
        }

        // 抽牌
        auto can_push = get_can_push(player);

        // 选择
        int choise = random() % can_push.size();

        // 找到对应的牌
        auto card = find_if(player.begin(), player.end(), [&] (const Card& card) {
            return card.color == can_push[choise].color && card.type == can_push[choise].type;
        });

        // 在加入交互后 这里应该有验证逻辑

        // 出牌
        // 修改参考牌
        reference_card = *card;
        cards2.push_back(move(*card));
        player.erase(card);

        cout << "玩家" << cur << "出牌:" << cards2.back().to_string() << endl;

        if (player.size() == 0) {
            cout << "玩家" << cur << "胜利" << endl;
            return 0;
        } else if (player.size() == 1) {
            cout << "玩家" << cur << "UNO" << endl;
        }

        // 功能牌判定
        if (cards2.back().color == 0) {
            if (cards2.back().type == 12) { // +4
                auto next = get_next(cur);
                players[next].push_back(get_card());
                players[next].push_back(get_card());
                players[next].push_back(get_card());
                players[next].push_back(get_card());
                cout << "玩家" << next << "+4" << endl;
            } else if (cards2.back().type == 13) { // 变色
                
                // 此处增加交互逻辑

            }

        } else if (cards2.back().color >= 1 && cards2.back().color <= 4) {

            switch (cards2.back().type) {
            case 10: // flip
                // 排序切换
                direct = !direct;
                cout << (direct ? "逆序" : "顺序") << endl;
                break;
            case 11: // pass
                cur = get_next(cur);
                cout << "玩家" << cur << "被pass" << endl;
                break;
            case 12: // +2
                auto next = get_next(cur);
                players[next].push_back(get_card());
                players[next].push_back(get_card());
                cout << "玩家" << next << "+2" << endl;
                break;
            }
        }

        cout << endl;

        cur = get_next(cur);
    }
}
