# Copyright (C) 2008 the authors.
#
# This file is part of the Fhtagn! C++ Library, and may be distributed under
# the following license terms and conditions as set forth in the COPYING file
# included in this source distribution.

EnsureSConsVersion(0, 98, 4)

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
    opts = self.Options(config_file)
    opts.Add(BoolOption('GCOV', 'Create code coverage files', 'no'))

    self.register_check(checks.BoostCheck, opts)
    self.register_check(checks.CppUnitCheck, opts)

    ### Fixup kwargs
    kw['options'] = opts
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
    Help(opts.GenerateHelpText(self))


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

    mandatory_headers = [

      # C++ headers (representing STL)
      ('C++', 'cmath'),
      ('C++', 'iostream'),
      ('C++', 'utility'),

      # C++ headers (representing boost)
      ('C++', 'boost/cstdint.hpp'),
      ('C++', 'boost/shared_ptr.hpp'),
      ('C++', 'boost/integer_traits.hpp'),
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
    if not conf.BoostCheck(LIBS = boost_libs, min_version = (1, 35, 0)):
      print ">> Features depending on boost will not be built."
    else:
      if self.is_unix():
        self['CXXFLAGS'] += ['-pthread']

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
FHTAGN_VERSION = (0, 1) # (major, minor)

env = FhtagnEnvironment(FHTAGN_VERSION, LIBPATH = [
    'fhtagn',
    os.path.join('fhtagn', 'util'),
  ])

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
]

if env.has_key('FHTAGN_BOOST_VERSION'):
  SUBDIRS += [
    os.path.join('fhtagn', 'xml'),
    os.path.join('fhtagn', 'threads'),
  ]

import os.path
for subdir in SUBDIRS:
  build_dir = os.path.join('#', env[env.BUILD_PREFIX], subdir)
  SConscript(
      os.path.join(subdir, 'SConscript'),
      exports = [
        'env',
      ],
      build_dir = build_dir
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


if env.getSources('testsuite'):
  testsuite_name = os.path.join('#', env[env.BUILD_PREFIX], 'test', 'testsuite')
  testsuite = env.Program(testsuite_name, env.getSources('testsuite'),
      LIBS = env.getLibs('testsuite'))
  env.Alias('check', testsuite)
  env.Default(testsuite)


if env.getSources('allocspeed'):
  allocspeed_name = os.path.join('#', env[env.BUILD_PREFIX], 'test', 'allocspeed')
  allocspeed = env.Program(allocspeed_name, env.getSources('allocspeed'),
      LIBS = env.getLibs('allocspeed'))
  env.Default(allocspeed)


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
