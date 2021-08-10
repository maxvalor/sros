/*
 * ServiceResponseMessage.h
 *
 *  Created on: Apr 26, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_MSGS_SERVICERESPONSEMESSAGE_H_
#define INCLUDE_PSSC_PROTOCOL_MSGS_SERVICERESPONSEMESSAGE_H_


#include "PSSCMessage.h"

namespace pssc {

class ServiceResponseMessage : public PSSCMessage
{
private:
    std::shared_ptr<TCPMessage> msg;

public:
    // | INS | ID | CLIENT_ID | SUCCESS | SIZE_OF_DATA | DATA |
    static const pssc_ins INS = Ins::SERVICE_RESPONSE;
    static const pssc_size SIZE_OF_MESSAGE_NECCESSARY =
            SIZE_OF_PSSC_INS + SIZE_OF_PSSC_ID * 2 + SIZE_OF_BOOL + SIZE_OF_SIZE;

    pssc_id callerId;
    bool success;
    size_t sizeOfData;
    pssc_bytes data;

    ServiceResponseMessage() : data(nullptr), sizeOfData(0u) {} // @suppress("Class members should be properly initialized")

    ServiceResponseMessage(std::shared_ptr<TCPMessage> msg) : msg(msg)
    {
        // INS has been taken
        msg->NextData(messageId);
        msg->NextData(callerId);
        msg->NextData(success);
        if (success)
        {
            msg->NextData(sizeOfData);
            data = msg->GetDataPointerWithOffset();
        }
    }

    std::shared_ptr<TCPMessage> toTCPMessage() override
    {
        auto msg = TCPMessage::Generate(SIZE_OF_MESSAGE_NECCESSARY + sizeOfData);
        msg->AppendData(INS);
        msg->AppendData(messageId);
        msg->AppendData(callerId);
        msg->AppendData(success);
        if (success)
        {
            msg->AppendData(sizeOfData);
            msg->AppendData(data, sizeOfData);
        }
        return msg;
    }
};

}


#endif /* INCLUDE_PSSC_PROTOCOL_MSGS_SERVICERESPONSEMESSAGE_H_ */
