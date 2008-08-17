# Copyright (C) 2008 the authors.
#
# This file is part of the Fhtagn! C++ Library, and may be distributed under
# the following license terms and conditions as set forth in the COPYING file
# included in this source distribution.

EnsureSConsVersion(0, 98, 4)

from scons_util import ExtendedEnvironment
from scons_util import checks

###############################################################################
# Environment
class FhtagnEnvironment(ExtendedEnvironment):
  def __init__(self, version, parent = None, args = None, **kw):
    ### Remember version
    self.version = version

    ### Target resources lists
    self._sources = {}
    self._headers = {}
    self._libs = {}

    ### Define options for config.py
    opts = Options('fhtagn.conf')

    opts.Add('CXXFLAGS', 'Flags for the C++ compiler',
        '-ansi -std=c++98 -Wall -Wsign-promo -fstrict-aliasing -Wstrict-aliasing')

    self.BUILD_CONFIGS = {
      'debug': {
        'CPPDEFINES': { 'DEBUG': '1' },
        'CXXFLAGS': [ '-ggdb', '-O0' ],
      },
      'release': {
        'CPPDEFINES': { 'NDEBUG': '1' },
        'CXXFLAGS': [ '-ggdb', '-O2' ],
      },
    }
    opts.Add(EnumOption('BUILD_CONFIG', 'Target for the build', 'release',
          allowed_values = self.BUILD_CONFIGS.keys(),
        ))

    opts.Add(EnumOption('BUILD_LIB_TYPE', 'Type of library to build', 'both',
          allowed_values = ['static', 'shared', 'both']))

    self.BUILD_PREFIX = 'BUILD_PREFIX'
    opts.Add(PathOption(self.BUILD_PREFIX, 'Build directory', 'build',
          PathOption.PathIsDirCreate))

    import os.path
    opts.INSTALL_PREFIX = 'INSTALL_PREFIX'
    opts.Add(PathOption(opts.INSTALL_PREFIX, 'Base path for the installation.',
            os.path.join(os.path.sep, 'opt', 'local'),
            PathOption.PathIsDirCreate))

    self.register_check(checks.BoostCheck, opts)
    self.register_check(checks.CppUnitCheck, opts)

    ### Fixup kwargs
    import os
    kw['options'] = opts
    kw['ENV'] = os.environ
    if not kw.has_key('CPPPATH'):
      kw['CPPPATH'] = []
    kw['CPPPATH'] = ['#', os.path.join('#', '${BUILD_PREFIX}') ] + kw['CPPPATH']
    kw['LIBPATH'] = [ os.path.join('#', '${BUILD_PREFIX}', p) for p in kw['LIBPATH'] ]

    ### Superclass
    Environment.__init__(self, **kw)
    self.init_checks('FHTAGN', 'CUSTOM_TESTS')

    ### Fixup Options
    self['CXXFLAGS'] = self['CXXFLAGS'].split()

    for k, v in self.BUILD_CONFIGS[self['BUILD_CONFIG']].items():
      if not self.has_key(k):
        self[k] = v
      else:
        if type(v) == type(dict()):
          self[k].update(v)
        elif type(v) == type(list()):
          self[k].extend(v)
        else:
          self[k] += v

    ### Render help
    Help(opts.GenerateHelpText(self))

  def configure(self):

    import os.path
    conf = self.Configure(
        conf_dir = os.path.join(self[self.BUILD_PREFIX], 'configure.tmp'),
        log_file = os.path.join(self[self.BUILD_PREFIX], 'config.log'),
        config_h = os.path.join(self[self.BUILD_PREFIX],
          'fhtagn/fhtagn-config.h'),
        custom_tests = self['CUSTOM_TESTS']
      )

    mandatory_headers = [
      # C headers
      ('C', 'stdint.h'),

      # C++ headers (representing STL)
      ('C++', 'cmath'),
      ('C++', 'iostream'),
      ('C++', 'utility'),

      # C++ headers (representing boost)
      ('C++', 'boost/shared_ptr.hpp'),
      ('C++', 'boost/integer_traits.hpp'),
    ]

    optional_types = [
      ('C++', 'int64_t', '#include <stdint.h>'),
      ('C++', 'int32_t', '#include <stdint.h>'),
    ]

    if not conf.BoostCheck():
      print ">> Features depending on boost will not be built."

    if not conf.CppUnitCheck():
      print ">> CppUnit extensions will not be built."
      print ">> Unit tests will not be built."

    for info in mandatory_headers:
      if info[0] == 'C':
        if not conf.CheckCHeader(info[1], include_quotes = '<>'):
          return False
      elif info[0] == 'C++':
        if not conf.CheckCXXHeader(info[1], include_quotes = '<>'):
          return False

    for info in optional_types:
      header = False
      if len(info) == 3:
        header = info[2]
      conf.CheckType(info[1], includes = header, language = info[0]);


    # add version
    conf.Define('FHTAGN_VERSION', '%d.%d' % self.version,
            'Fhtagn! version')
    conf.Define('FHTAGN_MAJOR', '%d' % self.version[0],
            'Fhtagn! major version component')
    conf.Define('FHTAGN_MINOR', '%d' % self.version[1],
            'Fhtagn! minor version component')

    conf.Finish()
    return True


  def addSources(self, target, sources):
    if not self._sources.has_key(target):
      self._sources[target] = []
    self._sources[target].extend(self.arg2nodes(sources))


  def addLibs(self, target, libs):
    if not self._libs.has_key(target):
      self._libs[target] = []
    self._libs[target].extend(libs)


  def addHeaders(self, target, headers):
    if not self._headers.has_key(target):
      self._headers[target] = []
    self._headers[target].extend(self.arg2nodes(headers))


  def getSources(self, target):
    return self._sources.get(target, [])


  def getLibs(self, target):
    return self._libs.get(target, [])


  def getHeaders(self, target):
    return self._headers.get(target, [])


  def getHeadersRelative(self, target):
    retval = {}

    build_prefix = env[env.BUILD_PREFIX]

    for header in self.getHeaders(target):
      build_path = header.get_path()
      if not build_path.startswith(build_prefix):
        sys.stderr.write('Header file %s in unknown location.\n', build_path)
        continue

      build_path = build_path[len(build_prefix):]
      while build_path[0] == os.path.sep:
        build_path = build_path[1:]

      path, filename = os.path.split(build_path)

      if not retval.has_key(path):
        retval[path] = []
      retval[path].append(header)

    return retval.items()




