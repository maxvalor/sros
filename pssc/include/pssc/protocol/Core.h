/*
 * PSSCCore.h
 *
 *  Created on: Apr 25, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_CORE_H_
#define INCLUDE_PSSC_PROTOCOL_CORE_H_


#include "pssc/util/IDGenerator.h"

#include <unordered_map>
#include <list>

#include "pssc/transport/tcp/TCPServer.h"
#include "types.h"
#include "pssc/protocol/msgs/pssc_msgs.h"

namespace pssc
{

using namespace trs;

class Core
{
public:
    Core(int port);
    int Start();
private:
    std::unique_ptr<TCPServer> server;

    IDGenerator<std::uint64_t> nodeIdGen;

    pssc_rw_mutex rwlckNodes;
    std::unordered_map<pssc_id, std::shared_ptr<TCPConnection>> nodes;

    pssc_rw_mutex rwlckTopics;
    std::unordered_map<std::string, std::list<pssc_id>> topics;

    pssc_rw_mutex rwlckSrvs;
    std::unordered_map<std::string, pssc_id> srvs;

    void OnConnected(std::shared_ptr<TCPConnection> conn);
    void OnDisconnected(std::shared_ptr<TCPConnection> conn);
    void DispatchMessage(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg);


    void Register(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg);
    void Publish(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg);
    void Subscribe(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg);
    void UnSubscribe(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg);
    void AdvertiseService(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg);
    void CallService(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg);
    void ResponseService(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg);
    void CloseService(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg);
    void QuerySubNum(std::shared_ptr<TCPConnection> conn, std::shared_ptr<TCPMessage> msg);
};

};


#endif /* INCLUDE_PSSC_PROTOCOL_CORE_H_ */
