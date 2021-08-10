/*
 * RegisterMessage.h
 *
 *  Created on: Apr 25, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_MSGS_REGISTERMESSAGE_H_
#define INCLUDE_PSSC_PROTOCOL_MSGS_REGISTERMESSAGE_H_

#include "PSSCMessage.h"

namespace pssc {

class RegisterMessage : public PSSCMessage
{
public:
    // | INS | ID |
    static const pssc_ins INS = Ins::REGISTER;
    static const pssc_size SIZE_OF_MESSAGE = SIZE_OF_PSSC_INS + SIZE_OF_PSSC_ID;

    RegisterMessage() = default; // @suppress("Class members should be properly initialized")

    RegisterMessage(std::shared_ptr<TCPMessage> msg)
    {
        // INS has been taken
        msg->NextData(messageId);
    }

    std::shared_ptr<TCPMessage> toTCPMessage() override
    {
        auto msg = TCPMessage::Generate(SIZE_OF_MESSAGE);
        msg->AppendData(INS);
        msg->AppendData(messageId);
        return msg;
    }
};

}



#endif /* INCLUDE_PSSC_PROTOCOL_MSGS_REGISTERMESSAGE_H_ */
