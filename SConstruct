#
# $Id$
#
# This file is part of the Fhtagn! C++ Library.
# Copyright (C) 2009,2010,2011 Jens Finkhaeuser <unwesen@users.sourceforge.net>.
#
# Author: Jens Finkhaeuser <unwesen@users.sourceforge.net>
#
# This program is licensed as free software for personal, educational or
# other non-commerical uses: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Alternatively, licenses for commercial purposes are available as well.
# Please send your enquiries to the copyright holder's address above.

from scons_util import ExtendedEnvironment
from scons_util import checks

import os.path

###############################################################################
# Environment
class FhtagnEnvironment(ExtendedEnvironment):
  def __init__(self, version, parent = None, args = None, **kw):
    import os

    ### Define options for config.py
    config_file = os.environ.get('SCONS_CONF', 'fhtagn.conf')
    print 'Using config file "%s"...' % config_file
    vars = self.Variables(config_file)
    vars.Add(BoolVariable('GCOV', 'Create code coverage files', 'no'))

    self.register_check(checks.ByteorderCheck, vars)
    self.register_check(checks.BoostCheck, vars)
    self.register_check(checks.CppUnitCheck, vars)

    ### Fixup kwargs
    kw['variables'] = vars
    kw['name'] = 'fhtagn'
    kw['version'] = version
    kw['ENV'] = os.environ
    if not kw.has_key('CPPPATH'):
      kw['CPPPATH'] = []
    kw['CPPPATH'] = ['#', os.path.join('#', '${BUILD_PREFIX}') ] + kw['CPPPATH']
    kw['LIBPATH'] = [ os.path.join('#', '${BUILD_PREFIX}', p) for p in kw['LIBPATH'] ]

    ### Superclass
    ExtendedEnvironment.__init__(self, **kw)
    self.init_checks('FHTAGN', 'CUSTOM_TESTS')

    ### Render help
    Help(vars.GenerateHelpText(self))


  def configure(self):
    if self.GetOption('clean') or self.GetOption('help'):
      return True

    import os.path
    conf = self.Configure(
        conf_dir = os.path.join(self[self.BUILD_PREFIX], 'configure.tmp'),
        log_file = os.path.join(self[self.BUILD_PREFIX], 'config.log'),
        config_h = os.path.join(self[self.BUILD_PREFIX],
          os.path.join('fhtagn', 'fhtagn-config.h')),
        custom_tests = self['CUSTOM_TESTS']
      )
    conf.ByteorderCheck()

    mandatory_headers = [
      # C++ headers (representing STL)
      ('C++', 'cmath'),
      ('C++', 'iostream'),
      ('C++', 'utility'),

      # C++ headers (representing boost)
      ('C++', 'boost/cstdint.hpp'),
      ('C++', 'boost/integer_traits.hpp'),
    ]

    optional_headers = [
      # C++ headers
      ('C++', 'tr1/memory'),
      ('C++', 'boost/shared_ptr.hpp'),
    ]

    optional_types = [
      ['C++', 'boost::int64_t', '#include <boost/cstdint.hpp>'],
      ['C++', 'boost::int32_t', '#include <boost/cstdint.hpp>'],
    ]

    optional_functions = [
      ('C', 'malloc_size'),
      ('C', 'getrusage'),
      ('C', 'gettimeofday'),
    ]

    boost_libs = [
      'signals',
      'thread',
      'program_options',
    ]
    if not conf.BoostCheck(LIBS = boost_libs, min_version = (1, 35, 0), prefer_static = True):
      print ">> Features depending on boost will not be built."
    else:
      if self.is_unix():
        self['CXXFLAGS'] += ['-pthread']
        self['CFLAGS'] += ['-pthread']
      else:
        self['CXXFLAGS'] += ['/MT', '/DBOOST_THREAD_USE_LIB']
        self['CFLAGS'] += ['/MT', '/DBOOST_THREAD_USE_LIB']

    if not conf.CppUnitCheck():
      print ">> CppUnit extensions will not be built."
      print ">> Unit tests will not be built."

    if not conf.CheckLib('gcov'):
      print ">> No code coverage files will be generated."
      env['GCOV'] = False
    elif env.get('GCOV', False):
      env.Append(CFLAGS = ['-fprofile-arcs', '-ftest-coverage'])
      env.Append(CXXFLAGS = ['-fprofile-arcs', '-ftest-coverage'])


    if not self.checkMandatoryHeaders(conf, mandatory_headers):
      return False

    self.checkOptionalHeaders(conf, optional_headers)
    if env.has_key('HAVE_TR1_MEMORY'):
      print '>> Using shared_ptr from TR1'
    elif  env.has_key('HAVE_BOOST_SHARED_PTR_HPP'):
      print '>> Using shared_ptr from boost'
    else:
      print '>> Did not find a shared_ptr definition from either TR1 or boost!'
      return False

    self.checkOptionalTypes(conf, optional_types)
    self.checkOptionalFunctions(conf, optional_functions)


    # add version
    conf.Define('FHTAGN_VERSION', '%d.%d' % self['version'],
            'Fhtagn! version')
    conf.Define('FHTAGN_MAJOR', '%d' % self['version'][0],
            'Fhtagn! major version component')
    conf.Define('FHTAGN_MINOR', '%d' % self['version'][1],
            'Fhtagn! minor version component')

    conf.Finish()
    return True


