/*
 * NodeHandle.h
 *
 *  Created on: Jul 29, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_NODEHANDLE_H_
#define INCLUDE_NODEHANDLE_H_

#include <unordered_map>
#include <mutex>
#include <memory>
#include <vector>
#include "Publisher.h"
#include "Subscriber.h"
#include <flatbuffers/flatbuffers.h>

namespace doggy {

#define MULTI_PROCESS

#ifdef MULTI_PROCESS
#include "../../../pssc/include/pssc/pssc.h"
#endif

using lock_guard = std::lock_guard<std::mutex>;

template <typename T>
class Topic
{
    std::mutex mtx;
    std::unordered_map<std::uint64_t, std::function<void(T)>> subscribers;
    std::uint64_t id = 0;
    std::string topic;
    friend class NodeHandle;

    template <typename F>
    std::uint64_t addSubscriber(F&& f)
    {
        lock_guard lck(mtx);
        auto sid = id++;
        subscribers.insert(std::make_pair(id, f));
        return sid;
    }

    void removeSubscriber(std::uint64_t sid)
    {
        lock_guard lck(mtx);
        auto it = subscribers.find(sid);
        if (it != subscribers.end())
        {
            subscribers.erase(it);
        }
    }
public:
    void publish(T msg, bool bExtPub)
    {
        lock_guard lck(mtx);
        for (auto subscriber : subscribers)
        {
            subscriber(msg);
        }
    }
};

class NodeHandle
{
    static NodeHandle* singleton;

    std::unordered_map<std::string, void*> topics;
    std::unordered_map<std::string,
        std::pair<std::uint64_t, std::function<void(std::uint8_t*, size_t)>>> mtTopicCallbacks;
#ifdef MULTI_PROCESS
    pssc::Node node;
#endif

private:
    template <typename T>
    Topic<T>* resovle(std::string& name)
    {
        Topic<T> *publisher = nullptr;
        try
        {
            auto origin_topic = topics.at(name);
            publisher = static_cast<Topic<T>*>(origin_topic);
        }
        catch(...)
        {
            publisher = new Topic<T>();
            publisher->topic = name;
            topics.insert(std::pair<std::string, void*>(name, static_cast<void*>(publisher)));
        }
        return publisher;
    }

public:
    NodeHandle(std::uint32_t port = 20001)
    {
        if (port != 0)
        {
            node.SetTopicCallback([this](std::string topic, std::uint8_t* data, size_t size)
            {
                DLOG(INFO) << "topic:" << topic << ", total size:" << size;
                auto funcPublish = mtTopicCallbacks.at(topic);
                funcPublish.second(data, size);
            });
            node.Initialize(port);
        }
    }

    static NodeHandle& Instance()
    {
        if (singleton == nullptr)
        {
            singleton = new NodeHandle();
        }

        return *singleton;
    }

    template <typename MSG>
    Subscriber Subscribe(std::string topic, std::function<void(std::shared_ptr<MSG>)> f)
    {
        Subscriber sub;

        auto mtFuncPublish = [this](std::uint8_t* data, size_t size) {
            auto msg = std::make_shared<MSG>(data, size);
        };

        auto funcPublish = mtTopicCallbacks.at(topic);
        if (funcPublish == mtTopicCallbacks.end())
        {
            mtTopicCallbacks.insert(std::make_pair(topic, std::make_pair(1u, mtFuncPublish)));
            node.Subscribe(topic);
        }
        else
        {
            funcPublish.first = funcPublish.first + 1;
        }

        auto t = resovle<MSG>(topic);
        auto sid = t->addSubscriber(f);

        sub.shutdown = [this, sid]() {
            //remove
            auto t = resovle<MSG>(topic);
            t->removeSubscriber(sid);
            auto funcPublish = mtTopicCallbacks.at(topic);
            funcPublish.first = funcPublish.first - 1;
            if (funcPublish.first == 0)
            {
                node.UnSubscribe(topic);
            }
        };

        return sub;
    }

    template <typename MSG>
    Publisher Advertise(std::string topic)
    {
        auto inFuncPublish = [this, topic](std::shared_ptr<Message> msg, bool bExtPub)
        {
            auto t = resovle<MSG>(topic);
            t->publish(msg, bExtPub);

            if (bExtPub)
            {
                auto data = msg->Serialize();
                node.Publish(topic, data.raw(), data.size(), false);
            }

        };

        return Publisher(inFuncPublish);
    }
};

}


#endif /* INCLUDE_NODEHANDLE_H_ */
