tests = [
  'test_init.c',
  'test_raw.c',
]

foreach t : tests
  test(t, executable(
    t + '.elf',
    t,
    dependencies: [liblfi_dep],
  ), suite: ['liblfi'])
endforeach
