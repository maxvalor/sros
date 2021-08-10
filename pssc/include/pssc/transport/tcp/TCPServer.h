/*
 * service.h
 *
 *  Created on: Mar 8, 2021
 *      Author: ubuntu
 */

#ifndef TCP_SERVICE_H_
#define TCP_SERVICE_H_

#include <functional>
#include <memory>
#include <boost/asio.hpp>
#include "TCPConnection.h"

namespace trs
{

using boost::asio::ip::tcp;

class TCPServer
{
private:
    static const std::uint64_t DEFAULT_MAX_CONNECTIONS = 100;
public:
    TCPServer(
          int port,
          std::function<void(std::shared_ptr<TCPConnection>)> funcConnected,
          std::function<void(std::shared_ptr<TCPConnection>)> funcDisconnected,
          size_t maxConnection = DEFAULT_MAX_CONNECTIONS
          );

    inline void Start() { Run(); }
    inline void Stop() { ioContext.stop(); }

private:
    void Accept();

private:
    size_t currentConnections;
    size_t maxConnection;
    boost::asio::io_service ioContext;
    std::shared_ptr<tcp::acceptor> acceptor;
    tcp::endpoint ep;

    std::function<void(std::shared_ptr<TCPConnection>)> OnConnected;
    std::function<void(std::shared_ptr<TCPConnection>)> OnDisconnected;

    void Run();
};

}

#endif /* TCP_SERVICE_H_ */
