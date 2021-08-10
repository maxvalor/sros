/*
 * PublishMessage.h
 *
 *  Created on: Apr 25, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_MSGS_PUBLISHMESSAGE_H_
#define INCLUDE_PSSC_PROTOCOL_MSGS_PUBLISHMESSAGE_H_

#include "PSSCMessage.h"

namespace pssc {

class PublishMessage : public PSSCMessage
{
private:
    std::shared_ptr<TCPMessage> msg;

public:
    // | INS | ID | PUBLISHER_ID | SIZE_OF_TOPIC | TOPIC | SIZE_OF_DATA | DATA | FEEDBACK |
    static const pssc_ins INS = Ins::PUBLISH;
    static const pssc_size SIZE_OF_MESSAGE_NECCESSARY =
            SIZE_OF_PSSC_INS + SIZE_OF_PSSC_ID * 2 + SIZE_OF_SIZE * 2 + SIZE_OF_BOOL;

    pssc_id publisherId;
    std::string topic;
    size_t sizeOfData;
    pssc_bytes data;
    bool feedback;

    PublishMessage() : feedback(false), data(nullptr) {} // @suppress("Class members should be properly initialized")

    PublishMessage(std::shared_ptr<TCPMessage> msg) : msg(msg)
    {
        // INS has been taken
        msg->NextData(messageId);
        msg->NextData(publisherId);
        msg->NextData(topic);
        msg->NextData(sizeOfData);
        data = msg->GetDataPointerWithOffset();
        msg->IgnoreBytes(sizeOfData);
        msg->NextData(feedback);
    }

    std::shared_ptr<TCPMessage> toTCPMessage() override
    {
        auto msg = TCPMessage::Generate(SIZE_OF_MESSAGE_NECCESSARY + topic.size() + sizeOfData);
        msg->AppendData(INS);
        msg->AppendData(messageId);
        msg->AppendData(publisherId);
        msg->AppendData(topic);
        msg->AppendData(sizeOfData);
        msg->AppendData(data, sizeOfData);
        msg->AppendData(feedback);
        return msg;
    }
};

}


#endif /* INCLUDE_PSSC_PROTOCOL_MSGS_PUBLISHMESSAGE_H_ */
