/*
 * AdvertiseServiceMessage.h
 *
 *  Created on: Apr 25, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_MSGS_ADVERTISESERVICEMESSAGE_H_
#define INCLUDE_PSSC_PROTOCOL_MSGS_ADVERTISESERVICEMESSAGE_H_


#include "PSSCMessage.h"

namespace pssc {

class AdvertiseServiceMessage : public PSSCMessage
{
public:
    // | INS | ID | ADVERTISER_ID | SIZE_OF_SRV_NAME | SRV_NAME |
    static const pssc_ins INS = Ins::ADDVERTISE_SERVICE;
    static const pssc_size SIZE_OF_MESSAGE_NECCESSARY =
            SIZE_OF_PSSC_INS + SIZE_OF_PSSC_ID * 2 + SIZE_OF_SIZE;

    pssc_id advertiserId;
    std::string srv_name;

    AdvertiseServiceMessage() = default; // @suppress("Class members should be properly initialized")

    AdvertiseServiceMessage(std::shared_ptr<TCPMessage> msg)
    {
        // INS has been taken
        msg->NextData(messageId);
        msg->NextData(advertiserId);
        msg->NextData(srv_name);
    }

    std::shared_ptr<TCPMessage> toTCPMessage() override
    {
        auto msg = TCPMessage::Generate(SIZE_OF_MESSAGE_NECCESSARY + srv_name.size());
        msg->AppendData(INS);
        msg->AppendData(messageId);
        msg->AppendData(advertiserId);
        msg->AppendData(srv_name);
        return msg;
    }
};

}


#endif /* INCLUDE_PSSC_PROTOCOL_MSGS_ADVERTISESERVICEMESSAGE_H_ */
