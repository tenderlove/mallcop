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

static VALUE channel_exec(VALUE self, VALUE command)
{
  LIBSSH2_CHANNEL *channel;
  int ret;

  Data_Get_Struct(self, LIBSSH2_CHANNEL, channel);

  ret = libssh2_channel_exec(channel, StringValuePtr(command));

  if (ret) {
    rb_raise(rb_eRuntimeError, "Channel exec failed (%d)", ret);
  }

  return Qtrue;
}

static VALUE read(VALUE self)
{
  LIBSSH2_CHANNEL *channel;
  char buffer[0x4000];
  int count;

  Data_Get_Struct(self, LIBSSH2_CHANNEL, channel);

  count = libssh2_channel_read(channel, buffer, sizeof(buffer));

  if (count > 0) {
    return rb_str_new(&buffer, count);
  } else {
    rb_raise(rb_eRuntimeError, "Read failed with (%d)", count);
    return Qnil;
  }

  return Qnil;
}

static VALUE write(VALUE self, VALUE string)
{
  LIBSSH2_CHANNEL *channel;
  ssize_t ret;

  Data_Get_Struct(self, LIBSSH2_CHANNEL, channel);

  libssh2_channel_write(channel, StringValuePtr(string), RSTRING_LEN(string));

  if (ret < 0) {
    rb_raise(rb_eRuntimeError, "Write error (%d)", ret);
  }

  return Qtrue;
}

static VALUE send_eof(VALUE self)
{
  LIBSSH2_CHANNEL *channel;
  int ret;

  Data_Get_Struct(self, LIBSSH2_CHANNEL, channel);

  ret = libssh2_channel_send_eof(channel);

  return Qtrue;
}

void init_mallcop_channel()
{
  rb_cMallCopChannel = rb_define_class_under(rb_mMallCop, "Channel", rb_cObject);

  rb_define_method(rb_cMallCopChannel, "request_pty", request_pty, 1);
  rb_define_method(rb_cMallCopChannel, "shell", shell, 0);
  rb_define_method(rb_cMallCopChannel, "channel_exec", channel_exec, 1);
  rb_define_method(rb_cMallCopChannel, "read", read, 0);
  rb_define_method(rb_cMallCopChannel, "write", write, 1);
  rb_define_method(rb_cMallCopChannel, "send_eof", send_eof, 0);
}
