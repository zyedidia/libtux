srcs = files(
  'platform.c',
  'ctx.c',
  'vm.c',
)

subdir('arch/' + cpuarch)

mmap = subproject('libmmap')
boxmap = subproject('libboxmap')

libpal_lfi = static_library(
  'pal_lfi',
  srcs,
  c_args: c_args_hardened + c_args_warnings,
  dependencies: [libtux_dep, mmap.get_variable('mmap'), boxmap.get_variable('boxmap')],
  link_whole: [mmap.get_variable('libmmap'), boxmap.get_variable('libboxmap')],
  include_directories: include_directories('include'),
  install: true,
)

libpal_lfi_dep = declare_dependency(link_with: libpal_lfi, include_directories: include_directories('include'))
