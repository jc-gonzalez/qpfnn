// -*- C++ -*-
#include <iostream>
#include <fstream>
#include <sstream>

#include "survey.h"

#include "message.h"
#include "channels.h"

///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[])
{
    Dbg::verbosityLevel = Dbg::TRACE_LEVEL;

    ScalabilityProtocolRole * masterConn;
    std::vector<ScalabilityProtocolRole *> clientConn;

    const int NumOfClients = 5;
    const int PortNum = 12345;

    // Agent names, hosts and ports (port range starts with startingPort)
    char sAgName[40];
    std::vector<std::string> agName;

    std::vector<bool> msgRecv;

    MessageString m;

    // Connection addresses and channel
    std::string bindAddr;
    std::string connAddr;

    //======================================================================
    // 1. Gather host information
    //======================================================================

    std::string masterAddress("127.0.0.1");
    std::cerr << "MasterAddress is " << masterAddress << "\n";

    //======================================================================
    // 2. Create the elements and connections for the host we are running on
    //======================================================================
    for (int i = 0; i < NumOfClients; ++i) {
        sprintf(sAgName, "Client_%d", i + 1);
        agName.push_back(std::string(sAgName));

    }

    // Create survey connections
    bindAddr = "tcp://" + masterAddress + ":" + std::to_string(PortNum);
    bindAddr = "ipc:///tmp/survey.ipc";
    connAddr = bindAddr;

    masterConn = new Survey(NN_SURVEYOR, bindAddr);
    for (int i = 0; i < NumOfClients; ++i) {
        clientConn.push_back(new Survey(NN_RESPONDENT, connAddr));
        msgRecv.push_back(false);
    }
    ((Survey*)(masterConn))->setNumOfRespondents(NumOfClients);

    //======================================================================
    // 3. Test communications
    //======================================================================

    for (int iter = 0; iter < 10; ++iter) {

        //==== Send PING message
        std::cerr << "\n\n\nMaster sends a 'PING'\n";
        masterConn->setMsgOut("PING");

        //==== Receive messages
        for (int i = 0; i < NumOfClients; ++i) {
            clientConn.at(i)->update();
            msgRecv[i] = false;
            while (clientConn.at(i)->next(m)) {
                std::cerr << agName.at(i) << " received a '" << m << "'\n";
                msgRecv[i] = true;
            }
        }

        //==== Send answers
        for (int i = 0; i < NumOfClients; ++i) {
            if (msgRecv.at(i)) {
                std::cerr << agName.at(i) << " sends answer . . .\n";
                clientConn.at(i)->setMsgOut(agName.at(i) + "|PONG");
                clientConn.at(i)->update();
            }
        }

        //==== Receive PONG messages
        masterConn->update();
        while (masterConn->next(m)) {
            std::cerr << "Master received a '" << m << "'\n";
            masterConn->update();
        }

        sleep(3);

    }

    return 0;
}
