/*
 * SubACKMessage.h
 *
 *  Created on: Apr 25, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_MSGS_SUBACKMESSAGE_H_
#define INCLUDE_PSSC_PROTOCOL_MSGS_SUBACKMESSAGE_H_

#include "PSSCMessage.h"

namespace pssc {

class SubACKMessage : public PSSCMessage
{
public:
    // | INS | ID | SUCCESS |
    static const pssc_ins INS = Ins::SUBACK;
    static const pssc_size SIZE_OF_MESSAGE = SIZE_OF_PSSC_INS + SIZE_OF_PSSC_ID + SIZE_OF_BOOL;

    bool success;

    SubACKMessage() = default; // @suppress("Class members should be properly initialized")

    SubACKMessage(std::shared_ptr<TCPMessage> msg)
    {
        // INS has been taken
        msg->NextData(messageId);
        msg->NextData(success);
    }

    std::shared_ptr<TCPMessage> toTCPMessage() override
    {
        auto msg = TCPMessage::Generate(SIZE_OF_MESSAGE);
        msg->AppendData(INS);
        msg->AppendData(messageId);
        msg->AppendData(success);
        return msg;
    }
};

}


#endif /* INCLUDE_PSSC_PROTOCOL_MSGS_SUBACKMESSAGE_H_ */
