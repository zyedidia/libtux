srcs = files(
  'platform.c',
  'ctx.c',
  'vm.c',
  'sys.c',
)

if cpuarch == 'x64'
  srcs += files(
    'runtime_x64.S',
  )
endif

mmap = subproject('libmmap')
boxmap = subproject('libboxmap')

libpal_sud = static_library(
  'pal_sud',
  srcs,
  c_args: c_args_hardened + c_args_warnings,
  dependencies: [libtux_dep, mmap.get_variable('mmap'), boxmap.get_variable('boxmap')],
  link_whole: [mmap.get_variable('libmmap'), boxmap.get_variable('libboxmap')],
  include_directories: include_directories('include'),
  install: true,
)

libpal_sud_dep = declare_dependency(link_with: libpal_sud, include_directories: include_directories('include'))
