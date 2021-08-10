/*
 * tcp_client.h
 *
 *  Created on: Mar 9, 2021
 *      Author: ubuntu
 */

#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#include <boost/asio.hpp>
#include <thread>
#include "TCPConnection.h"

namespace trs
{

class TCPClient
{
public:
    TCPClient(
      int port,
      std::function<void(std::shared_ptr<TCPConnection>)> on_connected,
      std::function<void(std::shared_ptr<TCPConnection>)> on_disconnected
    );

    void Connect();
    inline void Disconnect() { ioContext.stop(); }

private:
    boost::asio::io_service ioContext;
    std::shared_ptr<tcp::socket> sock;
    tcp::endpoint ep;
    std::thread contextThread;

    std::function<void(std::shared_ptr<TCPConnection>)> OnConnected;
    std::function<void(std::shared_ptr<TCPConnection>)> OnDisconnected;

    void Run();
};

}


#endif /* TCP_CLIENT_H_ */
