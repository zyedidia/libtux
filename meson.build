project(
  'lfi',
  ['c'],
  default_options: ['warning_level=3', 'buildtype=debugoptimized'],
  meson_version: '>=0.60.3',
)

add_project_arguments('-D_LARGEFILE64_SOURCE', '-D_FILE_OFFSET_BITS=64', '-Werror=implicit-function-declaration', language: 'c')

# Enable various hardening flags
c_args_hardened = [
  '-fno-strict-aliasing',
  '-fno-delete-null-pointer-checks',
  '-fno-strict-overflow',
  '-Werror=implicit',
  '-Werror=incompatible-pointer-types',
  '-Werror=int-conversion',
]

c_args_warnings = [
  '-Wno-unused-parameter',
  '-Wno-language-extension-token',
]

has_auto_var_init = meson.get_compiler('c').has_argument('-ftrivial-auto-var-init=zero')
if has_auto_var_init
  c_args_hardened += '-ftrivial-auto-var-init=zero'
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

subdir('src')
subdir('pal')
subdir('tools')
