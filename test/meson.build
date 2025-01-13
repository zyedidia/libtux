tests = [
  'test_init.c',
  'test_raw.c',
  'test_tux.c',
]

raw = [
  {'file': 'raw/test.S', 'inputs': []}
]

tux = [
  {'file': 'tux/hello.c', 'inputs': []},
  {'file': 'tux/alloc.c', 'inputs': []},
  {'file': 'tux/argv.c', 'inputs': ['one', 'two', 'three']},
  {'file': 'tux/file.c', 'inputs': ['tux/inputs/file.txt']},
  {'file': 'tux/chdir.c', 'inputs': ['tux/inputs', 'file.txt']},
  {'file': 'tux/thread.c', 'inputs': []},
]

test('test_init', executable(
  'test_init.elf',
  'test_init.c',
  dependencies: [liblfi_dep]
), suite: ['liblfi'])

lficc = find_program(cpu + '-lfi-linux-musl-clang', required: false)
if not lficc.found()
  warning('lficc not found: cannot test liblfi')
  subdir_done()
endif

go = find_program('go', required: false)
if not go.found()
  warning('go not found: cannot test liblfi')
  subdir_done()
endif

test_raw = executable(
  'test_raw.elf',
  'test_raw.c',
  dependencies: [liblfi_dep]
)

test_tux = executable(
  'test_tux.elf',
  'test_tux.c',
  dependencies: [liblfi_dep]
)

foreach input : raw
  out = input['file'].replace('/', '_') + '.lfi'
  exec = custom_target(out,
    output: out,
    input: input['file'],
    command: [lficc, '-O2', '-o', '@OUTPUT@', '@INPUT@', '-static-pie', '-nostdlib'],
  )
  expected = input['file'] + '.out'
  test(input['file'],
    go, args: ['run', 'run.go', expected, test_raw, exec] + input['inputs'],
    workdir: meson.current_source_dir(),
    suite: ['liblfi']
  )
endforeach

foreach input : tux
  out = input['file'].replace('/', '_') + '.lfi'
  exec = custom_target(out,
    output: out,
    input: input['file'],
    command: [lficc, '-O2', '-o', '@OUTPUT@', '@INPUT@', '-static-pie'],
  )
  expected = input['file'] + '.out'
  test(input['file'],
    go, args: ['run', 'run.go', expected, test_tux, exec] + input['inputs'],
    workdir: meson.current_source_dir(),
    suite: ['liblfi']
  )
endforeach
