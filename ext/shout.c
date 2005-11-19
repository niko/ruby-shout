/*
   shout.c - libshout extension for ruby
   jared jennings
   originally written 29 aug 2001
   redone for libshout2 18 jul 2003

   Copyright (c) 2003-2005, Jared Jennings
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of Jared Jennings nor the names of contributors may
      be used to endorse or promote products derived from this software without
      specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
 */

#include "ruby.h"
#include <shout/shout.h>
#include <stdio.h>

#define DEFAULT_MOUNTPOINT "/default"

/*
----------------- ShoutError --------------------
 */
static VALUE cShoutError;

static void Init_shout_error() {
        cShoutError = rb_define_class("ShoutError", rb_eRuntimeError);

        rb_define_const(cShoutError, "SUCCESS",
                        INT2NUM(SHOUTERR_SUCCESS));
        rb_define_const(cShoutError, "INSANE",
                        INT2NUM(SHOUTERR_INSANE));
        rb_define_const(cShoutError, "CONNECTED",
                        INT2NUM(SHOUTERR_CONNECTED));
        rb_define_const(cShoutError, "UNCONNECTED",
                        INT2NUM(SHOUTERR_UNCONNECTED));
        rb_define_const(cShoutError, "NOCONNECT",
                        INT2NUM(SHOUTERR_NOCONNECT));
        rb_define_const(cShoutError, "SOCKET",
                        INT2NUM(SHOUTERR_SOCKET));
        rb_define_const(cShoutError, "UNSUPPORTED",
                        INT2NUM(SHOUTERR_UNSUPPORTED));
        rb_define_const(cShoutError, "NOLOGIN",
                        INT2NUM(SHOUTERR_NOLOGIN));
        rb_define_const(cShoutError, "MALLOC",
                        INT2NUM(SHOUTERR_MALLOC));
        rb_define_const(cShoutError, "METADATA",
                        INT2NUM(SHOUTERR_METADATA));
	rb_define_const(cShoutError, "BUSY",
			INT2NUM(SHOUTERR_BUSY));
}

static void raise_shout_error(shout_t *conn) {
        rb_raise(cShoutError, "%d: %s", shout_get_errno(conn),
                                        shout_get_error(conn));
}

/* For metadata-related errors, which don't relate to a shout_t, so we can't
 * use shout_get_errno or shout_get_error on them.
 */
static void raise_nonspecific_shout_error(int errno) {
	rb_raise(cShoutError, "%d", errno);
}

/*
------------------- ShoutMetadata ---------------------
*/

static VALUE cShoutMetadata;

static VALUE _sh_metadata_new(VALUE class) {
        shout_metadata_t *m;
        VALUE meta;

        m = shout_metadata_new();

        meta = Data_Wrap_Struct(class, 0, shout_metadata_free, m);

        rb_obj_call_init(meta, 0, 0);

        return meta;
}

static VALUE _sh_metadata_add(VALUE self, VALUE name, VALUE value) {
        shout_metadata_t *m;
        int err;

        Data_Get_Struct(self, shout_metadata_t, m);
        err = shout_metadata_add(m, STR2CSTR(name), STR2CSTR(value));

        if(err != SHOUTERR_SUCCESS) {
		raise_nonspecific_shout_error(err);
        }

        return value;
}

static void Init_shout_metadata() {

        cShoutMetadata = rb_define_class("ShoutMetadata", rb_cObject);

        rb_define_singleton_method(cShoutMetadata, "new", _sh_metadata_new, 0);
        rb_define_method(cShoutMetadata, "add", _sh_metadata_add, 2);
}

/*
--------------------- Shout ---------------------------
 */

static void invalid_object() {
        rb_raise(rb_eRuntimeError, "invalid shout_connection");
}

#define GET_SC(obj, sc) { \
        Data_Get_Struct(obj, shout_connection, sc); \
        if(sc == NULL) invalid_object(); \
}

static VALUE cShout;

typedef struct _shout_connection {
        shout_t *conn;
} shout_connection;

static void free_sh(shout_connection *cp) {
        if(cp->conn) {
                shout_close(cp->conn);
                /* returns UNCONNECTED, INSANE or SUCCESS. we don't care. */
                shout_free(cp->conn);
        }
        free(cp);
}


/* Make a new shout object. This method does not connect to any server. See
 * #connect. */
