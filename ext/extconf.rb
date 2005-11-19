require 'mkmf'

dir_config("shout")
have_library("ogg", "oggpack_writeinit")
have_library("vorbis", "vorbis_dsp_clear")
have_library("pthread", "pthread_create")
if find_library("shout", "shout_init","/usr","/usr/local") and have_header("shout/shout.h")
    create_makefile("shout")
else
    print "*** ERROR: need to have libshout and shout/shout.h to compile this module\n"
end
