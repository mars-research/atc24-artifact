morello_trampoline = custom_target('morello-trampoline',
  input : ['build-trampoline.sh', 'trampoline.S'],
  output : ['trampoline.bin.o'],
  command : [
    'bash',
    '@INPUT0@',
    'morello_trampoline', # NAME
    '@INPUT1@', # SRC
    '@OUTPUT@', # DST
  ] + meson.get_compiler('c').cmd_array() + [ '-mabi=purecap' ],
)

morello_tcb_trampoline = custom_target('morello-tcb-trampoline',
  input : ['build-trampoline.sh', 'trampoline.tcb.S', 'trampoline.S'],
  output : ['trampoline.tcb.bin.o'],
  command : [
    'bash',
    '@INPUT0@',
    'morello_tcb_trampoline', # NAME
    '@INPUT1@', # SRC
    '@OUTPUT@', # DST
  ] + meson.get_compiler('c').cmd_array() + [ '-mabi=purecap' ],
)

morello_executive_trampoline = custom_target('morello-executive-trampoline',
  input : ['build-trampoline.sh', 'trampoline.executive.S'],
  output : ['trampoline.executive.bin.o'],
  command : [
    'bash',
    '@INPUT0@',
    'morello_executive_trampoline', # NAME
    '@INPUT1@', # SRC
    '@OUTPUT@', # DST
  ] + meson.get_compiler('c').cmd_array(),
)

morello_executivetcb_trampoline = custom_target('morello-executivetcb-trampoline',
  input : ['build-trampoline.sh', 'trampoline.executivetcb.S', 'trampoline.executive.S'],
  output : ['trampoline.executivetcb.bin.o'],
  command : [
    'bash',
    '@INPUT0@',
    'morello_executivetcb_trampoline', # NAME
    '@INPUT1@', # SRC
    '@OUTPUT@', # DST
  ] + meson.get_compiler('c').cmd_array(),
)

morello_rets_trampoline = custom_target('morello-rets-trampoline',
  input : ['build-trampoline.sh', 'rets-trampoline.S'],
  output : ['rets-trampoline.bin.o'],
  command : [
    'bash',
    '@INPUT0@',
    'morello_rets_trampoline', # NAME
    '@INPUT1@', # SRC
    '@OUTPUT@', # DST
  ] + meson.get_compiler('c').cmd_array(),
)

morello_alt_rets_trampoline = custom_target('morello-alt-rets-trampoline',
  input : ['build-trampoline.sh', 'alt-rets-trampoline.S'],
  output : ['alt-rets-trampoline.bin.o'],
  command : [
    'bash',
    '@INPUT0@',
    'morello_alt_rets_trampoline', # NAME
    '@INPUT1@', # SRC
    '@OUTPUT@', # DST
  ] + meson.get_compiler('c').cmd_array(),
)

morello_sources = files('MorelloContext.cpp', 'MorelloDomain.cpp', 'util.S') + [morello_trampoline, morello_executive_trampoline, morello_executivetcb_trampoline, morello_tcb_trampoline, morello_rets_trampoline, morello_alt_rets_trampoline]
morello_includes = ['morello']


#trampoline = custom_target('gen-output',
#                        input : ['somefile1.c', 'file2.c'],
#                        output : ['out.c', 'out.h'],
#                        command : [mycomp, '@INPUT@',
#                                   '--c-out', '@OUTPUT0@',
#                                   '--h-out', '@OUTPUT1@'])

