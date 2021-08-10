/*
 * SerializedInteger.h
 *
 *  Created on: Aug 9, 2021
 *      Author: ubuntu
 */

#ifndef TINY_ROS_SAMPLE_PUB_SUB_SERIALIZEDINTEGER_H_
#define TINY_ROS_SAMPLE_PUB_SUB_SERIALIZEDINTEGER_H_

#include "Integer.h"
#include "SerializedMessage.h"

SerializedMessage SerializeMessage(Integer& i)
{
    SerializedMessage msg;
    msg.size = sizeof(i.data);
    msg.data = &i;

    return msg;
}

SerializedMessage SerializeMessage(std::shared_ptr<Integer> i)
{
    SerializedMessage msg;
    msg.size = sizeof(i->data);
    msg.data = i.get();

    return msg;
}

#endif /* TINY_ROS_SAMPLE_PUB_SUB_SERIALIZEDINTEGER_H_ */
