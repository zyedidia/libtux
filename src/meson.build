srcs = files(
  'tux.c',
  'buf.c',
  'proc.c',
  'elf.c',
  'sys.c',
  'file.c',
  'fd.c',
)

subdir('syscalls')
subdir('arch/' + cpuarch)

cwalk_dep = subproject('cwalk').get_variable('cwalk_dep')

libtux = static_library(
  'tux',
  srcs,
  c_args: c_args_hardened + c_args_warnings,
  include_directories: include_directories('include', 'arch/' + cpuarch),
  dependencies: [cwalk_dep.as_link_whole()],
  install: true,
)

install_headers('include/tux.h')

libtux_dep = declare_dependency(link_with: libtux, include_directories: include_directories('include'))
