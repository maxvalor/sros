/*
 * ServiceClient.h
 *
 *  Created on: Aug 10, 2021
 *      Author: ubuntu
 */

#pragma once

#include <functional>
namespace tiny_ros
{

template <typename T>
class ServiceClient
{
    std::function<bool(std::shared_ptr<T> msg)> call;
    friend class NodeHandle;
public:
    ServiceClient() : call(nullptr)
    {

    }

//    ServiceClient(ServiceClient& o)
//    {
//        call = o.call;
//    }
//
//    ServiceClient(ServiceClient&& o)
//    {
//        call = o.call;
//        o.call = nullptr;
//    }


    bool isValid()
    {
        return call != nullptr;
    }

//    bool Call(T srv)
//    {
//        if (call != nullptr)
//        {
//            return call(srv);
//        }
//        return false;
//    }

    bool Call(std::shared_ptr<T> srv)
    {
        if (call != nullptr)
        {
            return call(srv);
        }
        return false;
    }
};

}
