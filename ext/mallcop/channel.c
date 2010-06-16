#include <mallcop.h>

VALUE rb_cMallCopChannel;

static void dealloc(MallCopChannel *m_channel)
{
  libssh2_channel_free( m_channel->libssh2_channel );
  mallcop_session_release( m_channel->m_session );
  free( m_channel );
}

static VALUE allocate(VALUE klass)
{
  MallCopChannel *m_channel = calloc( 1, sizeof(MallCopChannel) );
  return Data_Wrap_Struct(klass, 0, dealloc, m_channel);
}

static VALUE initialize(VALUE self)
{
  MallCopChannel *m_channel;
  MallCopSession *m_session;
  VALUE session;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  if ( m_channel->libssh2_channel ) {
    rb_raise(rb_eRuntimeError, "the libssh2 channel is already initialized");
    return Qnil;
  }

  session = rb_iv_get(self, "@session");

  if ( NIL_P(session) ) {
    rb_raise(rb_eRuntimeError, "@session is nil");
    return Qfalse;
  }

  Data_Get_Struct(session, MallCopSession, m_session);
  m_channel->m_session = m_session;
  m_channel->libssh2_channel = libssh2_channel_open_session(m_session->libssh2_session);

  if ( !m_channel->libssh2_channel ) {
    rb_raise(rb_eRuntimeError, "channel init failed");
    return Qfalse;
  }

  mallcop_session_retain(m_session);

  return Qtrue;
}

static VALUE request_pty(VALUE self, VALUE term)
{
  MallCopChannel *m_channel;
  int ret;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  ret = libssh2_channel_request_pty(m_channel->libssh2_channel, StringValuePtr(term));

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
  MallCopChannel *m_channel;
  int ret;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  ret = libssh2_channel_shell(m_channel->libssh2_channel);

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
  MallCopChannel *m_channel;
  int ret;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  ret = libssh2_channel_exec(m_channel->libssh2_channel, StringValuePtr(command));

  if (ret) {
    rb_raise(rb_eRuntimeError, "Channel exec failed (%d)", ret);
  }

  return Qtrue;
}

static VALUE read(VALUE self)
{
  MallCopChannel *m_channel;
  char buffer[0x4000];
  int count;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  count = libssh2_channel_read(m_channel->libssh2_channel, buffer, sizeof(buffer));

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
  MallCopChannel *m_channel;
  ssize_t ret;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  libssh2_channel_write(m_channel->libssh2_channel, StringValuePtr(string), RSTRING_LEN(string));

  if (ret < 0) {
    rb_raise(rb_eRuntimeError, "Write error (%d)", ret);
  }

  return Qtrue;
}

static VALUE eof(VALUE self)
{
  MallCopChannel *m_channel;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  return libssh2_channel_eof(m_channel->libssh2_channel) ? Qtrue : Qfalse;
}

static VALUE send_eof(VALUE self)
{
  MallCopChannel *m_channel;
  int ret;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  ret = libssh2_channel_send_eof(m_channel->libssh2_channel);

  return Qtrue;
}

void init_mallcop_channel()
{
  rb_cMallCopChannel = rb_define_class_under(rb_mMallCop, "Channel", rb_cObject);
  rb_define_alloc_func(rb_cMallCopChannel, allocate);

  rb_define_private_method(rb_cMallCopChannel, "native_eof?", eof, 0);
  rb_define_private_method(rb_cMallCopChannel, "native_initialize", initialize, 0);

  rb_define_method(rb_cMallCopChannel, "request_pty", request_pty, 1);
  rb_define_method(rb_cMallCopChannel, "shell", shell, 0);
  rb_define_method(rb_cMallCopChannel, "channel_exec", channel_exec, 1);
  rb_define_method(rb_cMallCopChannel, "read", read, 0);
  rb_define_method(rb_cMallCopChannel, "write", write, 1);
  rb_define_method(rb_cMallCopChannel, "send_eof", send_eof, 0);
}
