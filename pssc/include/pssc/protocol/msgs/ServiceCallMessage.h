/*
 * CallMessage.h
 *
 *  Created on: Apr 26, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_MSGS_SERVICECALLMESSAGE_H_
#define INCLUDE_PSSC_PROTOCOL_MSGS_SERVICECALLMESSAGE_H_

#include "PSSCMessage.h"

namespace pssc {

class ServiceCallMessage : public PSSCMessage
{
private:
    std::shared_ptr<TCPMessage> msg;

public:
    // | INS | ID | CALLER_ID | SIZE_OF_SRV_NAME | SRV_NAME | SIZE_OF_DATA | DATA |
    static const pssc_ins INS = Ins::SERVICE_CALL;
    static const pssc_size SIZE_OF_MESSAGE_NECCESSARY =
            SIZE_OF_PSSC_INS + SIZE_OF_PSSC_ID * 2 + SIZE_OF_SIZE * 2;

    pssc_id callerId;
    std::string srv_name;
    size_t sizeOfData;
    pssc_bytes data;

    ServiceCallMessage() : data(nullptr) {} // @suppress("Class members should be properly initialized")

    ServiceCallMessage(std::shared_ptr<TCPMessage> msg) : msg(msg)
    {
        // INS has been taken
        msg->NextData(messageId);
        msg->NextData(callerId);
        msg->NextData(srv_name);
        msg->NextData(sizeOfData);
        data = msg->GetDataPointerWithOffset();
    }

    std::shared_ptr<TCPMessage> toTCPMessage() override
    {
        auto msg = TCPMessage::Generate(SIZE_OF_MESSAGE_NECCESSARY + srv_name.size() + sizeOfData);
        msg->AppendData(INS);
        msg->AppendData(messageId);
        msg->AppendData(callerId);
        msg->AppendData(srv_name);
        msg->AppendData(sizeOfData);
        msg->AppendData(data, sizeOfData);
        return msg;
    }
};

}


#endif /* INCLUDE_PSSC_PROTOCOL_MSGS_SERVICECALLMESSAGE_H_ */
