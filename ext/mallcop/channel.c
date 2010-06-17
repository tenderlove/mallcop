#include <mallcop.h>

VALUE rb_cMallCopChannel;

static void dealloc(MallCopChannel *m_channel)
{
  BLOCK(libssh2_channel_free( m_channel->libssh2_channel ));
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

  m_channel->libssh2_channel = libssh2_channel_open_session(m_session->libssh2_session);

  if ( !m_channel->libssh2_channel ) {
    if ( libssh2_session_last_errno(m_session->libssh2_session) == LIBSSH2_ERROR_EAGAIN ) {
      return INT2FIX(LIBSSH2_ERROR_EAGAIN);
    }

    mallcop_raise_last_error(m_session, rb_eRuntimeError);
    return Qnil;
  }

  // Initializing the libssh2 channel is successful, so we can associate
  // the session object with the channel object.
  m_channel->m_session = m_session;
  mallcop_session_retain(m_session);

  return Qtrue;
}

static VALUE request_pty(VALUE self, VALUE term)
{
  MallCopChannel *m_channel;
  int ret;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  ret = libssh2_channel_request_pty(m_channel->libssh2_channel, StringValuePtr(term));

  if ( ret == 0 || ret == LIBSSH2_ERROR_EAGAIN ) {
    return INT2FIX(ret);
  }

  mallcop_raise_last_error(m_channel->m_session, rb_eRuntimeError);
  return Qnil;
}

static VALUE shell(VALUE self)
{
  MallCopChannel *m_channel;
  int ret;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  ret = libssh2_channel_shell(m_channel->libssh2_channel);

  if ( ret == 0 || ret == LIBSSH2_ERROR_EAGAIN ) {
    return INT2FIX(ret);
  }

  mallcop_raise_last_error(m_channel->m_session, rb_eRuntimeError);
  return Qnil;
}

static VALUE channel_exec(VALUE self, VALUE command)
{
  MallCopChannel *m_channel;
  int ret;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  ret = libssh2_channel_exec(m_channel->libssh2_channel, StringValuePtr(command));

  if ( ret == 0 || ret == LIBSSH2_ERROR_EAGAIN ) {
    return INT2FIX(ret);
  }

  mallcop_raise_last_error(m_channel->m_session, rb_eRuntimeError);
  return Qnil;
}

static VALUE channel_read(VALUE self)
{
  MallCopChannel *m_channel;
  char buffer[0x4000];
  int count;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  count = libssh2_channel_read(m_channel->libssh2_channel, buffer, sizeof(buffer));

  if ( count >= 0 ) {
    return rb_str_new((const char *) &buffer, count);
  } else if ( count == LIBSSH2_ERROR_EAGAIN ) {
    return INT2FIX(LIBSSH2_ERROR_EAGAIN);
  }

  mallcop_raise_last_error(m_channel->m_session, rb_eRuntimeError);
  return Qnil;
}

static VALUE channel_write(VALUE self, VALUE string)
{
  MallCopChannel *m_channel;
  ssize_t ret;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  ret = libssh2_channel_write(m_channel->libssh2_channel, StringValuePtr(string), RSTRING_LEN(string));

  if ( ret >= 0 || ret == LIBSSH2_ERROR_EAGAIN ) {
    return INT2FIX(ret);
  }

  mallcop_raise_last_error(m_channel->m_session, rb_eRuntimeError);
  return Qnil;
}

static VALUE channel_close(VALUE self)
{
  MallCopChannel *m_channel;
  int ret;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  ret = libssh2_channel_close(m_channel->libssh2_channel);

  if ( ret >= 0 || ret == LIBSSH2_ERROR_EAGAIN ) {
    return INT2FIX(ret);
  }

  mallcop_raise_last_error(m_channel->m_session, rb_eRuntimeError);
  return Qnil;
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

  if ( ret == 0 || ret == LIBSSH2_ERROR_EAGAIN ) {
    return INT2FIX(ret);
  }

  mallcop_raise_last_error(m_channel->m_session, rb_eRuntimeError);
  return Qnil;
}

static VALUE flush(VALUE self)
{
  MallCopChannel *m_channel;
  int ret;

  Data_Get_Struct(self, MallCopChannel, m_channel);

  ret = libssh2_channel_flush(m_channel->libssh2_channel);

  if ( ret == 0 || ret == LIBSSH2_ERROR_EAGAIN ) {
    return INT2FIX(ret);
  }

  mallcop_raise_last_error(m_channel->m_session, rb_eRuntimeError);
  return Qnil;
}

void init_mallcop_channel()
{
  rb_cMallCopChannel = rb_define_class_under(rb_mMallCop, "Channel", rb_cObject);
  rb_define_alloc_func(rb_cMallCopChannel, allocate);

  rb_define_private_method(rb_cMallCopChannel, "native_eof?", eof, 0);
  rb_define_private_method(rb_cMallCopChannel, "native_initialize", initialize, 0);
  rb_define_private_method(rb_cMallCopChannel, "native_channel_close", channel_close, 0);
  rb_define_private_method(rb_cMallCopChannel, "native_channel_read", channel_read, 0);
  rb_define_private_method(rb_cMallCopChannel, "native_channel_write", channel_write, 1);
  rb_define_private_method(rb_cMallCopChannel, "native_request_pty", request_pty, 1);
  rb_define_private_method(rb_cMallCopChannel, "native_shell", shell, 0);
  rb_define_private_method(rb_cMallCopChannel, "native_channel_exec", channel_exec, 1);
  rb_define_private_method(rb_cMallCopChannel, "native_send_eof", send_eof, 0);
  rb_define_private_method(rb_cMallCopChannel, "native_flush", flush, 0);
}
