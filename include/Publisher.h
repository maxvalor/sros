#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#include <functional>
#include "Message.h"

namespace doggy {

class Publisher {
    using funcTypePublish = std::function<void(std::shared_ptr<Message>)>;
    funcTypePublish f;

    friend class ThreadHandle;
    Publisher(funcTypePublish f) : f(f) {}

public:
    Publisher() : f(nullptr) {}
    template <typename T>
    void publish(T& msg)
    {
        if (f != nullptr)
        {
            std::shared_ptr<Message> sMsg(new T(std::move(msg)));
            f(sMsg);
        }
    }

    template <typename T>
    void publish(std::shared_ptr<T> msg)
    {
        if (f != nullptr)
        {
            f(msg);
        }
    }
};

} /* mini_ros */

#endif
