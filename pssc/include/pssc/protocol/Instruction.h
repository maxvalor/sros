/*
 * commands.h
 *
 *  Created on: Mar 9, 2021
 *      Author: ubuntu
 */

#ifndef PSSC_INSTRUCTION_H_
#define PSSC_INSTRUCTION_H_

namespace pssc {

enum Instruction
{
    REGISTER = 0x00,
    REGACK,
    UNREGISTER,
    SUBSCRIBE,
    SUBACK,
    UNSUBSCRIBE,
    UNSUBACK,
    PUBLISH,
    ADDVERTISE_SERVICE,
    ADVSRVACK,
    CLOSE_SERVICE,
    CLOSESRVACK,
    SERVICE_CALL,
    SERVICE_RESPONSE,
    QUERY_SUBSCRIBER_NUMBER,
    QUERY_SUBSCRIBER_NUMBER_ACK,
    UNKOWN,
};

using Ins = Instruction;

}


#endif /* PSSC_INSTRUCTION_H_ */
