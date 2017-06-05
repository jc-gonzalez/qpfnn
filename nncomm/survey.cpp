#include "survey.h"

//-----------------------------------------------------------------------------
// Survey
//-----------------------------------------------------------------------------
Survey::Survey(int elemCls, std::string addr)
{
    init(elemCls, addr.c_str());
}

Survey::Survey(int elemCls, const char * addr)
{
    init(elemCls, addr);
}

void Survey::setMsgOut(MessageString m)
{
    if (elemClass == NN_RESPONDENT) {
        ScalabilityProtocolRole::setMsgOut(m);
        DBG("++ Sending answer >> " << m);
    } else {
        if (readyToGo) {
            sck->send((void*)(m.c_str()), m.size(), 0);
            DBG("++ Sending msg >> " << m);
            DBG("++ Waiting for answers . . . ");
            int numResp = 1;
            try {
                rc = sck->recv(buf, sizeof(buf), 0);
                while ((rc > 0) && (numResp < maxRespondents)) {
                    DBG("++ Received answer: " << std::string(buf));
                    rc = sck->recv(buf, sizeof(buf), 0);
                    numResp++;
                }
            } catch (nn::exception & e) {
                DBG("---- Only " << numResp << " responses");
            }
        }
    }
}

void Survey::setNumOfRespondents(int r)
{
    maxRespondents = r;
}

void Survey::init(int elemCls, const char * addr)
{
    elemClass = elemCls;
    createSocket(elemClass);
    if (elemClass == NN_SURVEYOR) {
        //int deadline = WAIT_BINDING;
        //sck->setsockopt(NN_SURVEYOR, NN_SURVEYOR_DEADLINE, &deadline, sizeof(int));
        endPoint = sck->bind(addr);
        TRC("BIND >> " << addr);
    } else {
        endPoint = sck->connect(addr);
        TRC("CONNECT >> " << addr);
    }
    address = std::string(addr);
    (void)usleep(WAIT_BINDING);
}

void Survey::getIncommingMessageStrings()
{
    if (elemClass == NN_RESPONDENT) {
        ScalabilityProtocolRole::getIncommingMessageStrings();
    }
}

void Survey::processMessageString(MessageString & m)
{
    if (elemClass == NN_SURVEYOR) {
        DBG("I (" << elemName << ") got one answer: '" << m << "'");
    } else {
        std::string answer("My name is ");
        answer += elemName;
        DBG("Master asks: " << m);
        DBG("I (" << elemName << ") answer: '" << answer << "'");
        setMsgOut(answer);
    }
}
