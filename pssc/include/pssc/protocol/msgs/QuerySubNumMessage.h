/*
 * QuerySubNumMessage.h
 *
 *  Created on: Apr 25, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_MSGS_QUERYSUBNUMMESSAGE_H_
#define INCLUDE_PSSC_PROTOCOL_MSGS_QUERYSUBNUMMESSAGE_H_


#include "PSSCMessage.h"

namespace pssc {

class QuerySubNumMessage : public PSSCMessage
{
public:
    // | INS | ID | INQUIRER_ID | SIZE_OF_TOPIC_NAME | TOPIC_NAME |
    static const pssc_ins INS = Ins::QUERY_SUBSCRIBER_NUMBER;
    static const pssc_size SIZE_OF_MESSAGE_NECCESSARY =
            SIZE_OF_PSSC_INS + SIZE_OF_PSSC_ID * 2 + SIZE_OF_SIZE;

    pssc_id inquirerId;
    std::string topic;

    QuerySubNumMessage() = default; // @suppress("Class members should be properly initialized")

    QuerySubNumMessage(std::shared_ptr<TCPMessage> msg)
    {
        // INS has been taken
        msg->NextData(messageId);
        msg->NextData(inquirerId);
        msg->NextData(topic);
    }

    std::shared_ptr<TCPMessage> toTCPMessage() override
    {
        auto msg = TCPMessage::Generate(SIZE_OF_MESSAGE_NECCESSARY + topic.size());
        msg->AppendData(INS);
        msg->AppendData(messageId);
        msg->AppendData(inquirerId);
        msg->AppendData(topic);
        return msg;
    }
};

}


#endif /* INCLUDE_PSSC_PROTOCOL_MSGS_QUERYSUBNUMMESSAGE_H_ */
