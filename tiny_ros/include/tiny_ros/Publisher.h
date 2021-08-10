#pragma once

#include <unordered_map>
#include <mutex>
#include <functional>
#include <glog/logging.h>

namespace tiny_ros
{
template <typename T>
class Publisher
{
    std::recursive_mutex mtx;
    std::unordered_map<std::uint64_t, std::function<void(std::shared_ptr<T>)>> subscribers;
    std::uint64_t id = 0;
    std::string topic;

    std::function<void(std::string, size_t, std::uint8_t*)> pubToCore;

    friend class NodeHandle;

    template <typename F>
    std::uint64_t AddSubscriber(F&& f)
    {
        std::lock_guard<std::recursive_mutex> lck(mtx);
        auto sid = ++id;
        subscribers.insert(std::make_pair(id, f));
        return sid;
    }

    size_t RemoveSubscriber(std::uint64_t sid)
    {
        std::lock_guard<std::recursive_mutex> lck(mtx);

        auto it = subscribers.find(sid);
        if (it != subscribers.end())
        {
            subscribers.erase(it);
        }

        return subscribers.size();
    }

    void ExtPublish(size_t size, std::uint8_t* data)
    {
        std::lock_guard<std::recursive_mutex> lck(mtx);
        auto msg = std::make_shared<T>(size, data);
        auto subs = subscribers;
        for (auto subscriber : subs)
        {
            subscriber.second(msg);
        }
    }

public:
    void Publish(T msg)
    {
        std::lock_guard<std::recursive_mutex> lck(mtx);
        auto pMsg = std::make_shared<T>();
        *pMsg = msg;
        auto subs = subscribers;
        for (auto subscriber : subs)
        {
            subscriber.second(pMsg);
        }

        pubToCore(topic, msg->Size(), msg->Data());
    }

    void Publish(std::shared_ptr<T> msg)
    {
        std::lock_guard<std::recursive_mutex> lck(mtx);
        auto subs = subscribers;
        for (auto subscriber : subs)
        {
            subscriber.second(msg);
        }

        LOG(INFO) << "Publish to core";

        pubToCore(topic, msg->Size(), msg->Data());
    }
};
}
