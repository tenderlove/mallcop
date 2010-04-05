#ifndef MALLCOP_SESSION_H
#define MALLCOP_SESSION_H

#include <mallcop.h>

void init_mallcop_channel();

VALUE allocate_channel_with_session(LIBSSH2_SESSION * session);

#endif
