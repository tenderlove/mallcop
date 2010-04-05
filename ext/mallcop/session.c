#include <mallcop.h>

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

static VALUE start(VALUE self)
{
  LIBSSH2_SESSION *session;
  VALUE sock;
  rb_io_t *fptr;

  Data_Get_Struct(self, LIBSSH2_SESSION, session);

  sock = rb_iv_get(self, "@socket");
  GetOpenFile(sock, fptr);

  if(libssh2_session_startup(session, fptr->fd))
    rb_raise(rb_eRuntimeError, "session startup failed");

  return self;
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

void init_mallcop_session()
{
  VALUE klass = rb_define_class_under(rb_mMallCop, "Session", rb_cObject);
  rb_define_alloc_func(klass, allocate);

  rb_define_method(klass, "start", start, 0);
  rb_define_method(klass, "hostkey_hash", hostkey_hash, 1);
  rb_define_method(klass, "userauth_password", userauth_password, 2);
  rb_define_method(klass, "userauth_publickey_fromfile", userauth_publickey_fromfile, 4);
  rb_define_private_method(klass, "userauth_list", userauth_list, 1);
  rb_define_const(klass, "HASH_SHA1", INT2FIX(LIBSSH2_HOSTKEY_HASH_SHA1));
}
