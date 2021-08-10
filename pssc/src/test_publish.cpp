//============================================================================
// Name        : light.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "pssc/protocol/Node.h"
#include <stdio.h>

class Rate
{
    std::uint64_t base_time;
    std::uint64_t counter;
    struct timeval tv_start, tv_end;

    std::mutex mtx_cv;
    std::condition_variable cv;
public:
    enum {
        SLEEP_RTC_OK,
        SLEEP_RTC_NOT_SLEEP,
        SLEEP_RTC_INTERRUPTED,
    };
    Rate(double hz) : counter(0)
    {
        base_time = 1000000000 / hz;
    }

    void interrupt()
    {
        counter = 0;
        // wakeup
        cv.notify_one();
    }

    int sleep()
    {
        int rlt;

        if (counter > 0)
        {
            gettimeofday(&tv_end, NULL);
            std::uint64_t time_left =  base_time * (counter + 1) - ((tv_end.tv_sec - tv_start.tv_sec) * 1000000 + (tv_end.tv_usec - tv_start.tv_usec)) * 1000;
            if (time_left > 0)
            {
                // std::this_thread::sleep_for(std::chrono::nanoseconds(time_left));
                std::unique_lock <std::mutex> lck(mtx_cv);
                auto flag = cv.wait_for(lck, std::chrono::nanoseconds(time_left));
                rlt = ((flag == std::cv_status::timeout) ? SLEEP_RTC_OK : SLEEP_RTC_INTERRUPTED);
            }
            else
            {
                std::this_thread::yield();
                rlt = SLEEP_RTC_NOT_SLEEP;
            }

            if (counter > 10000000u && time_left > 0)
            {
                auto at = base_time * counter;
                auto ts = at / 1000000000;
                auto tus = (at - ts * 1000000000) / 1000;
                tv_start.tv_sec += ts;
                tv_start.tv_usec += tus;
                counter = 1;
            }
            else
            {
                ++counter;
            }
        }
        else
        {
            gettimeofday(&tv_start, NULL);
            // std::this_thread::sleep_for(std::chrono::nanoseconds(base_time));
            std::unique_lock <std::mutex> lck(mtx_cv);
            auto flag = cv.wait_for(lck, std::chrono::nanoseconds(base_time));
            rlt = ((flag == std::cv_status::timeout) ? SLEEP_RTC_OK : SLEEP_RTC_INTERRUPTED);
            ++counter;
        }


        return rlt;
    }

};

#define SEND_SIZE 1920 * 1080 * 4 / 20  // 1080P png file size

int main(int argc, char*argv[]) {
    pssc::Node node;
    node.SetTopicCallback([](std::string topic, std::uint8_t* data, size_t size)
    {
        LOG(INFO) << "topic:" << topic;
        int i;
        memcpy(&i, data, sizeof(i));
        LOG(INFO) << "data:" << i;
        LOG(INFO) << "total size:" << size;
    });
    node.Initialize(20001);
    // node.Subscribe("test_topic");
    std::uint8_t* data = new std::uint8_t[SEND_SIZE];

    Rate r(25);

    struct timeval tv_start,tv_end;
    for (int i = 0; i < 10000; ++i)
    {
        LOG(INFO) << "publish start:" << i;
//        memcpy(data, &i, sizeof(int));
        gettimeofday(&tv_start, NULL);
        memcpy(data, &tv_start, sizeof(tv_start));
        if (SEND_SIZE < 10000)
        {
            node.Publish("test_topic", data, SEND_SIZE, false);
            LOG(INFO) << "publish finished";
        }
        else if (node.QuerySubNum("test_topic"))
        {
            node.Publish("test_topic", data, SEND_SIZE, false);
            LOG(INFO) << "publish finished";
        }

        r.sleep();

    }

    getchar();

    return 0;
}
