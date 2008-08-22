##############################################################################
# Custom Builders

def substitute_build_function(target, source, env):
  map(lambda s, t: env._substitute_file(s.rstr(), t.rstr()), source, target)
  return None

def substitute_build_str(target, source, env):
  import sys
  map(lambda t: sys.stdout.write("Generating `%s'...\n" % t.rstr()), target)


##############################################################################
# class ExtendedEnvironment

from SCons.Environment import Environment
class ExtendedEnvironment(Environment):
  """The ExtendedEnvironment class extends SCons.Environment.Environment by two
functions for checking for libraries/frameworks in default and user-defined
installation paths.
"""

  def Options(self, file_name, defaults = {}):
    from SCons.Script import Options, EnumOption, PathOption

    opts = Options(file_name)

    opts.Add('CXXFLAGS', 'Flags for the C++ compiler',
        defaults.get('CXXFLAGS',
          '-ansi -std=c++98 -Wall -Wsign-promo -fstrict-aliasing -Wstrict-aliasing'))

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
    opts.Add(EnumOption('BUILD_CONFIG', 'Target for the build',
          defaults.get('BUILD_CONFIG', 'release'),
          allowed_values = self.BUILD_CONFIGS.keys(),
        ))

    opts.Add(EnumOption('BUILD_LIB_TYPE', 'Type of library to build',
          defaults.get('BUILD_LIB_TYPE', 'both'),
          allowed_values = ['static', 'shared', 'both']))

    self.BUILD_PREFIX = 'BUILD_PREFIX'
    opts.Add(PathOption(self.BUILD_PREFIX, 'Build directory',
          defaults.get('BUILD_PREFIX', 'build'),
          PathOption.PathIsDirCreate))

    import os.path
    opts.INSTALL_PREFIX = 'INSTALL_PREFIX'
    opts.Add(PathOption(opts.INSTALL_PREFIX, 'Base path for the installation.',
            defaults.get('INSTALL_PREFIX',
              os.path.join(os.path.sep, 'opt', 'local')),
            PathOption.PathIsDirCreate))

    return opts


  def __init__(self, *args, **kw):
    ### Target resources lists
    self._sources = {}
    self._headers = {}
    self._libs = {}

    #### Create regexes for later use
    import re
    self.__find_regex = re.compile(r'(@(.*?)@)')
    self.__match_regex = re.compile(r'^[a-zA-Z_]+$')

    ### Superclass
    Environment.__init__(self, *args, **kw)

    ### Custom builders
    from SCons.Builder import Builder
    from SCons.Action import Action
    substitute_builder = Builder(action = Action(
          substitute_build_function,
          substitute_build_str),
        src_suffix = '.in',
        name = 'SubstituteBuilder',
        explain = 'foo')
    self.Append(BUILDERS = {
        'Substitute' : substitute_builder,
    })

    ### Set a few variables, for autotools-compatibility
    import os.path
    self['prefix'] = self['INSTALL_PREFIX']
    self['exec_prefix'] = os.path.join(self['INSTALL_PREFIX'], 'bin')
    self['libdir'] = os.path.join(self['INSTALL_PREFIX'], 'lib')
    self['includedir'] = os.path.join(self['INSTALL_PREFIX'], 'include')

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

    ### Store version
    if kw.has_key('version'):
      version_name = 'VERSION'
      if kw.has_key('name'):
        version_name = '%s_%s' % (kw['name'].upper(), version_name)
      self[version_name] = '%d.%d' % kw['version']



  def register_check(self, check, opts = None):
    check.register_options(opts)
    if not self.__dict__.has_key('_custom_checks'):
      self._custom_checks = ()
    self._custom_checks += (check, )


  def init_checks(self, prefix, testdict):
    self._custom_check_prefix = prefix
    for c in self._custom_checks:
      c.init(self, testdict)


  def check_for_framework_in(self, context, text, test_file, suffix, prefixvar,
      outputvar):
    if self['PLATFORM'] != 'darwin':
      # No frameworks on this platform, falling back to libraries...
      return self.check_for_lib(context, text,
          test_file = test_file,
          suffix = suffix,
          prefix_var = prefixvar,
          output_var = outputvar)

    context.Message('Checking for framework %s... ' % text)

    import os.path
    install_dirs = [
      '',
      '/Library/Frameworks',
      os.path.expanduser('~/Library/Frameworks'),
    ]

    if self.has_key(prefixvar) and self[prefixvar]:
      install_dirs = [self[prefixvar]] + install_dirs

    for install_dir in install_dirs:
      old_flags = {
        'FRAMEWORKPATH': self['FRAMEWORKPATH'][:],
        'FRAMEWORKS': self['FRAMEWORKS'][:],
      }

      if install_dir:
        import os.path

        self['FRAMEWORKPATH'].append(install_dir)
        self['FRAMEWORKS'].append(text)


      result, output = context.TryRun(test_file, suffix)
      output = output.strip()

      if result:
        # keep modification to CXXFLAGS, also remember the install_dir in
        # prefixvar
        self[prefixvar] = install_dir
        self[outputvar] = output
        context.sconf.Define(outputvar, output,
            'Version of the installed %s framework' % text)
        context.Result(output)
        return (result, output)
      else:
        self['FRAMEWORKPATH'] = old_flags['FRAMEWORKPATH'][:]
        self['FRAMEWORKS'] = old_flags['FRAMEWORKS'][:]


    result = 0
    context.Result(result)

    # Fall back to libraries.
    return self.check_for_lib_in(context, text, test_file, suffix, prefixvar,
        outputvar, text)


  def check_for_lib(self, context, name, **kw):
    # Testfile suffix, default to C
    suffix = kw.get('suffix', '.c')
    # Testfile contents - in the default, it doesn't contain anything really.
    test_file = kw.get('test_file', "int main(int argc, char **argv) {}")
    # There might be a config variable with an installation directory to look
    # for the library in. The default assumes the name variable is very simple.
    prefix_var = kw.get('prefix_var', '%s_PREFIX' % name.upper())
    # When we detect the library's version, we save it in an output variable.
    output_var = kw.get('output_var', '%s_VERSION' % name.upper())
    # In order to successfully link the test file, we might need a few extra
    # flags.
    extra_flags = kw.get('extra_flags', {})
    # The check might just require headers, no linking against a library.
    header_only = kw.get('header_only', False)

    context.Message('Checking for %s... ' % name)

    # Default dirs to check in.
    install_dirs = [
      '',
      '/usr/local',
      '/opt/local',
    ]

    # The libs we test for are considered to be equivalent to name, except if
    # header_only is true...
    libs = [name]
    if header_only:
      libs = []
    pre_test_libs = context.AppendLIBS(libs)

    # User-provided install dir?
    if self.has_key(prefix_var) and self[prefix_var]:
      install_dirs = [self[prefix_var]] + install_dirs

    # Crawl through all installation directories
    for install_dir in install_dirs:
      # Save and extend current flags
      old_flags = {
        'CXXFLAGS': self['CXXFLAGS'][:],
        'LIBPATH': self['LIBPATH'][:],
      }

      old_libs = context.AppendLIBS(extra_flags.get('LIBS', []))
      self['CXXFLAGS'] += extra_flags.get('CXXFLAGS', [])
      self['LIBPATH'] += extra_flags.get('LIBPATH', [])

      # If there was an install dir (i.e. we're not just using the default
      # environment), we need to add to include and library search paths.
      if install_dir:
        import os.path

        include_dir = os.path.join(install_dir, 'include')
        self['CXXFLAGS'].append(self['INCPREFIX'] + include_dir
            + self['INCSUFFIX'])

        lib_dir = os.path.join(install_dir, 'lib')
        self['LIBPATH'].append(lib_dir)

      # Run test, save output from the test.
      result, output = context.TryRun(test_file, suffix)
      output = output.strip()

      if result:
        # Restore libs completely.
        context.SetLIBS(pre_test_libs)

        # keep modification to CXXFLAGS, also remember the install_dir in
        # prefix_var
        self[prefix_var] = install_dir

        # If there's no output (which would be the case with the default
        # test_file, we'll default to saying "found"
        if not output:
          output = 'found'
        self[output_var] = output
        context.sconf.Define(output_var, output,
            'Version of the installed %s library' % name)
        context.Result(output)
        return (result, output)

      # Restore libs to pre-testrun values
      context.SetLIBS(old_libs)

      # Restore flags
      self['CXXFLAGS'] = old_flags['CXXFLAGS'][:]
      self['LIBPATH'] = old_flags['LIBPATH'][:]

      # continue on to try next path...

    context.SetLIBS(pre_test_libs)

    # no result.
    result = 0
    context.Result(result)
    return result



  def _substitute(self, line):
    """
    substitutes occurrences of @FOO@ patterns with the contents of the environment
    variable 'FOO'.
    """
    mod_line = line

    matches = self.__find_regex.findall(line)
    for pattern, variable in matches:
      if not self.__match_regex.match(variable):
        raise AssertionError('Illegal variable name "%s"' % variable)

      if not self.has_key(variable):
        continue

      import re
      r = re.compile(pattern)
      mod_line = r.sub(self[variable], mod_line)

    return mod_line


  def _substitute_file(self, in_filename, out_filename):
    """
    substitute() on all lines of in_filename, written to out_filename.
    """
    if not in_filename or not out_filename:
      raise AssertionError('Invalid input or output filename: %s, %s' % (
            in_filename, out_filename))

    context = []
    depth = 0
    for line in file(in_filename).readlines():
      line = self._substitute(line)

      if line.startswith('if'):
        cond = line[3:].strip()
        # For now, the only condition we check for is the existence of a variable.
        # By implication, the whole cond string must match a variable name.
        if not self.__match_regex.match(cond):
          raise AssertionError('Illegal condition "%s" in if-statement' % cond)
        depth += 1
        if len(context) < (depth + 1):
          context.append({'content': []})
        if not self.has_key(cond):
          context[depth]['skip'] = True

      elif line.startswith('else'):
        if context[depth].has_key('skip'):
          del context[depth]['skip']
          context[depth]['content'] = []
        else:
          context[depth - 1]['content'] += context[depth]['content']
          context[depth]['content'] = []
          context[depth]['skip'] = True

      elif line.startswith('endif'):
        # Paste the contents of the current context onto the parent context and
        # then delete it.
        if not context[depth].has_key('skip'):
          context[depth - 1]['content'] += context[depth]['content']
        context.pop()
        depth -= 1

      else:
        if len(context) < (depth + 1):
          context.append({'content': []})
        context[depth]['content'].append(line)

    file(out_filename, 'w').writelines(context[0]['content'])



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
    import os.path

    build_prefix = self[self.BUILD_PREFIX]

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