###############################################################################
# initialize environment
FHTAGN_VERSION = (0, 3) # (major, minor)

env = FhtagnEnvironment(FHTAGN_VERSION, LIBPATH = [
    'fhtagn',
    os.path.join('fhtagn', 'util'),
  ])
# print env.Dump()
# Exit(1)


if not env.GetOption('clean'):
  if not env.configure():
    import sys
    sys.stderr.write('Error: configure failed!\n')
    Exit(1)


###############################################################################
# delegate to SConscript files in subdirs for locating sources

SUBDIRS = [
  'fhtagn',
  os.path.join('fhtagn', 'text'),
  os.path.join('fhtagn', 'text', 'detail'),
  os.path.join('fhtagn', 'restrictions'),
  os.path.join('fhtagn', 'memory'),
  os.path.join('fhtagn', 'meta'),
  os.path.join('fhtagn', 'patterns'),
  os.path.join('fhtagn', 'containers'),
  os.path.join('fhtagn', 'util'),
  'test',
  'tools',
]

if env.has_key('FHTAGN_BOOST_VERSION'):
  SUBDIRS += [
    os.path.join('fhtagn', 'xml'),
    os.path.join('fhtagn', 'threads'),
  ]

import os.path
for subdir in SUBDIRS:
  variant_dir = os.path.join('#', env[env.BUILD_PREFIX], subdir)
  SConscript(
      os.path.join(subdir, 'SConscript'),
      exports = [
        'env',
      ],
      variant_dir = variant_dir
    )


###############################################################################
# install paths
import os.path
lib_path = os.path.join('${INSTALL_PREFIX}', 'lib')
header_base_path = os.path.join('${INSTALL_PREFIX}', 'include')
pkgconfig_path = os.path.join('${INSTALL_PREFIX}', 'lib', 'pkgconfig')


###############################################################################
# define targets

fhtagn_name = os.path.join('#', env[env.BUILD_PREFIX], 'fhtagn', 'fhtagn')
if env['BUILD_LIB_TYPE'] in ('shared', 'both'):
  fhtagn_shared_lib = env.SharedLibrary(fhtagn_name, env.getSources('fhtagn'),
      LIBS = env.getLibs('fhtagn'))
  env.Install(lib_path, fhtagn_shared_lib)
  env.Default(fhtagn_shared_lib)

if env['BUILD_LIB_TYPE'] in ('static', 'both'):
  fhtagn_static_lib = env.StaticLibrary(fhtagn_name, env.getSources('fhtagn'),
      LIBS = env.getLibs('fhtagn'))
  env.Install(lib_path, fhtagn_static_lib)
  env.Default(fhtagn_static_lib)

fhtagn_util_name = os.path.join('#', env[env.BUILD_PREFIX], 'fhtagn', 'util',
    'fhtagn_util')
if env['BUILD_LIB_TYPE'] in ('shared', 'both'):
  fhtagn_util_shared_lib = env.SharedLibrary(fhtagn_util_name,
      env.getSources('fhtagn_util'), LIBS = env.getLibs('fthagn_util'))
  env.Install(lib_path, fhtagn_util_shared_lib)
  env.Default(fhtagn_util_shared_lib)

if env['BUILD_LIB_TYPE'] in ('static', 'both'):
  fhtagn_util_static_lib = env.StaticLibrary(fhtagn_util_name,
      env.getSources('fhtagn_util'), LIBS = env.getLibs('fthagn_util'))
  env.Install(lib_path, fhtagn_util_static_lib)
  env.Default(fhtagn_util_static_lib)


