/*
 * SerializedMessage.h
 *
 *  Created on: Aug 9, 2021
 *      Author: ubuntu
 */

#ifndef TINY_ROS_SAMPLE_PUB_SUB_SERIALIZEDMESSAGE_H_
#define TINY_ROS_SAMPLE_PUB_SUB_SERIALIZEDMESSAGE_H_

#include <cstddef>
#include <cstdint>
#include <memory>

struct SerializedMessage
{
    size_t size;
    std::uint8_t* data;

    size_t Size();
    std::uint8_t* Data();
};


#endif /* TINY_ROS_SAMPLE_PUB_SUB_SERIALIZEDMESSAGE_H_ */
