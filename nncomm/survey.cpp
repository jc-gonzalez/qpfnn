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
        //DBG("++ Sending answer >> " << m);
    } else {
        if (readyToGo) {
            sck->send((void*)(m.c_str()), m.size(), 0);
            surveyorWaiting = true;
            //DBG("++ Sending msg >> " << m);
            //DBG("++ Waiting for answers . . . ");
            /*
            int numResp = 1;
            try {
                rc = sck->recv(buf, sizeof(buf), 0);
                if (rc == ETIMEDOUT) {
                    TRC("ETIMEDOUT\n");
                } else {
                    while ((rc > 0) && (numResp < maxRespondents)) {
                        TRC("++ Received answer: " << std::string(buf));
                        numResp++;
                        rc = sck->recv(buf, sizeof(buf), 0);
                        if (rc == ETIMEDOUT) {
                            TRC("ETIMEDOUT\n");
                        }
                    }
                    }
            } catch (nn::exception & e) {
                TRC("---- Only " << numResp << " responses");
                }*/
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
    surveyorWaiting = false;
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
    if ((elemClass == NN_RESPONDENT) || (surveyorWaiting)) {
        ScalabilityProtocolRole::getIncommingMessageStrings();
        surveyorWaiting = ((iMsgList.size() >= maxRespondents) ||
                           (errno == ETIMEDOUT));
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
