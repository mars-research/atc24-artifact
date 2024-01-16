# TODO: x86_64

if build_machine.cpu() == 'aarch64'
  trampoline = custom_target('aarch64-trampoline',
    input : ['build-trampoline.sh', 'trampoline.aarch64.S'],
    output : ['trampoline.aarch64.bin.o'],
    command : [
      'bash',
      '@INPUT0@',
      'ideal_trampoline', # NAME
      '@INPUT1@', # SRC
      '@OUTPUT@', # DST
      build_machine.cpu(), # CPU
    ] + meson.get_compiler('c').cmd_array(),
  )

  tcb_trampoline = custom_target('aarch64-tcb-trampoline',
    input : ['build-trampoline.sh', 'trampoline.aarch64.tcb.S', 'trampoline.aarch64.S'],
    output : ['trampoline.aarch64.tcb.bin.o'],
    command : [
      'bash',
      '@INPUT0@',
      'ideal_tcb_trampoline', # NAME
      '@INPUT1@', # SRC
      '@OUTPUT@', # DST
      build_machine.cpu(), # CPU
    ] + meson.get_compiler('c').cmd_array(),
  )
elif build_machine.cpu() == 'x86_64'
  trampoline = custom_target('x86_64-trampoline',
    input : ['build-trampoline.sh', 'trampoline.x86_64.S'],
    output : ['trampoline.x86_64.bin.o'],
    command : [
      'bash',
      '@INPUT0@',
      'ideal_trampoline', # NAME
      '@INPUT1@', # SRC
      '@OUTPUT@', # DST
      build_machine.cpu(), # CPU
    ] + meson.get_compiler('c').cmd_array(),
  )

  tcb_trampoline = custom_target('x86_64-tcb-trampoline',
    input : ['build-trampoline.sh', 'trampoline.x86_64.tcb.S', 'trampoline.x86_64.S'],
    output : ['trampoline.x86_64.tcb.bin.o'],
    command : [
      'bash',
      '@INPUT0@',
      'ideal_tcb_trampoline', # NAME
      '@INPUT1@', # SRC
      '@OUTPUT@', # DST
      build_machine.cpu(), # CPU
    ] + meson.get_compiler('c').cmd_array(),
  )
else
  error('Platform not supported: ' + build_machine.cpu())
endif

ideal_sources = files('IdealContext.cpp', 'IdealDomain.cpp') + [
  trampoline,
  tcb_trampoline,
]
ideal_includes = ['ideal']
