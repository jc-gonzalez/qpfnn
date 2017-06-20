// -*- C++ -*-

#ifndef CHANNELS_H
#define CHANNELS_H

#include "nncomm.h"

#undef T

#define TLISTOF_TX_IDS                          \
        T(CMD),                                 \
        T(HMICMD),                              \
        T(INDATA),                              \
        T(TSKSCHED),                            \
        T(TSKPROC),                             \
        T(TSKRQST),                             \
        T(TSKREP),                              \
        T(HOSTMON),                             \
        T(TSKREPDIST)

#define T(x) TX_ID_ ## x
enum TxId { TLISTOF_TX_IDS };
#undef T

#define T(x) std::string( #x )
const ChannelDescriptor ChannelAcronym[] = { TLISTOF_TX_IDS };
#undef T

const ChannelDescriptor ChnlCmd        (ChannelAcronym[TX_ID_CMD]);
const ChannelDescriptor ChnlHMICmd     (ChannelAcronym[TX_ID_HMICMD]);
const ChannelDescriptor ChnlInData     (ChannelAcronym[TX_ID_INDATA]);
const ChannelDescriptor ChnlTskSched   (ChannelAcronym[TX_ID_TSKSCHED]);
const ChannelDescriptor ChnlTskProc    (ChannelAcronym[TX_ID_TSKPROC]);
//const ChannelDescriptor ChnlTskRqst    (ChannelAcronym[TX_ID_TSKRQST]);
//const ChannelDescriptor ChnlTskRep     (ChannelAcronym[TX_ID_TSKREP]);
const ChannelDescriptor ChnlTskRepDist (ChannelAcronym[TX_ID_TSKREPDIST]);

#endif
