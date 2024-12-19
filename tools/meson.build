libelf = dependency('libelf', required: true)

executable(
  'tux-run',
  sources: ['tux_run.c', 'perf_support.c'],
  dependencies: [libtux_dep, libpal_lfi_dep, argp, libelf],
  install: true,
)
