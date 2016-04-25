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

# By default, the RPM will install to the standard REDHAWK OSSIE root location (/usr/local/redhawk/core)
%{!?_ossiehome: %global _ossiehome /usr/local/redhawk/core}
%define _prefix %{_ossiehome}
Prefix: %{_prefix}

# Point install paths to locations within our target OSSIE root
%define _sysconfdir    %{_prefix}/etc
%define _localstatedir %{_prefix}/var
%define _mandir        %{_prefix}/man
%define _infodir       %{_prefix}/info

# Assume Java support by default. Use "rpmbuild --without java" to disable
%bcond_without java

Summary:        The frontend X library for REDHAWK
Name:           frontendInterfacesX
Version:        1.0.0
Release:        1%{?dist}
License:        LGPLv3+
Group:          REDHAWK/Interfaces
Source:         %{name}-%{version}.tar.gz
Vendor:         REDHAWK

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-buildroot

Requires:       redhawk >= 2.0.0, redhawk < 2.1.0
Requires:       bulkioInterfaces >= 2.0.0, bulkioInterfaces < 2.1.0
BuildRequires:  redhawk-devel >= 2.0.0, redhawk-devel < 2.1.0
BuildRequires:  bulkioInterfaces >= 2.0.0, bulkioInterfaces < 2.1.0
BuildRequires:  frontendInterfaces >= 2.3.0, frontendInterfaces < 2.4.0

%description
Libraries and interface definitions for frontendX.
 * Commit: __REVISION__
 * Source Date/Time: __DATETIME__


%prep
%setup

%build
./reconf
%configure %{?_without_java: --disable-java}
make

%install
rm -rf --preserve-root $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf --preserve-root $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_datadir}/idl/redhawk/FRONTENDX
%{_includedir}/redhawk/FRONTENDX
%{_prefix}/lib/python/redhawk/frontendInterfacesX
%{_prefix}/lib/python/frontendX
%{_libdir}/libfrontendInterfacesX.*
%{_libdir}/pkgconfig/frontendInterfacesX.pc
%{_includedir}/frontendX
%{_libdir}/libfrontendX-*
%{_libdir}/pkgconfig/frontendX.pc
%if 0%{?rhel} >= 6
%{_prefix}/lib/python/frontendInterfacesX-%{version}-py%{python_version}.egg-info
%{_prefix}/lib/python/frontendX-%{version}-py%{python_version}.egg-info
%endif
%if %{with java}
%{_prefix}/lib/frontendX.jar
%{_prefix}/lib/FRONTENDInterfacesX.jar
%endif

%post
/sbin/ldconfig

%postun
/sbin/ldconfig
