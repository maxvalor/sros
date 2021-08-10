/*
 * RegACKMessage.h
 *
 *  Created on: Apr 25, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_MSGS_REGACKMESSAGE_H_
#define INCLUDE_PSSC_PROTOCOL_MSGS_REGACKMESSAGE_H_

#include "PSSCMessage.h"

namespace pssc {

class RegACKMessage : public PSSCMessage
{
public:
    // | INS | ID | SUCCESS | NODE_ID |
    static const pssc_ins INS = Ins::REGACK;
    static const pssc_size SIZE_OF_MESSAGE = SIZE_OF_PSSC_INS + SIZE_OF_PSSC_ID + SIZE_OF_BOOL + SIZE_OF_PSSC_ID;

    bool success;
    pssc_id nodeId;

    RegACKMessage() = default; // @suppress("Class members should be properly initialized")

    RegACKMessage(std::shared_ptr<TCPMessage> msg)
    {
        // INS has been taken
        msg->NextData(messageId);
        msg->NextData(success);
        if (success)
        {
            msg->NextData(nodeId);
        }
    }

    std::shared_ptr<TCPMessage> toTCPMessage() override
    {
        auto msg = TCPMessage::Generate(SIZE_OF_MESSAGE);
        msg->AppendData(INS);
        msg->AppendData(messageId);
        msg->AppendData(success);
        if (success)
        {
            msg->AppendData(nodeId);
        }
        return msg;
    }
};

}



#endif /* INCLUDE_PSSC_PROTOCOL_MSGS_REGACKMESSAGE_H_ */
