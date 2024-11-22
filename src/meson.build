srcs = files(
  'tux.c',
  'buf.c',
  'proc.c',
  'elf.c',
)

libtux = library(
  'tux',
  srcs,
  c_args: c_args_hardened + ['-fvisibility=hidden'],
  include_directories: include_directories('include'),
  install: true,
)

install_headers('include/tux.h')

libtux_dep = declare_dependency(link_with: libtux, include_directories: include_directories('include'))
