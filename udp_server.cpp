#include <QUdpSocket>
#include <memory>
#include "udp_server.h"

UDP_Server::UDP_Server()
{
    Socket = std::make_unique<QUdpSocket>();
    Socket->bind(QHostAddress::LocalHost, 1234);
}
