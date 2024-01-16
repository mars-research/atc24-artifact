project('domrt', 'c', 'cpp')

engines = get_option('engines')
sources = ['main.cpp']
includes = []

subdir('null')
subdir('ideal')
subdir('morello')

if engines.contains('auto')
  engines += ['null', 'ideal']

  if build_machine.cpu() == 'aarch64'
    engines += ['morello']
  endif
endif

if engines.contains('null')
  sources += null_sources
  includes += null_includes
endif

if engines.contains('ideal')
  sources += ideal_sources
  includes += ideal_includes
endif

if engines.contains('morello')
  sources += morello_sources
  includes += morello_includes
endif

main_cflags = ['-fno-stack-protector']
if build_machine.cpu() == 'x86_64'
  main_cflags += ['-masm=intel']
endif

if get_option('default-engine') == 'morello'
  main_cflags += [
    '-DDEFAULT_DOMAIN_HEADER="MorelloDomain.hpp"',
    '-DDEFAULT_DOMAIN_TYPE=morello::MorelloDomain',
    '-DDEFAULT_DOMAIN_TYPE_MORELLO',
    '-DDEFAULT_CONTEXT_TYPE=morello::MorelloContext',
  ]
elif get_option('default-engine') == 'ideal'
  main_cflags += [
    '-DDEFAULT_DOMAIN_HEADER="IdealDomain.hpp"',
    '-DDEFAULT_DOMAIN_TYPE=ideal::IdealDomain',
    '-DDEFAULT_DOMAIN_TYPE_IDEAL',
    '-DDEFAULT_CONTEXT_TYPE=ideal::IdealContext',
  ]
elif get_option('default-engine') == 'null'
  main_cflags += [
    '-DDEFAULT_DOMAIN_HEADER="NullDomain.hpp"',
    '-DDEFAULT_DOMAIN_TYPE=null::NullDomain',
    '-DDEFAULT_DOMAIN_TYPE_NULL',
    '-DDEFAULT_CONTEXT_TYPE=null::NullContext',
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
