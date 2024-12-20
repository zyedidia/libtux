srcs = files(
  'tux.c',
  'buf.c',
  'proc.c',
  'elfload.c',
  'sys.c',
  'file.c',
  'fd.c',
  'perf.c',
)

subdir('syscalls')
subdir('arch/' + cpuarch)
subdir('host')

cwalk = subproject('cwalk')

defs = []

libelf = dependency('libelf', required: false)
if libelf.found()
  defs += '-DHAVE_LIBELF'
endif

libtux = static_library(
  'tux',
  srcs,
  c_args: c_args_hardened + c_args_warnings + defs,
  include_directories: include_directories('include', 'arch/' + cpuarch),
  dependencies: [cwalk.get_variable('cwalk_dep'), libelf],
  link_whole: cwalk.get_variable('libcwalk'),
  install: true,
)

install_headers('include/tux.h')

libtux_dep = declare_dependency(link_with: libtux, include_directories: include_directories('include'))
