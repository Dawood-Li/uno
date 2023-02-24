#include "room.h"

#include <QJsonDocument>
#include <QJsonObject>

Room::Room(quint16 port, size_t max_player_num, QObject *parent)
    : m_server(new QTcpServer())
    , m_clients(QVector<QTcpSocket*>(max_player_num, nullptr))
    , Uno(max_player_num)
    , QObject(parent)
{
    // 监听端口
    m_server->listen(QHostAddress::Any, port);

    // 处理新连接
    connect(m_server, &QTcpServer::newConnection, this, &Room::onNewConnections);
}

// 处理新连接
void Room::onNewConnections() {
    while(m_server->hasPendingConnections()) {
        auto *client = m_server->nextPendingConnection();

        // 若连接已满 则断开连接
        if (m_clients.count(nullptr) == 0) {
            auto json = QJsonObject({
                { "code", -1 },
                { "msg", "the room is full" },
                { "data", QJsonObject() },
            });
            client->write(QJsonDocument(json).toJson());
            client->disconnectFromHost();
            continue;
        }

        // 找到一个空位 保存连接
        size_t player_id = m_clients.indexOf(nullptr);
        m_clients[player_id] = client;

        // 连接断开事件
        // 这里 需要判断游戏是否已开始
        // 如果游戏一开始 抛出一个异常来终止游戏
        QObject::connect(client, &QTcpSocket::disconnected, [=] () {
            m_clients[player_id] = nullptr;
            // m_names[player_id] = "player_" + QString::number(player_id+1);
            client->deleteLater();
        });

        // 检查是不是所有的玩家都到齐了
        bool is_ready = true;
        for (int i = 0; i < players_max_num; i++) {
            if (m_clients[i] == nullptr || m_names[i].size() == 0) {
                is_ready = false;
                break;
            }
        }
        if (is_ready) {
            emit ready();
        }
    }
}

// 玩家出牌接口 需要重载
size_t Room::choose_card(size_t player_id, const std::vector<Card>& player_cards) {
    return rand() % player_cards.size(); // [0, n-1]
}

// 玩家选色接口 需要重载
size_t Room::choose_color(size_t player_id) {
    return rand() % 4 + 1; // [1,4]
}

// 消息接口 需要重载
void Room::msg(size_t player_id, const std::string& text) {
    
    auto json = QJsonObject({
        { "code", 0 },
        { "msg", QString::fromStdString(text) },
        { "data", QJsonObject() },
    });
    auto send_buf = QJsonDocument(json).toJson();

    // 群发    
    if (player_id == players_max_num) {
        for (player_id = 0; player_id < players_max_num; player_id++) {
            if (m_clients[player_id] != nullptr) {
                m_clients[player_id]->write(send_buf);
            } 
        }
    }

    // 单发
    else if (m_clients[player_id] != nullptr) {
        m_clients[player_id]->write(send_buf);
    }
}
