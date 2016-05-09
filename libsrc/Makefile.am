#
# This file is protected by Copyright. Please refer to the COPYRIGHT file
# distributed with this source distribution.
#
# This file is part of Frontend Interfaces X.
#
# Frontend Interfaces X is free software: you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# Frontend Interfaces X is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see http://www.gnu.org/licenses/.
#
###############################################################################
# C++ (via automake and libtool)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = frontendX.pc

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

lib_LTLIBRARIES = libfrontendx-@FRONTENDX_API_VERSION@.la
libfrontendx_@FRONTENDX_API_VERSION@_la_LDFLAGS = -version-info $(FRONTENDX_SO_VERSION)
libfrontendx_@FRONTENDX_API_VERSION@_la_LIBADD = $(BULKIO_LIBS) $(OSSIE_LIBS)

libfrontendx_@FRONTENDX_API_VERSION@_la_SOURCES = \
    cpp/fe_audio_device.cpp \
    cpp/fe_video_device.cpp

## Define the list of public header files and their install location.
library_includedir = $(includedir)/frontendx
library_include_HEADERS = \
	cpp/fe_audio_device.h \
        cpp/fe_audio_device.cpp \
	cpp/fe_audio_port_impl.h \
	cpp/fe_audio_struct_props.h \
	cpp/fe_video_device.h \
	cpp/fe_video_device.cpp \
	cpp/fe_video_port_impl.h \
	cpp/fe_video_struct_props.h \
	cpp/frontendX.h \
	cpp/fex_types.h

## The generated configuration header is installed in its own subdirectory of
## $(libdir).  The reason for this is that the configuration information put
## into this header file describes the target platform the installed library
## has been built for.  Thus the file must not be installed into a location
## intended for architecture-independent files, as defined by the Filesystem
## Hierarchy Standard (FHS).
## The nodist_ prefix instructs Automake to not generate rules for including
## the listed files in the distribution on 'make dist'.  Files that are listed
## in _HEADERS variables are normally included in the distribution, but the
## configuration header file is generated at configure time and should not be
## shipped with the source tarball.
#frontend_libincludedir = $(libdir)/frontend-$(FRONTENDX_API_VERSION)/include

libfrontendx_@FRONTENDX_API_VERSION@_la_CXXFLAGS = -Wall -I./cpp -DLOGGING $(FRONTENDX_INF_INCLUDES) $(BOOST_CPPFLAGS) $(OMNIORB_CFLAGS) $(OSSIE_CFLAGS)

clean-cpp:
	rm -rf cpp/.deps
	rm -f Makefile.in
	rm -f Makefile

###############################################################################
# Python

all-python: build-python

build-python:
	python setup.py build

install-python:
	test -n "$(DESTDIR)" && buildroot="--root=$(DESTDIR)"; \
	  python setup.py install $$buildroot --prefix=$(prefix) --install-purelib=$(prefix)/lib/python --force

clean-python:
	python setup.py clean --all

###############################################################################
# Java

if HAVE_JAVASUPPORT

FRONTEND_CLASSPATH = $(OSSIEHOME)/lib/frontend.jar:$(OSSIEHOME)/lib/FRONTENDInterfaces.jar

@rh_jarfile_rules@

JAVA_DIR := java
JAVA_SRCDIR := $(JAVA_DIR)/src
JAVA_BINDIR := $(JAVA_DIR)/bin

JAVA_SRCS := \
InFrontendAudioPort.java \
OutFrontendAudioPort.java \
FrontendAudioDelegate.java \
InFrontendVideoPort.java \
OutFrontendVideoPort.java \
FrontendVideoDelegate.java \
FEXTypes.java \
FrontendAudioDevice.java \
FrontendVideoDevice.java

java_JARFILES = frontendX.jar

frontendX_jar_SOURCE := $(addprefix $(JAVA_SRCDIR)/frontendX/, $(JAVA_SRCS))
frontendX_jar_CLASSPATH := $(OSSIE_CLASSPATH):$(BULKIO_CLASSPATH):$(FRONTEND_CLASSPATH):$(top_builddir)/FRONTENDXInterfaces.jar
frontendX_jar_MANIFEST := $(JAVA_DIR)/META-INF/MANIFEST.MF
frontendX_jar_JARADD := -C $(JAVA_SRCDIR) frontendX
frontendX_jar_JAVACFLAGS := -g -Xlint

frontendX.jar: $(top_builddir)/FRONTENDXInterfaces.jar

else

clean-java:
	@echo "Java support is disabled"

endif

###############################################################################
# General
#

.PHONY: all-local all-python build-python install-python clean-python reallyclean

all-local: all-python

install-exec-hook: install-python

clean-local: clean-python

reallyclean-recursive: reallyclean-local

reallyclean-local: reallyclean

reallyclean: distclean
	rm -rf cpp/.deps
	rm -f Makefile.in
	rm -f Makefile
	rm -f frontendx.pc
