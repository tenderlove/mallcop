#include <mallcop.h>

VALUE rb_cMallCopSession;

void mallcop_session_retain(MallCopSession *m_session)
{
  ++m_session->count;
  // Nothing yet
}

void mallcop_session_release(MallCopSession *m_session)
{
  if ( --m_session->count == 0 ) {
    libssh2_session_free( m_session->libssh2_session );
    free( m_session );
  }
}

static void dealloc(MallCopSession *m_session)
{
  mallcop_session_release( m_session );
}

static VALUE allocate(VALUE klass)
{
  MallCopSession *m_session = calloc( 1, sizeof(MallCopSession) );
  mallcop_session_retain(m_session);

  return Data_Wrap_Struct(klass, 0, dealloc, m_session);
}

static VALUE initialize(VALUE self)
{
  MallCopSession *m_session;

  Data_Get_Struct(self, MallCopSession, m_session);

  if ( m_session->libssh2_session ) {
    rb_raise(rb_eRuntimeError, "the libssh2 session is already initialized");
    return Qnil;
  }

  m_session->libssh2_session = libssh2_session_init();

  if ( !m_session->libssh2_session ) {
    rb_raise(rb_eRuntimeError, "session init failed");
    return Qnil;
  }

  return Qtrue;
}

static VALUE start(VALUE self, VALUE fd)
{
  MallCopSession *m_session;

  Data_Get_Struct(self, MallCopSession, m_session);

  return INT2FIX(libssh2_session_startup(m_session->libssh2_session, NUM2INT(fd)));
}

static VALUE hostkey_hash(VALUE self, VALUE hashtype)
{
  MallCopSession *m_session;
  const char *key;
  int keytype = NUM2INT(hashtype);
  Data_Get_Struct(self, MallCopSession, m_session);

  key = libssh2_hostkey_hash(m_session->libssh2_session, keytype);
  if(!key) return Qnil;
  if(keytype == LIBSSH2_HOSTKEY_HASH_SHA1)
    return rb_str_new(key, 20);

  return rb_str_new(key, 16);
}

static VALUE userauth_list(VALUE self, VALUE user)
{
  MallCopSession *m_session;
  char * list;

  Data_Get_Struct(self, MallCopSession, m_session);

  list = libssh2_userauth_list(m_session->libssh2_session,
          StringValuePtr(user), RSTRING_LEN(user));

  return rb_str_new2(list);
}

static VALUE userauth_password(VALUE self, VALUE user, VALUE password)
{
  MallCopSession *m_session;
  int ret;

  Data_Get_Struct(self, MallCopSession, m_session);

  ret = libssh2_userauth_password(m_session->libssh2_session,
          StringValuePtr(user), StringValuePtr(password));

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
  MallCopSession *m_session;
  int ret;

  Data_Get_Struct(self, MallCopSession, m_session);

  ret = libssh2_userauth_publickey_fromfile(m_session->libssh2_session,
          StringValuePtr(user), StringValuePtr(public_key),
          StringValuePtr(private_key), StringValuePtr(password));

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

static VALUE last_errno(VALUE self)
{
  MallCopSession *m_session;

  Data_Get_Struct(self, MallCopSession, m_session);

  return INT2FIX(libssh2_session_last_errno(m_session->libssh2_session));
}

static VALUE last_errmsg(VALUE self)
{
  MallCopSession *m_session;
  char *errmsg;
  int errlen;

  Data_Get_Struct(self, MallCopSession, m_session);

  libssh2_session_last_error(m_session->libssh2_session, &errmsg, &errlen, 0);

  return rb_str_new(errmsg, (long) errlen);
}

void init_mallcop_session()
{
  rb_cMallCopSession = rb_define_class_under(rb_mMallCop, "Session", rb_cObject);
  rb_define_alloc_func(rb_cMallCopSession, allocate);

  rb_define_private_method(rb_cMallCopSession, "native_initialize", initialize, 0);
  rb_define_private_method(rb_cMallCopSession, "native_start", start, 1);
  rb_define_private_method(rb_cMallCopSession, "native_userauth_list", userauth_list, 1);
  rb_define_private_method(rb_cMallCopSession, "native_last_errno", last_errno, 0);
  rb_define_private_method(rb_cMallCopSession, "native_last_errmsg", last_errmsg, 0);
  rb_define_private_method(rb_cMallCopSession, "native_userauth_password", userauth_password, 2);

  rb_define_method(rb_cMallCopSession, "hostkey_hash", hostkey_hash, 1);
  rb_define_method(rb_cMallCopSession, "userauth_publickey_fromfile", userauth_publickey_fromfile, 4);
  rb_define_const(rb_cMallCopSession, "HASH_SHA1", INT2FIX(LIBSSH2_HOSTKEY_HASH_SHA1));
}
