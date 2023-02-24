#ifndef ROOM_H
#define ROOM_H

#include "./uno.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <string>
#include <vector>

class Room : public QObject, public Uno {

    Q_OBJECT

public:

    explicit Room(quint16 port, size_t max_player_num, QObject *parent = nullptr);

    // 玩家出牌接口 需要重载
    size_t choose_card(size_t player_id, const std::vector<Card>& player_cards) override;

    // 玩家选色接口 需要重载
    size_t choose_color(size_t player_id) override;

    // 消息接口 需要重载
    void msg(size_t player_id, const std::string& text) override;

    bool is_ready();
    void play();

signals:
    void ready();

private slots:

    void onNewConnections();

private:

    QTcpServer *m_server;
    QVector<QTcpSocket *> m_clients;
};

#endif // ROOM_H