static VALUE _sh_initialize(int argc, VALUE *argv, VALUE self) {
        shout_connection *conn;

        conn = ALLOC(shout_connection);
        conn->conn = shout_new();
        DATA_PTR(self) = conn;
        return self;
}

static VALUE _Sh_new(int argc, VALUE *argv, VALUE klass) {
        VALUE self = Data_Wrap_Struct(klass, 0, free_sh, 0);
        rb_obj_call_init(self, argc, argv);
        return self;
}

/* Returns the libshout version, as a string. */
static VALUE _Sh_version(VALUE klass) {
        const char *version;
        VALUE version_String;
        version = shout_version(NULL, NULL, NULL);
        version_String = rb_str_new2(version);
        return version_String;
}

/* Connect to the server. You must set all the parameters you're going to set
 * before connecting.
 */
static VALUE _sh_connect(VALUE self) {
        int err;
        shout_connection *s;
        GET_SC(self, s);

        err = shout_open(s->conn);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return Qtrue;
}

/* Disconnect from the server. */
static VALUE _sh_disconnect(VALUE self) {
        int err;
        shout_connection *s;
        GET_SC(self, s);

        err = shout_close(s->conn);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return Qtrue;
}

/* Returns true if connected, false otherwise, 
 * nil if something really crazy happened. 
 */
static VALUE _sh_connectedp(VALUE self) {
	int err;
	shout_connection *s;
	GET_SC(self, s);

	err = shout_get_connected(s->conn);
	if(err == SHOUTERR_CONNECTED) {
		return Qtrue;
	} else if(err == SHOUTERR_UNCONNECTED) {
		return Qfalse;
	} else {
		return Qnil;
	}
}

/* Send some data. to_send is a String containing the data to send. */
static VALUE _sh_send(VALUE self, VALUE to_send) {
        int err;
        shout_connection *s;
        GET_SC(self, s);

        Check_SafeStr(to_send);
        err = shout_send(s->conn, (unsigned char *) (RSTRING(to_send)->ptr),
                                  RSTRING(to_send)->len);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return Qtrue;
}

/* Sleep the necessary amount of time to play back the audio data sent since
 * the last call to #sync. After calling this, it's time to send more data. */
static VALUE _sh_sync(VALUE self) {
        shout_connection *s;
        GET_SC(self, s);

        shout_sync(s->conn);
        return Qtrue;
}

/* Return the proper amount of time, in milliseconds, before more data needs to
 * be sent. This is for use when you would like to do something else in the
 * intervening time period besides sleep. */
static VALUE _sh_delay(VALUE self) {
        int ms;
        shout_connection *s;
        GET_SC(self, s);

        ms = shout_delay(s->conn);
        return INT2NUM(ms);
}




/* */
VALUE _sh_host(VALUE self) {
        const char *value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_host(s->conn);
        return rb_str_new2(value);
}

/* */
VALUE _sh_port(VALUE self) {
        int value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_port(s->conn);
        return INT2FIX(value);
}

/* */
VALUE _sh_user(VALUE self) {
        const char *value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_user(s->conn);
        return rb_str_new2(value);
}

/* */
VALUE _sh_pass(VALUE self) {
        const char *value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_password(s->conn);
        return rb_str_new2(value);
}

/* */
VALUE _sh_proto(VALUE self) {
        int value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_protocol(s->conn);
        return INT2FIX(value);
}

/* */
VALUE _sh_format(VALUE self) {
        int value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_format(s->conn);
        return INT2FIX(value);
}

/* */
VALUE _sh_mount(VALUE self) {
        const char *value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_mount(s->conn);
        return rb_str_new2(value);
}

/* */
VALUE _sh_dumpfile(VALUE self) {
        const char *value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_dumpfile(s->conn);
        return rb_str_new2(value);
}

/* */
VALUE _sh_agent(VALUE self) {
        const char *value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_agent(s->conn);
        return rb_str_new2(value);
}

/* */
VALUE _sh_public(VALUE self) {
        int value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_public(s->conn);
        return INT2FIX(value);
}

/* */
VALUE _sh_name(VALUE self) {
        const char *value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_name(s->conn);
        return rb_str_new2(value);
}

/* */
VALUE _sh_url(VALUE self) {
        const char *value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_url(s->conn);
        return rb_str_new2(value);
}

/* */
VALUE _sh_genre(VALUE self) {
        const char *value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_genre(s->conn);
        return rb_str_new2(value);
}

