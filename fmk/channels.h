// -*- C++ -*-

#ifndef CHANNELS_H
#define CHANNELS_H

#include "nncomm.h"

#undef T

#define TLISTOF_CHANNELS  T(CMD),               \
        T(INDATA),                              \
        T(TSKSCHED),                            \
        T(TSKPROC),                             \
        T(TSKRQST),                             \
        T(TSKREP),                              \
        T(TSKREPDIST)

#define T(x) CHNL_ ## x
enum ChannelId { TLISTOF_CHANNELS };
#undef T

#define T(x) std::string( #x )
const ChannelDescriptor ChannelAcronym[] = { TLISTOF_CHANNELS };
#undef T

const ChannelDescriptor ChnlCmd        (ChannelAcronym[CHNL_CMD]);
const ChannelDescriptor ChnlInData     (ChannelAcronym[CHNL_INDATA]);
const ChannelDescriptor ChnlTskSched   (ChannelAcronym[CHNL_TSKSCHED]);
const ChannelDescriptor ChnlTskProc    (ChannelAcronym[CHNL_TSKPROC]);
const ChannelDescriptor ChnlTskRqst    (ChannelAcronym[CHNL_TSKRQST]);
const ChannelDescriptor ChnlTskRep     (ChannelAcronym[CHNL_TSKREP]);
const ChannelDescriptor ChnlTskRepDist (ChannelAcronym[CHNL_TSKREPDIST]);

#endif
