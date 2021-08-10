/*
 * core.cpp
 *
 *  Created on: Mar 9, 2021
 *      Author: ubuntu
 */

#include "pssc/protocol/Core.h"
#include "pssc/protocol/Instruction.h"
#include <glog/logging.h>
#include <thread>
#include <string>
#include <future>
#include "pssc/protocol/msgs/pssc_msgs.h"

namespace pssc
{

Core::Core(int port)
{
    server = std::make_unique<TCPServer>(
            port,
            std::bind(&Core::OnConnected, this, std::placeholders::_1),
            std::bind(&Core::OnDisconnected, this, std::placeholders::_1)
    );
}

void Core::OnConnected(std::shared_ptr<TCPConnection> conn)
{
    DLOG(INFO) << "client connected.";

    conn->SetOnMessage(
        std::bind(&Core::DispatchMessage, this, conn, std::placeholders::_1)
    );

    conn->Start();
}

void Core::OnDisconnected(std::shared_ptr<TCPConnection> conn)
{
    // remove name->connection
    for (auto & node : nodes)
    {
        if (node.second.get() == conn.get())
        {
            DLOG(INFO) << "node with id " << node.first << " was disconnected.";
            auto nodeId = node.first;
            nodes.erase(nodeId);

            // close service
            pssc_write_guard guard(rwlckSrvs);
            std::list<std::string> srv_names;
            for(auto srv : srvs)
            {
                if (srv.second == nodeId)
                {
                    srv_names.push_back(srv.first);
                }
            }

            for(auto srv_name : srv_names)
            {
                srvs.erase(srv_name);
            }

            return;
        }
    }
}

void Core::DispatchMessage(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg)
{
    if (conn->IsRunning())
    {
        pssc_ins ins;
        msg->NextData(ins);

        switch (ins)
        {
            case Ins::REGISTER:
            {
                Register(conn, msg);
                break;
            }

            case Ins::QUERY_SUBSCRIBER_NUMBER:
            {
                QuerySubNum(conn, msg);
                break;
            }

            case Ins::PUBLISH:
            {
                Publish(conn, msg);
                break;
            }

            case Ins::SUBSCRIBE:
            {
                Subscribe(conn, msg);
                break;
            }

            case Ins::UNSUBSCRIBE:
            {
                UnSubscribe(conn, msg);
                break;
            }

            case Ins::ADDVERTISE_SERVICE:
            {
                AdvertiseService(conn, msg);
                break;
            }

            case Ins::SERVICE_CALL:
            {
                CallService(conn, msg);
                break;
            }

            case Ins::SERVICE_RESPONSE:
            {
                ResponseService(conn, msg);
                break;
            }

            case Ins::CLOSE_SERVICE:
            {
                CloseService(conn, msg);
                break;
            }

            default:
            {
                DLOG(ERROR) << "UNKOWN MESSAGE";
                break;
            }
        }
    }
}

void Core::Register(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg)
{
    DLOG(INFO) << "Register Received.";
    RegisterMessage req(msg);
    RegACKMessage ack;
    ack.messageId = req.messageId;

    pssc_write_guard guard(rwlckNodes);
    ack.nodeId = nodeIdGen.Next();
    if (nodes.find(ack.nodeId) != nodes.end())
    {
        ack.success = false;
    }
    else
    {
        nodes.insert(std::make_pair(ack.nodeId, conn));
        ack.success = true;
    }

    conn->PendMessage(ack.toTCPMessage());
    DLOG(INFO) << "Register Responsed with success:" << ack.success;
}

void Core::QuerySubNum(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg)
{
    QuerySubNumMessage req(msg);
    DLOG(WARNING) << "QUERY_SUBSCRIBER_NUMBER: inquirerId:" << req.inquirerId;

    pssc_read_guard guardTopics(rwlckTopics);
    auto&& subscribers = topics.find(req.topic);

    QuerySubNumACKMessage resp;
    resp.messageId = req.messageId;
    resp.subNum = ((subscribers == topics.end()) ? (0) : (subscribers->second.size()));

    conn->PendMessage(resp.toTCPMessage());
    DLOG(INFO) << "QUERY_SUBSCRIBER_NUMBER Responsed with subNum:" << resp.subNum;
}

void Core::Publish(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg)
{
    PublishMessage req(msg);
    DLOG(WARNING) << "PUBLISH: publisher id:" << req.publisherId;

    pssc_read_guard guardTopics(rwlckTopics);
    auto&& subscribers = topics.find(req.topic);
    DLOG(WARNING) << "publish data size: " << req.sizeOfData;

    if (subscribers == topics.end())
    {
        return;
    }

    if (subscribers->second.size() > 1)
    {
        std::vector<std::future<bool>> fs;
        for (auto& subscriberId : subscribers->second)
        {
            if (subscriberId == req.publisherId && !req.feedback)
            {
                continue;
            }
            DLOG(WARNING) << "publish topic: " + req.topic + " to node with id: " << subscriberId;
            pssc_read_guard guardNodes(rwlckNodes);
            try {
                auto& subConn = nodes.at(subscriberId);

                auto f = std::async([subConn, msg](){
                    subConn->PendMessage(msg);
                    return true;
                });
                fs.emplace_back(std::move(f));
            } catch (...) {
                // disconnected subscriber, do nothing
                std::this_thread::yield();
            }
        }

        for (auto& f :  fs)
        {
            f.get();
        }
    }
    else
    {
        for (auto& subscriberId : subscribers->second)
        {
            if (subscriberId == req.publisherId && !req.feedback)
            {
                continue;
            }
            DLOG(WARNING) << "publish topic: " + req.topic + " to node with id: " << subscriberId;
            pssc_read_guard guardNodes(rwlckNodes);
            try {
                auto& subConn = nodes.at(subscriberId);

                subConn->PendMessage(msg);

            } catch (...) {
                // disconnected subscriber, do nothing
                std::this_thread::yield();
            }
        }
    }
}

void Core::Subscribe(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg)
{
    SubscribeMessage req(msg);
    SubACKMessage resp;

    DLOG(INFO) << "SUBSCRIBE: " << req.subscriberId << "," << req.topic;

    pssc_write_guard guard(rwlckTopics);
    if (topics.find(req.topic) == topics.end())
    {
        std::list<pssc_id> subscribers;
        subscribers.emplace_back(req.subscriberId);
        topics.insert(std::make_pair(req.topic, subscribers));

        resp.success = true;
    }
    else
    {
        try {
            auto& subscribers = topics.at(req.topic);
            if (std::find(subscribers.begin(), subscribers.end(), req.subscriberId) == subscribers.end())
            {
                subscribers.push_back(req.subscriberId);
                DLOG(INFO) << "SUBSCRIBE: OK, count of subscriber:" << subscribers.size();
                resp.success = true;
            }
        }
        catch (...)
        {
            DLOG(ERROR) << "SUBSCRIBE: ???";
            resp.success = false;
        }
    }

    resp.messageId = req.messageId;
    conn->PendMessage(resp.toTCPMessage());
    DLOG(INFO) << "SUBSCRIBE response: " << req.subscriberId << "," << resp.success;
}

void Core::UnSubscribe(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg)
{
    UnSubscribeMessage req(msg);
    UnSubACKMessage resp;

    DLOG(INFO) << "UNSUBSCRIBE: " << req.subscriberId << "," << req.topic;

    pssc_write_guard guard(rwlckTopics);
    if (topics.find(req.topic) == topics.end())
    {
        resp.success = true;
    }
    else
    {
        try {
            auto& subscribers = topics.at(req.topic);
//            auto subscriber = std::find(subscribers.begin(), subscribers.end(), req.subscriberId);
//            if (subscriber != subscribers.end())
//            {
//                subscribers.erase(subscriber);
//                DLOG(INFO) << "UNSUBSCRIBE: OK, count of subscriber:" << subscribers.size();
//            }

            subscribers.remove(req.subscriberId);
            DLOG(INFO) << "UNSUBSCRIBE: OK, count of subscriber:" << subscribers.size();

            resp.success = true;
        }
        catch (...)
        {
            DLOG(ERROR) << "UNSUBSCRIBE: ???";
            resp.success = false;
        }
    }

    resp.messageId = req.messageId;
    conn->PendMessage(resp.toTCPMessage());
    DLOG(INFO) << "UNSUBSCRIBE response: " << req.subscriberId << "," << resp.success;
}


void Core::AdvertiseService(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg)
{
    AdvertiseServiceMessage req(msg);
    AdvSrvACKMessage ack;
    ack.messageId = req.messageId;

    DLOG(INFO) << "ADVERTISE SERVICE: " << req.advertiserId << "," << req.srv_name;

    pssc_write_guard guard(rwlckSrvs);
    if (srvs.find(req.srv_name) != srvs.end())
    {
        ack.success = false;

        conn->PendMessage(ack.toTCPMessage());
        DLOG(INFO) << "NOT DONE.";
    }
    else
    {
        srvs.insert(std::make_pair(req.srv_name, req.advertiserId));
        ack.success = true;

        conn->PendMessage(ack.toTCPMessage());

        DLOG(INFO) << "DONE.";
    }
}


void Core::CallService(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg)
{
    ServiceCallMessage req(msg);

    DLOG(INFO) << "CALL SERVICE: callerId:" << req.callerId
            << ", messageId:" << req.messageId
            << ", srv_name:" << req.srv_name;

    pssc_read_guard guard(rwlckSrvs);
    auto fd = srvs.find(req.srv_name);
    if (fd == srvs.end())
    {
        ServiceResponseMessage resp;
        resp.messageId = req.messageId;
        resp.success = false;
        conn->PendMessage(resp.toTCPMessage());
        DLOG(INFO) << "NOT DONE.";
    }
    else
    {
        try {
            auto srv_conn =  nodes.at(fd->second);
            srv_conn->PendMessage(msg);
            DLOG(INFO) << "DONE.";
        } catch (...) {
            ServiceResponseMessage resp;
            resp.messageId = req.messageId;
            resp.success = false;
            conn->PendMessage(resp.toTCPMessage());

            DLOG(INFO) << "NOT DONE: Closed.";
        }
    }
}

void Core::ResponseService(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg)
{
    ServiceResponseMessage req(msg);

    DLOG(INFO) << "RESPONSE SERVICE: clientId:" << req.callerId
                << ", messageId:" << req.messageId;
    auto srv_conn =  nodes.at(req.callerId);

    srv_conn->PendMessage(msg);
}

void Core::CloseService(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg)
{
    CloseServiceMessage req(msg);
    CloseSrvACKMessage resp;
    resp.messageId = req.messageId;

    DLOG(INFO) << "CLOSE SERVICE: advertiserId:" << req.advertiserId
            << ", messageId:" << req.messageId
            << ", srv_name:" << req.srv_name;

    pssc_write_guard guard(rwlckSrvs);
    auto fd = srvs.find(req.srv_name);
    if (fd == srvs.end() || fd->second != req.advertiserId)
    {
        resp.success = false;
        DLOG(INFO) << "NOT DONE.";
    }
    else
    {
        srvs.erase(fd);
        resp.success = true;
        DLOG(INFO) << "DONE.";
    }

    conn->PendMessage(resp.toTCPMessage());
}

int Core::Start()
{
    DLOG(INFO) << "start service.";
    server->Start();
    return 0;
}

}
