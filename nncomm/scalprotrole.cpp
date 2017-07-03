#include "scalprotrole.h"
#include <cassert>
#include <sstream>

#include <nanomsg/survey.h>

ScalabilityProtocolRole::ScalabilityProtocolRole()
    : sck(0), readyToGo(false), incMsgsMask(NN_IN | NN_OUT)
{
}

ScalabilityProtocolRole::~ScalabilityProtocolRole()
{
    if (sck != 0) sck->shutdown(endPoint);
}

void ScalabilityProtocolRole::createSocket(int protocol, int domain)
{
    sck = new Socket {domain, protocol};
    assert(sck != 0);
    assert(sck->fd() >= 0);
    TRC(elemName << ": " << sck);
    readyToGo = true;
}

void ScalabilityProtocolRole::update()
{
    if (!readyToGo) { return; }

    // Get incomming messages, if any
    getIncommingMessageStrings();
}

bool ScalabilityProtocolRole::next(MessageString & m)
{
    std::unique_lock<std::mutex> ulck(mtxMsgLists);
    bool thereAreMessages = !iMsgList.empty();
    if (thereAreMessages) {
        m = iMsgList.front();
        iMsgList.pop();
    }
    return thereAreMessages;
}

void ScalabilityProtocolRole::setMsgOut(MessageString m)
{
    if (readyToGo) {
        sck->send((void*)(m.c_str()), m.size(), 0);
        //TRC("++ Sending msg. " << m);
    }
}

int ScalabilityProtocolRole::getClass()
{
    return elemClass;
}

std::string ScalabilityProtocolRole::getClassName()
{
    std::stringstream ss;
    ss << elemClass;
    return ss.str();
}

std::string ScalabilityProtocolRole::getName()
{
    return elemName;
}

void ScalabilityProtocolRole::setName(std::string & name)
{
    elemName = name;
}

std::string ScalabilityProtocolRole::getAddress()
{
    return address;
}

void ScalabilityProtocolRole::getIncommingMessageStrings()
{
    if (elemClass == NN_SURVEYOR) {
        while (true) {
            try {
                rc = sck->recv(buf, MAX_MESSAGE_SIZE, 0);
                if (rc == ETIMEDOUT) { return; }
                if (rc > 0) {
                    std::unique_lock<std::mutex> ulck(mtxMsgLists);
                    memset((void*)(buf + rc), 0, MAX_MESSAGE_SIZE - rc);
                    iMsgList.push(MessageString(buf));
                }
            } catch (...) {
                return;
            }
        }
    } else {
        int rev = getevents(sck->fd(), incMsgsMask, 50);
        //if (rev & NN_IN == NN_IN) {
        if (rev > 0) {
            try {
                rc = sck->recv(buf, MAX_MESSAGE_SIZE, 0);
            } catch (...) {
                return;
            }
            if (rc > 0) {
                std::unique_lock<std::mutex> ulck(mtxMsgLists);
                memset((void*)(buf + rc), 0, MAX_MESSAGE_SIZE - rc);
                iMsgList.push(MessageString(buf));
            }
        }
    }
}

int ScalabilityProtocolRole::getevents(int s, int events, int timeout)
{
    int rc;
    fd_set pollset;
#if defined NN_HAVE_WINDOWS
    SOCKET rcvfd;
    SOCKET sndfd;
#else
    int rcvfd;
    int sndfd;
    int maxfd;
#endif
    size_t fdsz;
    struct timeval tv;
    int revents;

#if !defined NN_HAVE_WINDOWS
    maxfd = 0;
#endif
    FD_ZERO(&pollset);

    if (events & NN_IN) {
        fdsz = sizeof(rcvfd);
        rc = nn_getsockopt(s, NN_SOL_SOCKET, NN_RCVFD, (char*) &rcvfd, &fdsz);
        errno_assert(rc == 0);
        nn_assert(fdsz == sizeof(rcvfd));
        FD_SET(rcvfd, &pollset);
#if !defined NN_HAVE_WINDOWS
        if (rcvfd + 1 > maxfd) { maxfd = rcvfd + 1; }
#endif
    }

    if (events & NN_OUT) {
        fdsz = sizeof(sndfd);
        rc = nn_getsockopt(s, NN_SOL_SOCKET, NN_SNDFD, (char*) &sndfd, &fdsz);
        errno_assert(rc == 0);
        nn_assert(fdsz == sizeof(sndfd));
        FD_SET(sndfd, &pollset);
#if !defined NN_HAVE_WINDOWS
        if (sndfd + 1 > maxfd) { maxfd = sndfd + 1; }
#endif
    }

    if (timeout >= 0) {
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
    }
#if defined NN_HAVE_WINDOWS
    rc = select(0, &pollset, 0, 0, timeout < 0 ? 0 : &tv);
    wsa_assert(rc != SOCKET_ERROR);
#else
    rc = select(maxfd, &pollset, 0, 0, timeout < 0 ? 0 : &tv);
    errno_assert(rc >= 0);
#endif
    revents = 0;

    if ((events & NN_IN) && FD_ISSET(rcvfd, &pollset))  { revents |= NN_IN; }
    if ((events & NN_OUT) && FD_ISSET(sndfd, &pollset)) { revents |= NN_OUT; }

    return revents;
}
