/*
 * PSSCMessage.h
 *
 *  Created on: Apr 25, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_MSGS_PSSCMESSAGE_H_
#define INCLUDE_PSSC_PROTOCOL_MSGS_PSSCMESSAGE_H_

#include <memory>
#include "pssc/transport/tcp/TCPConnection.h"
#include "pssc/protocol/types.h"
#include "pssc/protocol/Instruction.h"

namespace pssc {

using trs::TCPConnection;
using trs::TCPMessage;

class PSSCMessage
{
protected:
    std::shared_ptr<TCPConnection> conn;
public:
    pssc_ins ins;
    pssc_id messageId;
    virtual ~PSSCMessage() = default;
    virtual std::shared_ptr<TCPMessage> toTCPMessage() = 0;
};

}


#endif /* INCLUDE_PSSC_PROTOCOL_MSGS_PSSCMESSAGE_H_ */
