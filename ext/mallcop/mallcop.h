#ifndef MALLCOP_H
#define MALLCOP_H

#include <ruby.h>
#include <ruby/io.h>
#include <libssh2.h>

extern VALUE rb_mMallCop;
extern VALUE rb_mMallCopErrors;
extern VALUE rb_cMallCopSession;
extern VALUE rb_cMallCopChannel;

#include <session.h>
#include <channel.h>

#endif
