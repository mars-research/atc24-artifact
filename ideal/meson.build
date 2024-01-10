# TODO: x86_64

aarch64_trampoline = custom_target('aarch64-trampoline',
  input : ['build-trampoline.sh', 'trampoline.aarch64.S'],
  output : ['trampoline.aarch64.bin.o'],
  command : [
    'bash',
    '@INPUT0@',
    'ideal_trampoline', # NAME
    '@INPUT1@', # SRC
    '@OUTPUT@', # DST
  ] + meson.get_compiler('c').cmd_array(),
)

aarch64_tcb_trampoline = custom_target('aarch64-tcb-trampoline',
  input : ['build-trampoline.sh', 'trampoline.aarch64.tcb.S', 'trampoline.aarch64.S'],
  output : ['trampoline.aarch64.tcb.bin.o'],
  command : [
    'bash',
    '@INPUT0@',
    'ideal_tcb_trampoline', # NAME
    '@INPUT1@', # SRC
    '@OUTPUT@', # DST
  ] + meson.get_compiler('c').cmd_array(),
)

ideal_sources = files('IdealContext.cpp', 'IdealDomain.cpp') + [
  aarch64_trampoline,
  aarch64_tcb_trampoline,
]
ideal_includes = ['ideal']
