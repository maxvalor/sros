/*
 * tcp_connection.h
 *
 *    Created on: Mar 8, 2021
 *            Author: ubuntu
 */

#ifndef TCP_CONNECTION_H_
#define TCP_CONNECTION_H_

#include <boost/asio.hpp>
#include <condition_variable>
#include <functional>
#include <list>
#include <thread>

#include "TCPMessage.h"

namespace trs
{

using boost::asio::ip::tcp;

class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
    TCPConnection() = default;
public:
    TCPConnection(const TCPConnection&) = default;
    TCPConnection(std::shared_ptr<tcp::socket> sock,
            std::function<void(std::shared_ptr<TCPConnection>)> funcDisconnected);

    virtual ~TCPConnection();

    inline void SetOnMessage(std::function<void(std::shared_ptr<TCPMessage>)> funcMessageReceived)
    {
        this->funcMessageReceived = funcMessageReceived;
    }
    void PendMessage(std::shared_ptr<TCPMessage> msg);

    void Start();
    void Stop();

    inline bool IsRunning() { return running; }

private:
    std::function<void(std::shared_ptr<TCPConnection>)> funcDisconnected;
    std::function<void(std::shared_ptr<TCPMessage>)> funcMessageReceived;

    std::shared_ptr<tcp::socket> sock;
    std::atomic_bool running;

    std::mutex mtxSendQueue;
    std::condition_variable cvSendQueue;
    std::list<std::shared_ptr<TCPMessage>> sendQueue;
    std::thread sendThread;

    void OnHeaderReceived(std::shared_ptr<TCPConnection> self, std::shared_ptr<TCPMessage::Header> header,
            boost::system::error_code ec, std::size_t receivedLength);
    void OnBodyReceived(std::shared_ptr<TCPConnection> self, std::shared_ptr<TCPMessage> msg,
            boost::system::error_code ec, std::size_t receivedLength);

    void ReadHeader();
    void ReadBody(std::shared_ptr<TCPMessage::Header> header);

    void Run();

    void Send(std::shared_ptr<TCPMessage>);
    bool Send(std::uint8_t* data, size_t size);
};

}

#endif /* TCP_CONNECTION_H_ */
