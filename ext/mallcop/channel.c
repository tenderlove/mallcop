#include <mallcop.h>

VALUE rb_cMallCopChannel;

static VALUE dealloc(LIBSSH2_CHANNEL *channel)
{
  // Apparently calling this assplodes :(
  //
  // libssh2_channel_free(channel);
}

VALUE MallCop_Wrap_Channel(VALUE session, LIBSSH2_CHANNEL * channel)
{
  VALUE rb_channel;

  rb_channel = Data_Wrap_Struct(rb_cMallCopChannel, NULL, dealloc, channel);
  rb_iv_set(rb_channel, "@session", session);

  return rb_channel;
}

static VALUE request_pty(VALUE self, VALUE term)
{
  LIBSSH2_CHANNEL *channel;
  int ret;

  Data_Get_Struct(self, LIBSSH2_CHANNEL, channel);

  ret = libssh2_channel_request_pty(channel, StringValuePtr(term));

  if(ret) {
    switch(ret) {
      case LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED:
        return Qfalse;
        break;
      default:
        rb_raise(rb_eRuntimeError, "PTY request failed (%d)", ret);
        break;
    }
  }

  return Qtrue;
}

static VALUE shell(VALUE self)
{
  LIBSSH2_CHANNEL *channel;
  int ret;

  Data_Get_Struct(self, LIBSSH2_CHANNEL, channel);

  ret = libssh2_channel_shell(channel);

  if (ret) {
    switch(ret) {
      case LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED:
        return Qfalse;
        break;
      default:
        rb_raise(rb_eRuntimeError, "Shell request failed (%d)", ret);
        break;
    }
  }

  return Qtrue;
}

void init_mallcop_channel()
{
  rb_cMallCopChannel = rb_define_class_under(rb_mMallCop, "Channel", rb_cObject);

  rb_define_method(rb_cMallCopChannel, "request_pty", request_pty, 1);
  rb_define_method(rb_cMallCopChannel, "shell", shell, 0);
}
