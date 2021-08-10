/*
 * Int.h
 *
 *  Created on: Aug 6, 2021
 *      Author: ubuntu
 */

#ifndef TINY_ROS_SAMPLE_PUB_SUB_INTEGER_H_
#define TINY_ROS_SAMPLE_PUB_SUB_INTEGER_H_

#include <cstddef>
#include <cstdint>

struct Integer
{
    int data;

    Integer() = default;
    Integer(size_t size, std::uint8_t* data)
    {
        this->data = *(int*)data;
    }

    size_t Size()
    {
        return sizeof(int);
    }
    std::uint8_t* Data()
    {
        return (std::uint8_t*)&data;
    }
};


#endif /* TINY_ROS_SAMPLE_PUB_SUB_INTEGER_H_ */
