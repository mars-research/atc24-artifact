project('domrt', 'c', 'cpp')

engines = {}
sources = ['main.cpp']
includes = []

subdir('null')
subdir('morello')

if get_option('engines').contains('null')
  sources += null_sources
  includes += null_includes
endif

if get_option('engines').contains('morello')
  sources += morello_sources
  includes += morello_includes
endif

main_cflags = ['-fno-stack-protector']
if get_option('default-engine') == 'morello'
  main_cflags += [
    '-DDEFAULT_DOMAIN_HEADER="MorelloDomain.hpp"',
    '-DDEFAULT_DOMAIN_TYPE=MorelloDomain',
  ]
elif get_option('default-engine') == 'null'
  main_cflags += [
    '-DDEFAULT_DOMAIN_HEADER="NullDomain.hpp"',
    '-DDEFAULT_DOMAIN_TYPE=NullDomain',
  ]
else
  error('Invalid default engine')
endif

executable('main',
  sources : sources,
  include_directories : includes,
  c_args : main_cflags,
  cpp_args : main_cflags,
  override_options : ['cpp_std=c++17'],
)
