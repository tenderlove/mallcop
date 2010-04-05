#ifndef MALLCOP_CHANNEL_H
#define MALLCOP_CHANNEL_H

#include <mallcop.h>

void init_mallcop_channel();

VALUE MallCop_Wrap_Channel(VALUE session, LIBSSH2_CHANNEL * channel);

#endif
