/*
 * Notifier.h
 *
 *  Created on: Apr 21, 2021
 *      Author: ubuntu
 */

#ifndef PSSC_NOTIFIER_H_
#define PSSC_NOTIFIER_H_

#include <mutex>
#include <condition_variable>
#include <glog/logging.h>

namespace util {

class Notifier
{
    std::mutex mtx;
    std::condition_variable cv;
public:

    template<typename Rep, typename Period>
    std::cv_status wait_for(const std::chrono::duration<Rep, Period>& time)
    {
        std::unique_lock<std::mutex> lck(mtx);
        return cv.wait_for(lck, time);
    }

    void wait()
    {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck);
    }

    void notify_one()
    {
        auto i = reinterpret_cast<int*>(this);
        cv.notify_one();
    }

    void notify_all()
    {
        cv.notify_all();
    }
};

}


#endif /* PSSC_NOTIFIER_H_ */
