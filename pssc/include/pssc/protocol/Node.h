/*
 * Node.h
 *
 *  Created on: Apr 21, 2021
 *      Author: ubuntu
 */

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <thread>
#include <functional>
#include <glog/logging.h>
#include "pssc/transport/tcp/TCPClient.h"
#include "pssc/util/IDGenerator.h"
#include "Instruction.h"
#include "types.h"
#include "pssc/util/Notifier.h"
#include "pssc/protocol/msgs/pssc_msgs.h"

namespace pssc {

using trs::TCPMessage;
using trs::TCPClient;
using trs::TCPConnection;

class Node
{

public:
    class ResponseOperator
    {
        pssc_id callerId;
        pssc_id messageId;
        std::shared_ptr<TCPConnection> conn;

        friend class Node;
    public:
        void SendResponse(bool success,
                std::uint8_t* data, size_t size);
    };

    class ResponseData
    {
        std::shared_ptr<ServiceResponseMessage> msg;
    public:
        ResponseData(bool success) : success(success) {}
        ResponseData(std::shared_ptr<ServiceResponseMessage> msg) : msg(msg)
        {
            success = msg->success;
            data = msg->data;
            sizeOfData = msg->sizeOfData;
        }
        bool success;
        size_t sizeOfData;
        pssc_bytes data;
    };

private:
    std::shared_ptr<TCPClient> client;
    std::shared_ptr<TCPConnection> conn;
    std::uint64_t nodeId;
    IDGenerator<std::uint64_t> messageIdGen;
    bool running;
    std::unordered_map<pssc_id, std::function<void()>> mapAckNoti;
    std::thread execPub, execCall;

    util::Notifier startNoti;

    std::mutex mtxMsg;
    std::condition_variable cvMsg;
    std::mutex mtxCall;
    std::condition_variable cvCall;
    std::list<std::shared_ptr<TCPMessage>> msgList;
    std::list<std::shared_ptr<TCPMessage>> callList;

    std::mutex mtxAcks;
    std::unordered_map<std::uint64_t, std::shared_ptr<TCPMessage>> acks;


    std::function<void(std::string, std::uint8_t*, size_t)> topicCallback;
    std::function<void(std::string, std::uint8_t*, size_t, std::shared_ptr<ResponseOperator>)> srvCallback;

private:
    void ExecPublish();
    void ExecCall();
    bool SendRequestAndWaitForResponse(pssc_id messageId, std::shared_ptr<TCPMessage> req, std::shared_ptr<TCPMessage>& resp);

    void OnConntected(std::shared_ptr<TCPConnection> conn);
    void OnDisconntected(std::shared_ptr<TCPConnection> conn);

    void OnMessageReceived(std::shared_ptr<TCPMessage> msg);
    void DispatchMessage(std::shared_ptr<TCPMessage> msg);


    void OnGenerelResponse(std::shared_ptr<TCPMessage> msg);
    void OnRegACK(std::shared_ptr<TCPMessage> msg);

    void OnPublish(std::shared_ptr<TCPMessage> msg);
    void OnSrvCall(std::shared_ptr<TCPMessage> msg);

public:

    Node()
    {
        topicCallback = [](std::string, std::uint8_t*, size_t){};
        srvCallback = [](std::string, std::uint8_t*, size_t, std::shared_ptr<ResponseOperator>){};
    }

    bool Initialize(int port);

    pssc_size QuerySubNum(std::string topic);
    void Publish(std::string topic, std::uint8_t* data, size_t size, bool feedback = false);
    bool Subscribe(std::string topic);
    bool UnSubscribe(std::string topic);
    bool AdvertiseService(std::string srv_name);
    bool CloseService(std::string srv_name);
    std::shared_ptr<Node::ResponseData> RemoteCall(std::string srv_name, std::uint8_t* data, size_t size);




    // on message received
    void SetTopicCallback(std::function<void(std::string, std::uint8_t*, size_t)> topicCallback)
    {
        this->topicCallback = topicCallback;
    }

    // on service call received
    void SetServiceCallback(std::function<void(std::string, std::uint8_t*, size_t, std::shared_ptr<ResponseOperator>)> srvCallback)
    {
        this->srvCallback = srvCallback;
    }
};

};
