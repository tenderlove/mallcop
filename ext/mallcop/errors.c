#include <mallcop.h>

VALUE rb_mMallCopErrors;

void init_mallcop_errors()
{
  rb_mMallCopErrors = rb_define_module_under(rb_mMallCop, "Errors");

  rb_define_const(rb_mMallCopErrors, "ERROR_ALLOC",             INT2FIX(LIBSSH2_ERROR_ALLOC));
  rb_define_const(rb_mMallCopErrors, "ERROR_BANNER_SEND",       INT2FIX(LIBSSH2_ERROR_BANNER_SEND));
  rb_define_const(rb_mMallCopErrors, "ERROR_CHANNEL_FAILURE",   INT2FIX(LIBSSH2_ERROR_CHANNEL_FAILURE));
  rb_define_const(rb_mMallCopErrors, "ERROR_EAGAIN",            INT2FIX(LIBSSH2_ERROR_EAGAIN));
  rb_define_const(rb_mMallCopErrors, "ERROR_KEX_FAILURE",       INT2FIX(LIBSSH2_ERROR_KEX_FAILURE));
  rb_define_const(rb_mMallCopErrors, "ERROR_SOCKET_DISCONNECT", INT2FIX(LIBSSH2_ERROR_SOCKET_DISCONNECT));
  rb_define_const(rb_mMallCopErrors, "ERROR_SOCKET_NONE",       INT2FIX(LIBSSH2_ERROR_SOCKET_NONE));
  rb_define_const(rb_mMallCopErrors, "ERROR_SOCKET_SEND",       INT2FIX(LIBSSH2_ERROR_SOCKET_SEND));
  rb_define_const(rb_mMallCopErrors, "ERROR_PROTO",             INT2FIX(LIBSSH2_ERROR_PROTO));
}