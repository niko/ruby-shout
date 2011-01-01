require 'mkmf'

case 
when RUBY_VERSION >= '1.9' then $CFLAGS += ' -DRUBY_1_9_x'
when RUBY_VERSION <  '1.9' then $CFLAGS += ' -DRUBY_1_8_x'
end

if have_func('rb_thread_blocking_region')
  $CFLAGS += " -DHAVE_RB_THREAD_BLOCKING_REGION"
end

dir_config("shout")
have_library("ogg", "oggpack_writeinit")
have_library("vorbis", "vorbis_dsp_clear")
have_library("pthread", "pthread_create")
if find_library("shout", "shout_init","/usr","/usr/local") and have_header("shout/shout.h")
  create_makefile("shout_ext")
else
  print "*** ERROR: need to have libshout and shout/shout.h to compile this module\n"
end
