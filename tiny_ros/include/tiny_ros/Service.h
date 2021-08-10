/*
 * Service.h
 *
 *  Created on: Aug 6, 2021
 *      Author: ubuntu
 */

#pragma once

#include <functional>
#include <glog/logging.h>
namespace tiny_ros
{
template <typename T>
class Service
{
    std::function<bool(std::shared_ptr<T>)> call;
    std::function<void(void)> shutdown;
    friend class NodeHandle;

    bool OnExtCall(size_t reqSize, std::uint8_t* reqData, size_t* respSize, std::uint8_t** respData)
    {
        auto msg = std::make_shared<T>();
        msg->request = typename T::Request(reqSize, reqData);
        auto rlt = call(msg);
        *respSize = msg->Size();
        *respData = new std::uint8_t[msg->Size()];
        memcpy(*respData, msg->response.Data(), msg->response.Size());
        LOG(INFO) << "OnExtCall";

        return rlt;
    }
public:
    Service() : shutdown(nullptr) {}
    void Shutdown()
    {
        if (shutdown != nullptr)
        {
            shutdown();
        }
    }
};
}

