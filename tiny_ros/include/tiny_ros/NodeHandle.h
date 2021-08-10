#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <mutex>
#include <functional>

#include "../../../pssc/include/pssc/pssc.h"

#include "Publisher.h"
#include "Subscriber.h"
#include "Service.h"
#include "ServiceClient.h"

namespace tiny_ros
{
using lock_guard = std::lock_guard<std::recursive_mutex>;

class NodeHandle
{
    static NodeHandle* singleton;
    static const size_t MAX_SEND_WITH_NO_QUERY = 10000;
    std::unordered_map<std::string, void*> topics;
    std::unordered_map<std::string, void*> srvs;

    std::unordered_map<std::string, std::function<void(size_t, std::uint8_t*)>> extPublish;
    std::unordered_map<std::string, std::function<bool(size_t, std::uint8_t*, size_t*, std::uint8_t**)>> extCall;
    std::recursive_mutex topic_mtx;
    std::recursive_mutex srv_mtx;
    std::recursive_mutex mtxExtPub;
    std::recursive_mutex mtxExtCall;

    pssc::Node node;

private:
    template <typename T>
    Publisher<T>* resovle(std::string& name)
    {
        Publisher<T> *publisher = nullptr;
        try
        {
            auto origin_topic = topics.at(name);
            publisher = static_cast<Publisher<T>*>(origin_topic);
        }
        catch(...)
        {
            publisher = new Publisher<T>();
            publisher->topic = name;
            topics.insert(std::pair<std::string, void*>(name, static_cast<void*>(publisher)));
        }
        return publisher;
    }
public:
    NodeHandle()
    {
        node.SetTopicCallback([this](std::string topic, std::uint8_t* data, size_t size)
        {
            std::lock_guard<std::recursive_mutex> lck(mtxExtPub);
            auto publish = extPublish.at(topic);
            publish(size, data);
        });
        node.SetServiceCallback([&](std::string srv_name, std::uint8_t* data, size_t size, std::shared_ptr<pssc::Node::ResponseOperator> op)
        {
            LOG(INFO) << "Called.";
            std::lock_guard<std::recursive_mutex> lck(mtxExtCall);
            auto call = extCall.at(srv_name);
            std::uint8_t* respData = nullptr;
            size_t respSize = 0;
            bool rlt = call(size, data, &respSize, &respData);
            op->SendResponse(rlt, respData, respSize);
            delete[] respData;
        });
        node.Initialize(20001);
    }
    static NodeHandle& Instance()
    {
        if (singleton == nullptr)
        {
            singleton = new NodeHandle();
        }

        return *singleton;
    }
    template <typename T>
    Publisher<T>* advertise(std::string name)
    {
        std::lock_guard<std::recursive_mutex> lck(topic_mtx);
        auto publisher = resovle<T>(name);
        publisher->pubToCore = [this](std::string topic, size_t size, std::uint8_t* data){
            if (size > MAX_SEND_WITH_NO_QUERY)
            {
                if (node.QuerySubNum(topic) == 0)
                {
                    return;
                }
            }
            node.Publish(topic, data, size, false);
        };
        return publisher;
    }

    template <typename T, typename F>
    Subscriber subscribe(std::string name, F&& f)
    {
        Subscriber sub;
        std::lock_guard<std::recursive_mutex> lck(topic_mtx);
        Publisher<T>* topic = resovle<T>(name);
        auto sid = topic->AddSubscriber(f);

        sub.shutdown = [this, sid, topic, name]() {
            //remove
            std::lock_guard<std::recursive_mutex> lck(topic_mtx);
            auto left = topic->RemoveSubscriber(sid);
            if (left == 0)
            {
                node.UnSubscribe(name);
            }
        };

        // ext
        std::lock_guard<std::recursive_mutex> lck2(mtxExtPub);
        if (extPublish.find(name) == extPublish.end())
        {
            auto extPub = [topic](size_t size, std::uint8_t* data){
                topic->ExtPublish(size, data);
            };

            extPublish.insert(std::make_pair(name, extPub));
            node.Subscribe(name);
        }


        return sub;
    }

    template <typename T, typename F>
    Service<T>* advertiseService(std::string name, F&& f)
    {
        std::lock_guard<std::recursive_mutex> lck(srv_mtx);

        if (srvs.find(name) == srvs.end())
        {
            auto *srv = new Service<T>();
            //ext
            node.AdvertiseService(name);
            auto extF = [srv](size_t reqSize, std::uint8_t* reqData, size_t* respSize, std::uint8_t** respData){
                return srv->OnExtCall(reqSize, reqData, respSize, respData);
            };

            {
                std::lock_guard<std::recursive_mutex> lck(mtxExtCall);
                extCall.insert(std::make_pair(name, extF));
            }


            srv->call = f;
            srv->shutdown = [this, name, srv](){
                std::lock_guard<std::recursive_mutex> lck(srv_mtx);
                node.CloseService(name);
                srvs.erase(name);
                srv->call = nullptr;
                {
                    std::lock_guard<std::recursive_mutex> lck(mtxExtCall);
                    extCall.erase(name);
                }
            };

            srvs.insert(std::pair<std::string, void*>(name, static_cast<void*>(srv)));

            return srv;
        }

        return nullptr;
    }

    template <typename T>
    ServiceClient<T> serviceClient(std::string name)
    {
        ServiceClient<T> client;
        std::lock_guard<std::recursive_mutex> lck(srv_mtx);
        try
        {
            client.call = [this, name](std::shared_ptr<T> msg){
                std::lock_guard<std::recursive_mutex> lck(srv_mtx);
                if (srvs.find(name) == srvs.end())
                {

                    auto resp = node.RemoteCall(name, msg->Data(), msg->Size());
                    LOG(INFO) << "RemoteCall result:" << resp->success;
                    if (!resp->success)
                    {
                        return false;
                    }
                    msg->response = typename T::Response(resp->sizeOfData, resp->data);
                    return true;
                }
                else
                {
                    auto origin_srv = srvs.at(name);
                    Service<T>* srv = static_cast<Service<T>*>(origin_srv);
                    if (srv->call != nullptr)
                    {
                        return srv->call(msg);
                    }
                    else
                    {
                        auto resp = node.RemoteCall(name, msg->Data(), msg->Size());
                        if (!resp->success)
                        {
                            return false;
                        }
                        msg->response = typename T::Response(resp->sizeOfData, resp->data);
                        return true;
                    }
                }
            };
        }
        catch(...) {}

        return client;
    }
};

NodeHandle* NodeHandle::singleton = nullptr;

void init() { NodeHandle::Instance(); }
}
