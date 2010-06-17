#ifndef MALLCOP_H
#define MALLCOP_H

#include <ruby.h>
#include <libssh2.h>

#define BLOCK(stmt) while ((stmt) == LIBSSH2_ERROR_EAGAIN)

extern VALUE rb_mMallCop;
extern VALUE rb_cMallCopSession;
extern VALUE rb_cMallCopChannel;

typedef struct {
  LIBSSH2_SESSION *libssh2_session;
  int count;
} MallCopSession;

typedef struct {
  LIBSSH2_CHANNEL *libssh2_channel;
  MallCopSession *m_session;
} MallCopChannel;

void init_mallcop_channel();
void init_mallcop_session();

void mallcop_session_retain(MallCopSession *session);
void mallcop_session_release(MallCopSession *session);

void mallcop_raise_last_error(MallCopSession *m_session, VALUE klass);

#endif
