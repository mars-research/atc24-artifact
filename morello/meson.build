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

morello_sources = files('MorelloDomain.cpp', 'util.S') + [morello_trampoline, morello_rets_trampoline, morello_alt_rets_trampoline]
morello_includes = ['morello']


#trampoline = custom_target('gen-output',
#                        input : ['somefile1.c', 'file2.c'],
#                        output : ['out.c', 'out.h'],
#                        command : [mycomp, '@INPUT@',
#                                   '--c-out', '@OUTPUT0@',
#                                   '--h-out', '@OUTPUT1@'])

