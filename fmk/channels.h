// -*- C++ -*-

#ifndef CHANNELS_H
#define CHANNELS_H

#include "nncomm.h"

#undef T

#define TLISTOF_CHANNELS  T(CMD),               \
        T(INDATA),                              \
        T(MONIT),                               \
        T(TSKSCHED),                            \
        T(TSKPROC),                             \
        T(TSKMONIT),                            \
        T(TSKREP)

#define T(x) CHNL_ ## x
enum ChannelId { TLISTOF_CHANNELS };
#undef T

#define T(x) std::string( #x )
const ChannelDescriptor ChannelAcronym[] = { TLISTOF_CHANNELS };
#undef T

const ChannelDescriptor ChnlCmd      (ChannelAcronym[CHNL_CMD]);
const ChannelDescriptor ChnlInData   (ChannelAcronym[CHNL_INDATA]);
const ChannelDescriptor ChnlMonit    (ChannelAcronym[CHNL_MONIT]);
const ChannelDescriptor ChnlTskSched (ChannelAcronym[CHNL_TSKSCHED]);
const ChannelDescriptor ChnlTskProc  (ChannelAcronym[CHNL_TSKPROC]);
const ChannelDescriptor ChnlTskMonit (ChannelAcronym[CHNL_TSKMONIT]);
const ChannelDescriptor ChnlTskRep   (ChannelAcronym[CHNL_TSKREP]);

#endif
