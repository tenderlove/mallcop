#include <mallcop.h>

VALUE rb_mMallCop;

void mallcop_raise_last_error(MallCopSession *m_session, VALUE klass)
{
  char *errmsg;

  libssh2_session_last_error(m_session->libssh2_session, &errmsg, NULL, 0);

  rb_raise(klass, errmsg);
}

void Init_mallcop()
{
  int rc;

  rb_mMallCop = rb_define_module("MallCop");

  rb_define_const(rb_mMallCop, "ERROR_ALLOC",             INT2FIX(LIBSSH2_ERROR_ALLOC));
  rb_define_const(rb_mMallCop, "ERROR_BANNER_SEND",       INT2FIX(LIBSSH2_ERROR_BANNER_SEND));
  rb_define_const(rb_mMallCop, "ERROR_CHANNEL_FAILURE",   INT2FIX(LIBSSH2_ERROR_CHANNEL_FAILURE));
  rb_define_const(rb_mMallCop, "ERROR_EAGAIN",            INT2FIX(LIBSSH2_ERROR_EAGAIN));
  rb_define_const(rb_mMallCop, "ERROR_KEX_FAILURE",       INT2FIX(LIBSSH2_ERROR_KEX_FAILURE));
  rb_define_const(rb_mMallCop, "ERROR_SOCKET_DISCONNECT", INT2FIX(LIBSSH2_ERROR_SOCKET_DISCONNECT));
  rb_define_const(rb_mMallCop, "ERROR_SOCKET_NONE",       INT2FIX(LIBSSH2_ERROR_SOCKET_NONE));
  rb_define_const(rb_mMallCop, "ERROR_SOCKET_SEND",       INT2FIX(LIBSSH2_ERROR_SOCKET_SEND));
  rb_define_const(rb_mMallCop, "ERROR_PROTO",             INT2FIX(LIBSSH2_ERROR_PROTO));
  rb_define_const(rb_mMallCop, "HASH_SHA1",               INT2FIX(LIBSSH2_HOSTKEY_HASH_SHA1));

  init_mallcop_session();
  init_mallcop_channel();
}
