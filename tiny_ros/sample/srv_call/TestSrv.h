/*
 * TestSrv.h
 *
 *  Created on: Aug 10, 2021
 *      Author: ubuntu
 */

#ifndef TINY_ROS_SAMPLE_SRV_CALL_TESTSRV_H_
#define TINY_ROS_SAMPLE_SRV_CALL_TESTSRV_H_


struct TestSrv
{
    TestSrv()
    {

    }

    struct Request
    {
        int data;

        Request() = default;

        Request(size_t size, std::uint8_t* data)
        {
            this->data = *(int*)data;
        }
    } request;

    struct Response
    {
        int data;

        Response() = default;

        Response(size_t size, std::uint8_t* data)
        {
            this->data = *(int*)data;
        }

        size_t Size()
        {
            return sizeof(Response);
        }
        std::uint8_t* Data()
        {
            return (std::uint8_t*)this;
        }
    } response;

    size_t Size()
    {
        return sizeof(TestSrv);
    }
    std::uint8_t* Data()
    {
        return (std::uint8_t*)this;
    }
};


#endif /* TINY_ROS_SAMPLE_SRV_CALL_TESTSRV_H_ */
