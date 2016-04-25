
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
#!/usr/bin/env python

from distutils.core import setup
import os

packages = ['frontendX' ]

# Allow the version to be replaced by the ant build script; but, if nothing
# replaces it (i.e. a developer does a command-line build), use 1.X.X
version='__VERSION__'
if version.find('__') == 0:
    version = '1.0.0'

setup(
        name='frontendX',
        version=version,
        description='Python Classes for REDHAWK FRONTEND Interfaces X',
        packages=packages,
        package_dir = { 'frontendX' : 'python' }
    )
