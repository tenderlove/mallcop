#include <mallcop.h>

VALUE rb_cMallCopChannel;

static VALUE dealloc(LIBSSH2_CHANNEL *channel)
{
  // Apparently calling this assplodes :(
  //
  // libssh2_channel_free(channel);
}

VALUE allocate_channel_with_session(VALUE rb_session)
{
  VALUE rb_channel;
  LIBSSH2_CHANNEL *channel;
  LIBSSH2_SESSION *session;

  Data_Get_Struct(rb_session, LIBSSH2_SESSION, session);

  channel = libssh2_channel_open_session(session);

  if (!channel) rb_raise(rb_eRuntimeError, "channel init failed");

  rb_channel = Data_Wrap_Struct(rb_cMallCopChannel, NULL, dealloc, channel);

  rb_iv_set(rb_channel, "@session", rb_session);

  return rb_channel;
}

void init_mallcop_channel()
{
  rb_cMallCopChannel = rb_define_class_under(rb_mMallCop, "Channel", rb_cObject);
}