/* */
VALUE _sh_description(VALUE self) {
        const char *value;
        shout_connection *s; GET_SC(self, s);

        value = shout_get_description(s->conn);
        return rb_str_new2(value);
}

/* Unimplemented: audio_info */

/* audio_info and metadata should both be objects that always exist, and have
 * [] and []= methods by which you get to the stuff in them. i.e. act like
 * Hashes, because they are, albeit in another language.
 */

/*
--------------------------- setters ------------------------------
*/

/* Set the hostname to connect to. The default is localhost. */
VALUE _sh_host_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_STRING);
        err = shout_set_host(s->conn, RSTRING(value)->ptr);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set the destination port. The default is 8000. */
VALUE _sh_port_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_FIXNUM);
        err = shout_set_port(s->conn, FIX2INT(value));
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set the user to authenticate as. The default is "source". */
VALUE _sh_user_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_STRING);
        err = shout_set_user(s->conn, RSTRING(value)->ptr);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set the password to authenticate with. The default is no password. */
VALUE _sh_pass_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_STRING);
        err = shout_set_password(s->conn, RSTRING(value)->ptr);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set the protocol to use when connecting. Default is Shout::HTTP. Possible
 * values are:
 * [Shout::HTTP]       HTTP; the protocol used by Icecast.
 * [Shout::XAUDIOCAST] XAudioCast. Obsolete.
 * [Shout::ICY]        Icy. Obsolete. Used by Shoutcast.
 */
VALUE _sh_proto_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_FIXNUM);
        err = shout_set_protocol(s->conn, FIX2INT(value));
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set the format of the audio. Possible values are:
 * [Shout::VORBIS] Ogg Vorbis
 * [Shout::MP3]    MP3
 */
VALUE _sh_format_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_FIXNUM);
        err = shout_set_format(s->conn, FIX2INT(value));
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set the mountpoint on the server. */
VALUE _sh_mount_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_STRING);
        err = shout_set_mount(s->conn, RSTRING(value)->ptr);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set a filename where the server should dump the data from this stream. Only
 * do this if you know what you are doing. */
VALUE _sh_dumpfile_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_STRING);
        err = shout_set_dumpfile(s->conn, RSTRING(value)->ptr);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set the User-Agent reported. The default is "libshout/<libshout version>",
 * e.g. "libshout/2.0.0". */
VALUE _sh_agent_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_STRING);
        err = shout_set_agent(s->conn, RSTRING(value)->ptr);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set whether or not this stream should be "public", i.e. advertised to a yp
 * server such as yp.icecast.org. True or false. Nil counts as false. */
VALUE _sh_public_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        if(value == Qfalse || value == Qnil ||
                        (FIXNUM_P(value) && FIX2INT(value) == 0) ) {
                err = shout_set_public(s->conn, 0);
        } else {
                err = shout_set_public(s->conn, 1);
        }
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set the name of the stream, e.g. "monkey's radio tunes." */
VALUE _sh_name_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_STRING);
        err = shout_set_name(s->conn, RSTRING(value)->ptr);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set the URL to send the data to. Takes a string. */
VALUE _sh_url_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_STRING);
        err = shout_set_url(s->conn, RSTRING(value)->ptr);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set the 'genre' of the stream. */
VALUE _sh_genre_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_STRING);
        err = shout_set_genre(s->conn, RSTRING(value)->ptr);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set a longer description of the stream. Probably several lines of text. */
VALUE _sh_description_eq(VALUE self, VALUE value) {
        int err;
        shout_connection *s; GET_SC(self, s);

        Check_Type(value, T_STRING);
        err = shout_set_description(s->conn, RSTRING(value)->ptr);
        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return value;
}

/* Set MP3 metadata. Create a ShoutMetadata object, add some stuff to it and
 * pass it to this method. If the format of the stream isn't MP3, and you try
 * to set its metadata, an exception will most likely be raised.
 */
VALUE _sh_metadata_eq(VALUE self, VALUE meta) {
        int err;
        shout_connection *s; GET_SC(self, s);
        shout_metadata_t *m; Data_Get_Struct(meta, shout_metadata_t, m);

        err = shout_set_metadata(s->conn, m);

        if(err != SHOUTERR_SUCCESS) {
                raise_shout_error(s->conn);
        }
        return meta;
}



/*
----------------------------------------------------------------
*/

