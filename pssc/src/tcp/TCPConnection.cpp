/*
 * TCPConnection.h
 *
 *    Created on: Mar 8, 2021
 *            Author: ubuntu
 */


#include <glog/logging.h>
#include "pssc/transport/tcp/TCPConnection.h"

namespace trs
{

TCPConnection::TCPConnection(std::shared_ptr<tcp::socket> sock,
        std::function<void(std::shared_ptr<TCPConnection>)> funcDisconnected)
    : sock(sock), funcDisconnected(funcDisconnected)
{
    running = true;
}


TCPConnection::~TCPConnection() {}

void TCPConnection::Start()
{
    ReadHeader();

    sendThread = std::thread(
        std::bind(&TCPConnection::Run, this)
    );

    sendThread.detach();
}

void TCPConnection::Stop()
{
    running = false;
    try {
        if (sock->is_open())
        {
            sock->close();
        }
    } catch (...) {
        DLOG(WARNING) << "exception in closing socket.";
    }


//    if (sendThread.joinable())
//    {
//        sendThread.join();
//    }
}

void TCPConnection::ReadHeader()
{
    DLOG(INFO) << "ReadHeader";

    // for keep alive
    auto self = shared_from_this();

    auto header = std::make_shared<TCPMessage::Header>();

    boost::asio::async_read(
        *sock, boost::asio::buffer(header.get(), TCPMessage::SIZE_OF_HEADER),
        std::bind(&TCPConnection::OnHeaderReceived, this, self, header, std::placeholders::_1, std::placeholders::_2));
}


void TCPConnection::ReadBody(std::shared_ptr<TCPMessage::Header> header) {

    DLOG(INFO) << "ReadBody";

    auto self = shared_from_this();

    auto msg = TCPMessage::Generate(header);

    boost::asio::async_read(
        *sock, boost::asio::buffer(msg->body, msg->header.bodyLength),
        std::bind(&TCPConnection::OnBodyReceived, this, self, msg, std::placeholders::_1, std::placeholders::_2));
}

void TCPConnection::OnHeaderReceived(std::shared_ptr<TCPConnection> self,
        std::shared_ptr<TCPMessage::Header> header,
        boost::system::error_code ec, std::size_t receivedLength)
{
//    DLOG(INFO) << "Read header from remote, size: " << receivedLength;

    if (ec != boost::system::errc::success
            || TCPMessage::SIZE_OF_HEADER != receivedLength)
    {
        Stop();
        DLOG(ERROR) << "an exception occurred after receive message.";
        funcDisconnected(shared_from_this());
        return;
    }

    header->decode();

    if (header->bodyLength > 0)
    {
        ReadBody(header);
    }
    else
    {
        // for supporting multi-threads
        ReadHeader();
        funcMessageReceived(TCPMessage::Generate(header));
    }
}

void TCPConnection::OnBodyReceived(std::shared_ptr<TCPConnection> self,
        std::shared_ptr<TCPMessage> msg,
        boost::system::error_code ec, std::size_t receivedLength)
{
//    DLOG(INFO) << "received body length:" << receivedLength;

    if (ec != boost::system::errc::success
            || msg->header.bodyLength != receivedLength)
    {
        Stop();
        DLOG(ERROR) << "an exception occurred after receive message.";
        funcDisconnected(shared_from_this());
        return;
    }

    // for supporting multi-threads
    ReadHeader();
    funcMessageReceived(msg);
}

void TCPConnection::Run()
{
    while (running)
    {
        std::unique_lock<std::mutex> lck(mtxSendQueue);
        cvSendQueue.wait(lck, [this]()
        {
            return !sendQueue.empty();
        });

        auto msg = sendQueue.front();
        sendQueue.pop_front();
        Send(msg);
    }
}

void TCPConnection::PendMessage(std::shared_ptr<TCPMessage> msg)
{
    {
        std::lock_guard<std::mutex> lck(mtxSendQueue);
        sendQueue.emplace_back(msg);
    }

    cvSendQueue.notify_one();
}


bool TCPConnection::Send(std::uint8_t* data, size_t size)
{
    boost::system::error_code ec;
    size_t written = 0;
    size_t restSize = size;
    std::uint8_t* cache = data;

    // send message
    do {
        written = sock->write_some(
            boost::asio::buffer(cache, restSize), ec);
        if (ec)
        {
            break;
        }
        cache += written;
        restSize -= written;
    } while (ec == boost::system::errc::success && running && restSize > 0);

    if (ec != boost::system::errc::success)
    {
        Stop();
        DLOG(ERROR) << "a fatal error occurs while sending data, system exit.";
        funcDisconnected(shared_from_this());
        return false;
    }

    return true;
}

void TCPConnection::Send(std::shared_ptr<TCPMessage> msg)
{
    auto header = msg->header;

    header.encode();

    DLOG(INFO) << "send data size:" << msg->header.bodyLength;

    if(Send((std::uint8_t*)&header, TCPMessage::SIZE_OF_HEADER)
        && msg->header.bodyLength > 0)
    {
        Send(msg->body, msg->header.bodyLength);
    }
}

}


