#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <memory>

#define DEST_PORT 1234
#define DEST_IP "127.0.0.1"

class UDP_Server
{
public:
    UDP_Server();

private:
    std::unique_ptr<QUdpSocket> Socket;
};

#endif // UDP_SERVER_H
