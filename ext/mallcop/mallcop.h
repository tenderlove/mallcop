#ifndef MALLCOP_H
#define MALLCOP_H

#include <ruby.h>
#include <ruby/io.h>
#include <libssh2.h>

extern VALUE rb_mMallCop;
extern VALUE rb_cMallCopSession;
extern VALUE rb_cMallCopChannel;

void init_mallcop_channel();
void init_mallcop_session();

VALUE MallCop_Wrap_Channel(VALUE session, LIBSSH2_CHANNEL * channel);

#endif
