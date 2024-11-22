project(
  'lfi',
  ['c'],
  default_options: ['warning_level=1', 'buildtype=debugoptimized'],
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

has_auto_var_init = meson.get_compiler('c').has_argument('-ftrivial-auto-var-init=zero')
if has_auto_var_init
  c_args_hardened += '-ftrivial-auto-var-init=zero'
endif

subdir('src')
subdir('pal')
subdir('tools')