void Init_shout()
{
        cShout = rb_define_class("Shout", rb_cObject);

        shout_init();

        rb_define_singleton_method(cShout, "new", _Sh_new, -1);
        rb_define_singleton_method(cShout, "version", _Sh_version, 0);

        rb_define_method(cShout, "initialize", _sh_initialize, -1);
        rb_define_method(cShout, "connect", _sh_connect, 0);
        rb_define_method(cShout, "open", _sh_connect, 0);
        rb_define_method(cShout, "disconnect", _sh_disconnect, 0);
        rb_define_method(cShout, "close", _sh_disconnect, 0);
	rb_define_method(cShout, "connected?", _sh_connectedp, 0);

        rb_define_method(cShout, "send", _sh_send, 1);
        rb_define_method(cShout, "sync", _sh_sync, 0);
        rb_define_method(cShout, "delay", _sh_delay, 0);

        /* getters */
        rb_define_method(cShout, "host",      _sh_host,        0);
        rb_define_method(cShout, "port",      _sh_port,        0);
        rb_define_method(cShout, "user",      _sh_user,        0);
        rb_define_method(cShout, "username",  _sh_user,        0);
        rb_define_method(cShout, "pass",      _sh_pass,        0);
        rb_define_method(cShout, "password",  _sh_pass,        0);
        rb_define_method(cShout, "protocol",  _sh_proto,       0);
        rb_define_method(cShout, "format",    _sh_format,      0);
        rb_define_method(cShout, "mount",     _sh_mount,       0);
        rb_define_method(cShout, "dumpfile",  _sh_dumpfile,    0);
        rb_define_method(cShout, "agent",     _sh_agent,       0);
        rb_define_method(cShout, "user_agent",_sh_agent,       0);
        rb_define_method(cShout, "public",    _sh_public,      0);
        rb_define_method(cShout, "name",      _sh_name,        0);
        rb_define_method(cShout, "url",       _sh_url,         0);
        rb_define_method(cShout, "genre",     _sh_genre,       0);
        rb_define_method(cShout, "description",_sh_description,0);
	/* metadata getting is still unsupported. */
        /* audio info thingy. */
        /* leave for version 2.2 */

        /* setters */
        rb_define_method(cShout, "host=",       _sh_host_eq,        1);
        rb_define_method(cShout, "port=",       _sh_port_eq,        1);
        rb_define_method(cShout, "user=",       _sh_user_eq,        1);
        rb_define_method(cShout, "username=",   _sh_user_eq,        1);
        rb_define_method(cShout, "pass=",       _sh_pass_eq,        1);
        rb_define_method(cShout, "password=",   _sh_pass_eq,        1);
        rb_define_method(cShout, "protocol=",   _sh_proto_eq,       1);
        rb_define_method(cShout, "format=",     _sh_format_eq,      1);
        rb_define_method(cShout, "mount=",      _sh_mount_eq,       1);
        rb_define_method(cShout, "dumpfile=",   _sh_dumpfile_eq,    1);
        rb_define_method(cShout, "agent=",      _sh_agent_eq,       1);
        rb_define_method(cShout, "user_agent=", _sh_agent_eq,       1);
        rb_define_method(cShout, "public=",     _sh_public_eq,      1);
        rb_define_method(cShout, "name=",       _sh_name_eq,        1);
        rb_define_method(cShout, "url=",        _sh_url_eq,         1);
        rb_define_method(cShout, "genre=",      _sh_genre_eq,       1);
        rb_define_method(cShout, "description=", _sh_description_eq,1);
        rb_define_method(cShout, "metadata=",   _sh_metadata_eq,    1);

	rb_define_const(cShout, "HTTP", INT2FIX(SHOUT_PROTOCOL_HTTP));
	rb_define_const(cShout, "XAUDIOCAST", INT2FIX(SHOUT_PROTOCOL_XAUDIOCAST));
	rb_define_const(cShout, "ICY", INT2FIX(SHOUT_PROTOCOL_ICY));

        rb_define_const(cShout, "MP3", INT2FIX(SHOUT_FORMAT_MP3));
        rb_define_const(cShout, "OGG", INT2FIX(SHOUT_FORMAT_OGG));
        rb_define_const(cShout, "VORBIS", INT2FIX(SHOUT_FORMAT_VORBIS));

        Init_shout_error();
        Init_shout_metadata();

        /* shout_shutdown(); goes somewhere. *shrug* */
}
