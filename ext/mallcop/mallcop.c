#include <mallcop.h>

VALUE rb_mMallCop;

void Init_mallcop()
{
  int rc;

  rb_mMallCop = rb_define_module("MallCop");

  init_mallcop_errors();
  init_mallcop_session();
  init_mallcop_channel();
}
