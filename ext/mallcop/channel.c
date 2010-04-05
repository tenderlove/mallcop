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

void init_mallcop_channel()
{
  rb_cMallCopChannel = rb_define_class_under(rb_mMallCop, "Channel", rb_cObject);
}
