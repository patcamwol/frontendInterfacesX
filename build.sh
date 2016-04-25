#!/bin/sh
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

if [ "$1" = "clean" ]; then
  make clean
elif [ "$1" = "rpm" ]; then
  # A very simplistic RPM build scenario
  mydir=`dirname $0`
  tmpdir=`mktemp -d`
  cp -r ${mydir} ${tmpdir}/frontendInterfacesX-1.0.0
  tar czf ${tmpdir}/frontendInterfacesX-1.0.0.tar.gz --exclude=".svn" -C ${tmpdir} frontendInterfacesX-1.0.0
  rpmbuild -ta ${tmpdir}/frontendInterfacesX-1.0.0.tar.gz
  rm -rf $tmpdir
else
  # Checks if build is newer than makefile (based on modification time)
  if [ ! -e configure ] || [ ! -e Makefile ] || [ configure.ac -nt Makefile ] || [ Makefile.am -nt Makefile ]; then
    ./reconf
    ./configure
  fi
  make
fi