###############################################################################
# initialize environment
FHTAGN_VERSION = (0, 1) # (major, minor)

env = FhtagnEnvironment(FHTAGN_VERSION, LIBPATH = [ 'fhtagn', 'fhtagn/util' ])

if not env.GetOption('clean'):
  if not env.configure():
    import sys
    sys.stderr.write('Error: configure failed!\n')
    Exit(1)


###############################################################################
# delegate to SConscript files in subdirs for locating sources

SUBDIRS = [
  'fhtagn',
  'fhtagn/detail',
  'fhtagn/text',
  'fhtagn/text/detail',
  'fhtagn/restrictions',
  'test',
]

if env.has_key('FHTAGN_BOOST_VERSION'):
  SUBDIRS += [
    'fhtagn/xml',
    'fhtagn/threads',
  ]

if env.has_key('FHTAGN_CPPUNIT_VERSION'):
  SUBDIRS += [
    'fhtagn/util',
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


###############################################################################
# define targets

fhtagn_name = os.path.join('#', env[env.BUILD_PREFIX], 'fhtagn')
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

fhtagn_util_name = os.path.join('#', env[env.BUILD_PREFIX], 'fhtagn_util')
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



###############################################################################
# install

env.Alias('install', lib_path)

for dir, headers in env.getHeadersRelative('fhtagn'):
  full_dir = os.path.join(header_base_path, dir)
  env.Install(full_dir, headers)
  env.Alias('install', full_dir)

for dir, headers in env.getHeadersRelative('fhtagn_util'):
  full_dir = os.path.join(header_base_path, dir)
  env.Install(full_dir, headers)
  env.Alias('install', full_dir)
