/*
 * Node.cpp
 *
 *  Created on: Apr 25, 2021
 *      Author: ubuntu
 */

#include "pssc/protocol/Node.h"
#include "pssc/protocol/types.h"

namespace pssc {

bool Node::Initialize(int port)
{
    running = true;

    execPub = std::thread(
        std::bind(&Node::ExecPublish, this)
    );
    execPub.detach();

    execCall = std::thread(
        std::bind(&Node::ExecCall, this)
    );
    execCall.detach();

    client = std::make_shared<TCPClient>(
        port,
        std::bind(&Node::OnConntected, this, std::placeholders::_1),
        std::bind(&Node::OnDisconntected, this, std::placeholders::_1)
    );

    client->Connect();

    return startNoti.wait_for(std::chrono::milliseconds(300)) == std::cv_status::no_timeout;
}

void Node::OnConntected(std::shared_ptr<TCPConnection> conn)
{
    this->conn = conn;
    DLOG(INFO) << "connected.";

    conn->SetOnMessage(std::bind(&Node::DispatchMessage, this, std::placeholders::_1));

    RegisterMessage req;
    req.messageId = messageIdGen.Next();
    conn->PendMessage(req.toTCPMessage());
}

void Node::OnMessageReceived(std::shared_ptr<TCPMessage> msg)
{
    mtxMsg.lock();
    msgList.push_back(msg);
    mtxMsg.unlock();
    cvMsg.notify_one();
}

void Node::DispatchMessage(std::shared_ptr<TCPMessage> msg)
{
    pssc_ins ins;
    msg->NextData(ins);
    switch(ins)
    {
        case Ins::REGACK:
        {
            OnRegACK(msg);
            break;
        }

        case Ins::QUERY_SUBSCRIBER_NUMBER_ACK:
        {
            OnGenerelResponse(msg);
            break;
        }

        case Ins::PUBLISH:
        {
            OnPublish(msg);
            break;
        }

        case Ins::SUBACK:
        {
            OnGenerelResponse(msg);
            break;
        }

        case Ins::UNSUBACK:
        {
            OnGenerelResponse(msg);
            break;
        }

        case Ins::ADVSRVACK:
        {
            OnGenerelResponse(msg);
            break;
        }

        case Ins::CLOSESRVACK:
        {
            OnGenerelResponse(msg);
            break;
        }

        case Ins::SERVICE_CALL:
        {
            OnSrvCall(msg);
            break;
        }

        case Ins::SERVICE_RESPONSE:
        {
            OnGenerelResponse(msg);
            break;
        }

        default:
        {
            DLOG(ERROR) << "UNKOWN MESSAGE";
            break;
        }
    }
}

void Node::ExecPublish()
{
    while (running)
    {
        std::unique_lock<std::mutex> lck(mtxMsg);
        if (msgList.empty())
        {
            cvMsg.wait(lck);
        }

        auto msg = *msgList.begin();
        msgList.pop_front();

        PublishMessage req(msg);
        topicCallback(req.topic, req.data, req.sizeOfData);
    }
}

void Node::ExecCall()
{
    while (running)
    {
        std::unique_lock<std::mutex> lck(mtxCall);
        if (callList.empty())
        {
            cvCall.wait(lck);
        }

        auto msg = *callList.begin();
        callList.pop_front();

        ServiceCallMessage req(msg);
        auto op = std::make_shared<ResponseOperator>();
        op->messageId = req.messageId;
        op->callerId = req.callerId;
        op->conn = conn;
        srvCallback(req.srv_name, req.data, req.sizeOfData, op);
    }
}

void Node::OnGenerelResponse(std::shared_ptr<TCPMessage> msg)
{
    LOG(INFO) << "Received Response.";
    pssc_id messageId;
    msg->NextData(messageId);
    msg->Reset();
    msg->IgnoreBytes(SIZE_OF_PSSC_INS);

    mtxAcks.lock();
//    acks.insert(std::pair<pssc_id, std::shared_ptr<TCPMessage>>(messageId, msg));
    acks.insert(std::make_pair(messageId, msg));
    auto funcNoti = mapAckNoti.at(messageId);
    mapAckNoti.erase(messageId);
    mtxAcks.unlock();

    funcNoti();
}

void Node::OnRegACK(std::shared_ptr<TCPMessage> msg)
{
    RegACKMessage ack(msg);
    if (ack.success)
    {
        LOG(INFO) << "Success to register node with id " << ack.nodeId;
        nodeId = ack.nodeId;
        startNoti.notify_one();
    }
    else
    {
        LOG(WARNING) << "Failed to register node.";
    }
}

void Node::OnSrvCall(std::shared_ptr<TCPMessage> msg)
{
    mtxCall.lock();
    callList.push_back(msg);
    mtxCall.unlock();
    cvCall.notify_one();

    LOG(INFO) << "Received Service Call.";
}

void Node::OnPublish(std::shared_ptr<TCPMessage> msg)
{
    mtxMsg.lock();
    msgList.push_back(msg);
    mtxMsg.unlock();
    cvMsg.notify_one();

    LOG(INFO) << "Received Publish.";
}

void Node::OnDisconntected(std::shared_ptr<TCPConnection> conn)
{
    running = false;
    DLOG(INFO) << "disconnected.";
}

bool Node::SendRequestAndWaitForResponse(pssc_id messageId, std::shared_ptr<TCPMessage> req, std::shared_ptr<TCPMessage>& resp)
{
    auto noti = std::make_shared<util::Notifier>();
    auto f = std::bind([](std::shared_ptr<util::Notifier> noti)
    {
        noti->notify_one();
    }, noti);

    mtxAcks.lock();
//    mapAckNoti.insert(std::pair<pssc_id, std::function<void()>>(messageId, f));
    mapAckNoti.insert(std::make_pair(messageId, f));
    mtxAcks.unlock();
    conn->PendMessage(req);

//    auto rlt = noti->wait_for(std::chrono::milliseconds(3000));
//    if (rlt == std::cv_status::timeout)
//    {
//        return false;
//    }
    noti->wait();
    std::lock_guard<std::mutex> lck(mtxAcks);
    resp = acks.at(messageId);
    return true;
}

pssc_size Node::QuerySubNum(std::string topic)
{
    QuerySubNumMessage query;
    query.messageId = messageIdGen.Next();
    query.inquirerId = nodeId;
    query.topic = topic;

    std::shared_ptr<TCPMessage> msg;
    if(!SendRequestAndWaitForResponse(query.messageId, query.toTCPMessage(), msg))
    {
        return 0;
    }

    QuerySubNumACKMessage resp(msg);
    return resp.subNum;
}

void Node::Publish(std::string topic, std::uint8_t*data, size_t size, bool feedback)
{
    PublishMessage req;
    req.messageId = messageIdGen.Next();
    req.publisherId = nodeId;
    req.topic = topic;
    req.sizeOfData = size;
    req.data = data;
    req.feedback = feedback;

    conn->PendMessage(req.toTCPMessage());
}


bool Node::Subscribe(std::string topic)
{
    SubscribeMessage req;
    req.messageId = messageIdGen.Next();
    req.subscriberId = nodeId;
    req.topic = topic;

    std::shared_ptr<TCPMessage> msg;
    if(!SendRequestAndWaitForResponse(req.messageId, req.toTCPMessage(), msg))
    {
        return false;
    }

    SubACKMessage resp(msg);
    return resp.success;
}

bool Node::UnSubscribe(std::string topic)
{
    UnSubscribeMessage req;
    req.messageId = messageIdGen.Next();
    req.subscriberId = nodeId;
    req.topic = topic;

    std::shared_ptr<TCPMessage> msg;
    if(!SendRequestAndWaitForResponse(req.messageId, req.toTCPMessage(), msg))
    {
        return false;
    }

    UnSubACKMessage resp(msg);
    return resp.success;
//    conn->PendMessage(req.toTCPMessage());
    return true;
}

bool Node::AdvertiseService(std::string srv_name)
{
    AdvertiseServiceMessage req;
    req.messageId = messageIdGen.Next();
    req.advertiserId = nodeId;
    req.srv_name = srv_name;

    std::shared_ptr<TCPMessage> msg;
    if(!SendRequestAndWaitForResponse(req.messageId, req.toTCPMessage(), msg))
    {
        return false;
    }

    AdvSrvACKMessage resp(msg);
    return resp.success;
}

bool Node::CloseService(std::string srv_name)
{
    CloseServiceMessage req;
    req.messageId = messageIdGen.Next();
    req.advertiserId = nodeId;
    req.srv_name = srv_name;

    std::shared_ptr<TCPMessage> msg;
    if(!SendRequestAndWaitForResponse(req.messageId, req.toTCPMessage(), msg))
    {
        return false;
    }

    CloseSrvACKMessage resp(msg);
    return resp.success;
}

std::shared_ptr<Node::ResponseData> Node::RemoteCall(std::string srv_name, std::uint8_t* data, size_t size)
{
    ServiceCallMessage req;
    req.messageId = messageIdGen.Next();
    req.callerId = nodeId;
    req.srv_name = srv_name;
    req.sizeOfData = size;
    req.data = data;

    std::shared_ptr<TCPMessage> msg;
    if(!SendRequestAndWaitForResponse(req.messageId, req.toTCPMessage(), msg))
    {
        return std::make_shared<Node::ResponseData>(false);
    }

    auto resp = std::make_shared<ServiceResponseMessage>(msg);
    return std::make_shared<Node::ResponseData>(resp);
}

void Node::ResponseOperator::SendResponse(bool success,
        std::uint8_t* data, size_t size)
{
    ServiceResponseMessage resp;
    resp.success = success;
    resp.messageId = messageId;
    resp.callerId = callerId;
    resp.sizeOfData = size;
    resp.data = data;
    conn->PendMessage(resp.toTCPMessage());
}

}
