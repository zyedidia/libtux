srcs = files(
  'sud.c',
)

mmap = subproject('libmmap').get_variable('mmap')
boxmap = subproject('libboxmap').get_variable('boxmap')

libpal_sud = static_library(
  'pal_sud',
  srcs,
  c_args: c_args_hardened,
  dependencies: [libtux_dep, mmap.as_link_whole(), boxmap.as_link_whole()],
  include_directories: include_directories('include'),
  install: true,
)

libpal_sud_dep = declare_dependency(link_with: libpal_sud, include_directories: include_directories('include'))
