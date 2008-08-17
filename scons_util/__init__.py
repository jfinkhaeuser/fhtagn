##############################################################################
# class ExtendedEnvironment

from SCons.Environment import Environment
class ExtendedEnvironment(Environment):
  """The ExtendedEnvironment class extends SCons.Environment.Environment by two
functions for checking for libraries/frameworks in default and user-defined
installation paths.
"""

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
