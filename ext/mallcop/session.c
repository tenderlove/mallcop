#include <mallcop.h>

VALUE rb_cMallCopSession;

static VALUE dealloc(LIBSSH2_SESSION *session)
{
  libssh2_session_free(session);
}

static VALUE allocate(VALUE klass)
{
  LIBSSH2_SESSION *session;

  session = libssh2_session_init();
  if(!session) rb_raise(rb_eRuntimeError, "session init failed");

  return Data_Wrap_Struct(klass, 0, dealloc, session);
}

static VALUE start(VALUE self, VALUE sock)
{
  LIBSSH2_SESSION *session;
  rb_io_t *fptr;

  Data_Get_Struct(self, LIBSSH2_SESSION, session);

  GetOpenFile(sock, fptr);

  return INT2FIX(libssh2_session_startup(session, fptr->fd));
}

static VALUE hostkey_hash(VALUE self, VALUE hashtype)
{
  LIBSSH2_SESSION *session;
  const char *key;
  int keytype = NUM2INT(hashtype);
  Data_Get_Struct(self, LIBSSH2_SESSION, session);

  key = libssh2_hostkey_hash(session, keytype);
  if(!key) return Qnil;
  if(keytype == LIBSSH2_HOSTKEY_HASH_SHA1)
    return rb_str_new(key, 20);

  return rb_str_new(key, 16);
}

static VALUE userauth_list(VALUE self, VALUE user)
{
  LIBSSH2_SESSION *session;
  char * list;

  Data_Get_Struct(self, LIBSSH2_SESSION, session);

  list = libssh2_userauth_list(session, StringValuePtr(user), RSTRING_LEN(user));
  return rb_str_new2(list);
}

static VALUE userauth_password(VALUE self, VALUE user, VALUE password)
{
  LIBSSH2_SESSION *session;
  int ret;

  Data_Get_Struct(self, LIBSSH2_SESSION, session);
  ret = libssh2_userauth_password(session, StringValuePtr(user),
      StringValuePtr(password));

  if(ret) {
    switch(ret) {
      case LIBSSH2_ERROR_AUTHENTICATION_FAILED:
      case LIBSSH2_ERROR_SOCKET_NONE:
        return Qfalse;
        break;
      default:
        rb_raise(rb_eRuntimeError, "authentication failed (%d)", ret);
        break;
    }
  }

  return Qtrue;
}

static VALUE userauth_publickey_fromfile(VALUE self, VALUE user, VALUE public_key, VALUE private_key, VALUE password)
{
  LIBSSH2_SESSION *session;
  int ret;

  Data_Get_Struct(self, LIBSSH2_SESSION, session);
  ret = libssh2_userauth_publickey_fromfile(session, StringValuePtr(user),
          StringValuePtr(public_key), StringValuePtr(private_key),
          StringValuePtr(password));

  if(ret) {
    switch(ret) {
      case LIBSSH2_ERROR_AUTHENTICATION_FAILED:
      case LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
        return Qfalse;
        break;
      default:
        rb_raise(rb_eRuntimeError, "authentication failed (%d)", ret);
        break;
    }
  }

  return Qtrue;
}

static VALUE open_channel(VALUE self)
{
  LIBSSH2_SESSION *session;
  LIBSSH2_CHANNEL *channel;

  Data_Get_Struct(self, LIBSSH2_SESSION, session);

  channel = libssh2_channel_open_session(session);

  if (channel) {
    return MallCop_Wrap_Channel(self, channel);
  } else {
    return Qnil;
  }
}

static VALUE last_errno(VALUE self)
{
  LIBSSH2_SESSION *session;

  Data_Get_Struct(self, LIBSSH2_SESSION, session);

  return INT2FIX(libssh2_session_last_errno(session));
}

static VALUE last_error(VALUE self)
{
  LIBSSH2_SESSION *session;
  char *errmsg;

  Data_Get_Struct(self, LIBSSH2_SESSION, session);

  libssh2_session_last_error(session, &errmsg, 0, 0);

  return Qnil;
}

void init_mallcop_session()
{
  rb_cMallCopSession = rb_define_class_under(rb_mMallCop, "Session", rb_cObject);
  rb_define_alloc_func(rb_cMallCopSession, allocate);

  rb_define_private_method(rb_cMallCopSession, "native_start", start, 1);
  rb_define_private_method(rb_cMallCopSession, "native_open_channel", open_channel, 0);
  rb_define_private_method(rb_cMallCopSession, "native_userauth_list", userauth_list, 1);
  rb_define_private_method(rb_cMallCopSession, "native_last_errno", last_errno, 0);

  rb_define_method(rb_cMallCopSession, "hostkey_hash", hostkey_hash, 1);
  rb_define_method(rb_cMallCopSession, "userauth_password", userauth_password, 2);
  rb_define_method(rb_cMallCopSession, "userauth_publickey_fromfile", userauth_publickey_fromfile, 4);
  rb_define_const(rb_cMallCopSession, "HASH_SHA1", INT2FIX(LIBSSH2_HOSTKEY_HASH_SHA1));
}
