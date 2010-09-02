#!/usr/bin/env ruby 
require "mkmf"

have_func("ruby_xcalloc")
have_func("rb_str_set_len", "ruby.h")
create_makefile("rpatricia")
