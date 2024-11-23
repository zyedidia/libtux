srcs = files(
  'tux.c',
  'buf.c',
  'proc.c',
  'elf.c',
  'sys.c',
)

subdir('arch/' + cpuarch)

libtux = static_library(
  'tux',
  srcs,
  c_args: c_args_hardened,
  include_directories: include_directories('include'),
  install: true,
)

install_headers('include/tux.h')

libtux_dep = declare_dependency(link_with: libtux, include_directories: include_directories('include'))
