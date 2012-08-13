import os, errno, platform, distutils.core, distutils.sysconfig


build_path_guess = 'build/lib.%s-%s-%s.%s' % (
    platform.system().lower(),
    platform.machine(),
    platform.python_version_tuple()[0],
    platform.python_version_tuple()[1])


try:
    os.symlink(build_path_guess + '/rservr/_rservr.so', '_rservr.so')
except OSError as error:
    if error.errno != errno.EEXIST:
        raise error


try:
    prefix_paths = [os.environ['RSERVR_PREFIX']]
except KeyError:
    prefix_paths = ['/usr', '/usr/local']

libexec_search_paths = [prefix_paths[i] + '/libexec/rservr' for i in range(len(prefix_paths))]
include_search_paths = [prefix_paths[i] + '/include' for i in range(len(prefix_paths))]


submodules = ['dataref', 'netcntl', 'passthru', 'ready', 'rqconfig', 'rqsrvc', 'trigger']

extra_files = {
    'dataref': ['src/rsvp-dataref-thread.c'],
    'netcntl': [],
    'passthru': ['src/rsvp-passthru-assist.c'],
    'ready': [],
    'rqconfig': ['src/rsvp-rqconfig-thread.c'],
    'rqsrvc': ['src/rsvp-rqsrvc-auto.c'],
    'trigger': [] }

extra_libs = {
    'dataref': [],
    'netcntl': [],
    'passthru': ['rsvp-passthru-assist'],
    'ready': [],
    'rqconfig': [],
    'rqsrvc': ['rsvp-rqsrvc-auto'],
    'trigger': [] }

extra_objects = {
    'dataref': [],
    'netcntl': [],
    'passthru': [],
    'ready': [],
    'rqconfig': [],
    'rqsrvc': [],
    'trigger': [] }


ext_modules = [
    distutils.core.Extension(name = 'rservr._rservr',
        sources = [
        'src/rservr.c',
        'src/load-all.c',
        'src/command.c',
        'src/client.c',
        'src/log-output.c',
        'src/admin-client.c',
        'src/service-client.c',
        'src/control-client.c',
        'src/resource-client.c',
        'src/monitor-client.c',
        'src/detached-client.c',
        'src/command-queue.c',
        'src/message-queue.c',
        'src/request.c',
        'src/response.c',
        'src/load-plugin.c',
        'src/command-table.c',
        'src/remote-service.c',
        'src/client-timing.c',
        'src/tools.c',
        'src/label-check.c',
        'src/config-parser.c'],
        libraries = ['rservr-client'],
        include_dirs = include_search_paths)] + [
    distutils.core.Extension(name = 'rservr.rsvp_' + submodules[i],
        sources = [
        'src/load-all.c',
        'src/rsvp-' + submodules[i] + '-hook.c',
        'src/rsvp-' + submodules[i] + '.c'] + extra_files[submodules[i]],
        libraries = ['rservr-client'] + extra_libs[submodules[i]] + ['rsvp-' + submodules[i]],
        extra_objects = ['_rservr.so'] + extra_objects[submodules[i]],
        include_dirs = include_search_paths,
        library_dirs = libexec_search_paths,
        runtime_library_dirs = libexec_search_paths + [distutils.sysconfig.get_python_lib() + '/rservr'])
    for i in range(len(submodules))]


distutils.core.setup(name = 'rservr', version = '0.1', packages = ['rservr'],
    description = 'Python extensions for the Resourcerver project.',
    author = 'Kevin P. Barry', author_email = 'ta0kira@users.berlios.de',
    package_data = {'rservr' : ['rservr/__init__.py'] }, ext_modules = ext_modules)
