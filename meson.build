project(
  'libtux',
  ['c'],
  default_options: ['warning_level=3', 'buildtype=debugoptimized', 'c_std=gnu11'],
  meson_version: '>=0.60.3',
)

add_global_arguments(
  '-ffunction-sections',
  '-fdata-sections',
  '-fvisibility=hidden',
  language: 'c'
)

add_project_arguments(
  '-D_LARGEFILE64_SOURCE',
  '-D_FILE_OFFSET_BITS=64',
  '-Werror=implicit-function-declaration',
  language: 'c'
)

# Enable various hardening flags
cc = meson.get_compiler('c')
c_args_hardened = []
if cc.has_argument('-fno-strict-aliasing')
  c_args_hardened += '-fno-strict-aliasing'
endif
if cc.has_argument('-fno-delete-null-pointer-checks')
  c_args_hardened += '-fno-delete-null-pointer-checks'
endif
if cc.has_argument('-fno-strict-overflow')
  c_args_hardened += '-fno-strict-overflow'
endif
if cc.has_argument('-Werror=implicit')
  c_args_hardened += '-Werror=implicit'
endif
if cc.has_argument('-Werror=incompatible-pointer-types')
  c_args_hardened += '-Werror=incompatible-pointer-types'
endif
if cc.has_argument('-Werror=int-conversion')
  c_args_hardened += '-Werror=int-conversion'
endif
if cc.has_argument('-ftrivial-auto-var-init=zero')
  c_args_hardened += '-ftrivial-auto-var-init=zero'
endif

c_args_warnings = []
if cc.has_argument('-Wno-unused-parameter')
  c_args_warnings += '-Wno-unused-parameter'
endif
if cc.has_argument('-Wno-gnu-zero-variadic-macro-arguments') # suppress warnings for ##__VA_ARGS__ in print macros
  c_args_warnings += '-Wno-gnu-zero-variadic-macro-arguments'
endif

if get_option('analyzer') and cc.has_argument('-fanalyzer')
  add_global_arguments('-fanalyzer', language: 'c')
endif

cpu = target_machine.cpu_family()

archmap = {
  'aarch64': 'arm64',
  'x86_64': 'x64',
}

if not archmap.has_key(cpu)
  warning('unsupported architecture: ' + cpu)
  subdir_done()
endif

cpuarch = archmap[cpu]

argp = subproject('libargp').get_variable('argp_dep')

host = get_option('host')
if host == 'auto'
  # TODO: autodetect current platform
  host = 'linux'
endif

pal = get_option('pal')
if pal == 'auto'
  # TODO: pick a valid isolation layer based on the chosen host
  pal = 'lfi'
endif

subdir('src')
subdir('pal')
subdir('tools')