EXECUTABLE_EXTRA_LINKFLAGS = []
if not env.is_unix():
  EXECUTABLE_EXTRA_LINKFLAGS = [
    '/SUBSYSTEM:CONSOLE',
    '/NODEFAULTLIB:LIBCMT.lib',
    '/NODEFAULTLIB:MSVCPRT.lib'
  ]

if env.getSources('testsuite'):
  testsuite_name = os.path.join('#', env[env.BUILD_PREFIX], 'test', 'testsuite')
  testsuite = env.Program(testsuite_name, env.getSources('testsuite'),
      LIBS = env.getLibs('testsuite'),
      LINKFLAGS = env['LINKFLAGS'] + EXECUTABLE_EXTRA_LINKFLAGS)
  env.Alias('check', testsuite)
  env.Default(testsuite)


if env.getSources('allocspeed'):
  allocspeed_name = os.path.join('#', env[env.BUILD_PREFIX], 'test', 'allocspeed')
  allocspeed = env.Program(allocspeed_name, env.getSources('allocspeed'),
      LIBS = env.getLibs('allocspeed'),
      LINKFLAGS = env['LINKFLAGS'] + EXECUTABLE_EXTRA_LINKFLAGS)
  env.Default(allocspeed)


if env.getSources('ftime'):
  ftime_name = os.path.join('#', env[env.BUILD_PREFIX], 'tools', 'ftime')
  ftime = env.Program(ftime_name, env.getSources('ftime'),
      LIBS = env.getLibs('ftime'),
      LINKFLAGS = env['LINKFLAGS'] + EXECUTABLE_EXTRA_LINKFLAGS)
  env.Default(ftime)


if env.is_unix():
  # Add a few env variables for substition later on.
  if env.has_key('FHTAGN_BOOST_VERSION'):
    boost_libs = ''
    if env.has_key('BOOST_PREFIX') and env['BOOST_PREFIX']:
      boost_libs += '%s%s%s' % (env['LIBDIRPREFIX'],
          os.path.join(env['BOOST_PREFIX'], 'lib'), env['LIBDIRSUFFIX'])
    for libname in env.getNamedLibs('boost', ['signals', 'thread']):
      boost_libs += ' %s%s%s' % (env['LIBLINKPREFIX'], libname,
          env['LIBLINKSUFFIX'])
    env['FHTAGN_BOOST_LIBS'] = boost_libs

  if env.has_key('FHTAGN_CPPUNIT_VERSION'):
    cppunit_libs = os.popen('cppunit-config --libs').read()
    env['FHTAGN_UTIL_CPPUNIT_LIBS'] = cppunit_libs


  # Generate .pc files
  pc_name = env.Substitute(os.path.join('#', env[env.BUILD_PREFIX], 'libfhtagn.pc'),
      'libfhtagn.pc.in')
  env.Install(pkgconfig_path, pc_name)
  env.Default(pc_name)

  util_pc_name = env.Substitute(os.path.join('#', env[env.BUILD_PREFIX],
        'libfhtagn_util.pc'),
      'libfhtagn_util.pc.in')
  env.Install(pkgconfig_path, util_pc_name)
  env.Default(util_pc_name)

###############################################################################
# install

env.Alias('install', lib_path)

for subdir, headers in env.getHeadersRelative('fhtagn'):
  full_dir = os.path.join(header_base_path, subdir)
  env.Install(full_dir, headers)
  env.Alias('install', full_dir)

for subdir, headers in env.getHeadersRelative('fhtagn_util'):
  full_dir = os.path.join(header_base_path, subdir)
  env.Install(full_dir, headers)
  env.Alias('install', full_dir)

###############################################################################
# Packaging
env.Tool('packaging')

package_version = '%d.%d' % FHTAGN_VERSION

all_sources = env.stripBuildPrefix(env.FindSourceFiles())
all_sources = [f for f in all_sources if os.path.exists(f)]
all_sources += env.getManifestFiles()

env.Package(
  NAME            = 'fhtagn',
  VERSION         = package_version,
  PACKAGETYPE     = 'src_targz',
  source          = all_sources,
)
env.Package(
  NAME            = 'fhtagn',
  VERSION         = package_version,
  PACKAGETYPE     = 'src_tarbz2',
  source          = all_sources,
)
