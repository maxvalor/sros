#pragma once

#include <functional>

namespace tiny_ros
{
class Subscriber {
    std::function<void(void)> shutdown;

    friend class NodeHandle;
    Subscriber(std::function<void(void)> f) : shutdown(f) {}
public:
    Subscriber() : shutdown(nullptr) {}
    void Shutdown()
    {
        if (shutdown != nullptr)
        {
            shutdown();
        }
    }
};
}
