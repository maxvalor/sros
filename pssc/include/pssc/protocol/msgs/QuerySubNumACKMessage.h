/*
 * QuerySubNumACKMessage.h
 *
 *  Created on: Apr 25, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_MSGS_QUERYSUBNUMACKMESSAGE_H_
#define INCLUDE_PSSC_PROTOCOL_MSGS_QUERYSUBNUMACKMESSAGE_H_


#include "PSSCMessage.h"

namespace pssc {

class QuerySubNumACKMessage : public PSSCMessage
{
public:
    // | INS | MID | SUBSCRIBER NUMBER |
    static const pssc_ins INS = Ins::QUERY_SUBSCRIBER_NUMBER_ACK;
    static const pssc_size SIZE_OF_MESSAGE_NECCESSARY =
            SIZE_OF_PSSC_INS + SIZE_OF_PSSC_ID  + SIZE_OF_SIZE;

    pssc_size subNum;

    QuerySubNumACKMessage() = default; // @suppress("Class members should be properly initialized")

    QuerySubNumACKMessage(std::shared_ptr<TCPMessage> msg)
    {
        // INS has been taken
        msg->NextData(messageId);
        msg->NextData(subNum);
    }

    std::shared_ptr<TCPMessage> toTCPMessage() override
    {
        auto msg = TCPMessage::Generate(SIZE_OF_MESSAGE_NECCESSARY);
        msg->AppendData(INS);
        msg->AppendData(messageId);
        msg->AppendData(subNum);
        return msg;
    }
};

}


#endif /* INCLUDE_PSSC_PROTOCOL_MSGS_QUERYSUBNUMACKMESSAGE_H_ */
