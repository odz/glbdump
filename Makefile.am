SUBDIRS = libltdl

ACLOCAL_AMFLAGS = -I libltdl/m4
AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS =
AM_LDFLAGS =

INCLUDES = $(LTDLINCL)

bin_PROGRAMS = glbdump
glbdump_SOURCES = glbdump.cc dumper.cc common.cc type.cc array_type.cc base_type.cc const_type.cc \
    pointer_type.cc structure_type.cc typedef.cc
glbdump_LDADD = -lelf -ldwarf $(LIBLTDL)
glbdump_DEPENDENCIES = $(LIBLTDL)

noinst_HEADERS = array_type.h  base_type.h  common.h  const_type.h  dumper.h  pointer_type.h  structure_type.h  typedef.h  type.h
