/* Script for -z combreloc -z separate-code -z relro -z now */
/* Copyright (C) 2014-2020 Free Software Foundation, Inc.
   Copying and distribution of this script, with or without modification,
   are permitted in any medium without royalty provided the copyright
   notice and this notice are preserved.  */
OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64",
              "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)
SEARCH_DIR("/nix/store/fh9f7pr9kxfwvs8lm4479lvhrkzmmkdw-binutils-2.35.1/x86_64-unknown-linux-gnu/lib64"); SEARCH_DIR("/nix/store/fh9f7pr9kxfwvs8lm4479lvhrkzmmkdw-binutils-2.35.1/lib64"); SEARCH_DIR("/nix/store/fh9f7pr9kxfwvs8lm4479lvhrkzmmkdw-binutils-2.35.1/x86_64-unknown-linux-gnu/lib"); SEARCH_DIR("/nix/store/fh9f7pr9kxfwvs8lm4479lvhrkzmmkdw-binutils-2.35.1/lib");
SECTIONS
{
  PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x400000)); . = SEGMENT_START("text-segment", 0x400000) + SIZEOF_HEADERS;

  .interp         : { *(.interp) }
  .note.gnu.build-id  : { *(.note.gnu.build-id) }
  .hash           : { *(.hash) }
  .gnu.hash       : { *(.gnu.hash) }
  .dynsym         : { *(.dynsym) }
  .dynstr         : { *(.dynstr) }
  .gnu.version    : { *(.gnu.version) }
  .gnu.version_d  : { *(.gnu.version_d) }
  .gnu.version_r  : { *(.gnu.version_r) }
  .rela.dyn       :
    {
      *(.rela.init)
      *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*)
      *(.rela.fini)
      *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*)
      *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*)
      *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*)
      *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*)
      *(.rela.ctors)
      *(.rela.dtors)
      *(.rela.got)
      *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*)
      *(.rela.ldata .rela.ldata.* .rela.gnu.linkonce.l.*)
      *(.rela.lbss .rela.lbss.* .rela.gnu.linkonce.lb.*)
      *(.rela.lrodata .rela.lrodata.* .rela.gnu.linkonce.lr.*)
      *(.rela.ifunc)
    }
  .rela.plt       :
    {
      *(.rela.plt)
      PROVIDE_HIDDEN (__rela_iplt_start = .);
      *(.rela.iplt)
      PROVIDE_HIDDEN (__rela_iplt_end = .);
    }
  . = ALIGN(CONSTANT (MAXPAGESIZE));
  .init           :
  {
    KEEP (*(SORT_NONE(.init)))
  }
  .plt            : { *(.plt) *(.iplt) }
.plt.got        : { *(.plt.got) }
.plt.sec        : { *(.plt.sec) }
  .text           :
  {
    *(.text.unlikely .text.*_unlikely .text.unlikely.*)
    *(.text.exit .text.exit.*)
    *(.text.startup .text.startup.*)
    *(.text.hot .text.hot.*)
    *(SORT(.text.sorted.*))
    *(.text .stub .text.* .gnu.linkonce.t.*)
    /* .gnu.warning sections are handled specially by elf.em.  */
    *(.gnu.warning)
  }

 .domain_a : 
  {
      . = ALIGN(4096);
    _domain_a_start = .;
     *(.domain_a*)
     . = ALIGN(4096);
    _domain_a_end = .;
  }

   .trampoline : 
  {
      . = ALIGN(4096);
    _trampoline_start = .;
     *(.trampoline*)
     . = ALIGN(4096);
    _trampoline_end = .;
  } 
 .domain_b : 
  {
      . = ALIGN(4096);
    _domain_b_start = .;
     *(.domain_b*)
     . = ALIGN(4096);
    _domain_b_end = .;
  }

 .domain_c : 
  {
      . = ALIGN(4096);
    _domain_c_start = .;
     *(.domain_C*)
     *(.domain_c*)
     . = ALIGN(4096);
    _domain_c_end = .;
  }

 .domain_d : 
  {
      . = ALIGN(4096);
    _domain_d_start = .;
     *(.domain_D*)
     *(.domain_d*)
     . = ALIGN(4096);
    _domain_d_end = .;
  }

   .domain_e : 
  {
      . = ALIGN(4096);
    _domain_e_start = .;
     *(.domain_E*)
     *(.domain_e*)
     . = ALIGN(4096);
    _domain_e_end = .;
  }
   .domain_f : 
  {
      . = ALIGN(4096);
    _domain_f_start = .;
     *(.domain_F*)
     *(.domain_f*)
     . = ALIGN(4096);
    _domain_f_end = .;
  }
     .domain_g : 
  {
      . = ALIGN(4096);
    _domain_g_start = .;
     *(.domain_G*)
     *(.domain_g*)
     . = ALIGN(4096);
    _domain_g_end = .;
  }
     .domain_h : 
  {
      . = ALIGN(4096);
    _domain_h_start = .;
     *(.domain_H*)
     *(.domain_h*)
     . = ALIGN(4096);
    _domain_h_end = .;
  }
     .domain_i : 
  {
      . = ALIGN(4096);
    _domain_i_start = .;
     *(.domain_I*)
     *(.domain_i*)
     . = ALIGN(4096);
    _domain_i_end = .;
  }
     .domain_j : 
  {
      . = ALIGN(4096);
    _domain_j_start = .;
     *(.domain_J*)
     *(.domain_i*)
     . = ALIGN(4096);
    _domain_j_end = .;
  }
   .domain_k : 
  {
      . = ALIGN(4096);
    _domain_k_start = .;
     *(.domain_K*)
     *(.domain_k*)
     . = ALIGN(4096);
    _domain_k_end = .;
  }
     .domain_l : 
  {
      . = ALIGN(4096);
    _domain_l_start = .;
     *(.domain_L*)
     *(.domain_l*)
     . = ALIGN(4096);
    _domain_l_end = .;
  }
   .domain_m : 
  {
      . = ALIGN(4096);
    _domain_m_start = .;
     *(.domain_M*)
     *(.domain_m*)
     . = ALIGN(4096);
    _domain_m_end = .;
  }
  
  .fini           :
  {
    KEEP (*(SORT_NONE(.fini)))
  }
  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);
  . = ALIGN(CONSTANT (MAXPAGESIZE));
  /* Adjust the address for the rodata segment.  We want to adjust up to
     the same address within the page on the next page up.  */
  . = SEGMENT_START("rodata-segment", ALIGN(CONSTANT (MAXPAGESIZE)) + (. & (CONSTANT (MAXPAGESIZE) - 1)));
  .rodata         : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
  .rodata1        : { *(.rodata1) }
  .eh_frame_hdr   : { *(.eh_frame_hdr) *(.eh_frame_entry .eh_frame_entry.*) }
  .eh_frame       : ONLY_IF_RO { KEEP (*(.eh_frame)) *(.eh_frame.*) }
  .gcc_except_table   : ONLY_IF_RO { *(.gcc_except_table .gcc_except_table.*) }
  .gnu_extab   : ONLY_IF_RO { *(.gnu_extab*) }
  /* These sections are generated by the Sun/Oracle C++ compiler.  */
  .exception_ranges   : ONLY_IF_RO { *(.exception_ranges*) }
  /* Adjust the address for the data segment.  We want to adjust up to
     the same address within the page on the next page up.  */
  . = DATA_SEGMENT_ALIGN (CONSTANT (MAXPAGESIZE), CONSTANT (COMMONPAGESIZE));
  /* Exception handling  */
  .eh_frame       : ONLY_IF_RW { KEEP (*(.eh_frame)) *(.eh_frame.*) }
  .gnu_extab      : ONLY_IF_RW { *(.gnu_extab) }
  .gcc_except_table   : ONLY_IF_RW { *(.gcc_except_table .gcc_except_table.*) }
  .exception_ranges   : ONLY_IF_RW { *(.exception_ranges*) }
  /* Thread Local Storage sections  */
  .tdata          :
   {
     PROVIDE_HIDDEN (__tdata_start = .);
     *(.tdata .tdata.* .gnu.linkonce.td.*)
   }
  .tbss           : { *(.tbss .tbss.* .gnu.linkonce.tb.*) *(.tcommon) }
  .preinit_array    :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  }
  .init_array    :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);
  }
  .fini_array    :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);
  }
  .ctors          :
  {
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  }
  .dtors          :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  }
  .jcr            : { KEEP (*(.jcr)) }
  .data.rel.ro : { *(.data.rel.ro.local* .gnu.linkonce.d.rel.ro.local.*) *(.data.rel.ro .data.rel.ro.* .gnu.linkonce.d.rel.ro.*) }
  .dynamic        : { *(.dynamic) }
  .got            : { *(.got.plt) *(.igot.plt) *(.got) *(.igot) }
  . = DATA_SEGMENT_RELRO_END (0, .);
  .data           :
  {
    *(.data .data.* .gnu.linkonce.d.*)
    SORT(CONSTRUCTORS)
  }
  .data1          : { *(.data1) }
  _edata = .; PROVIDE (edata = .);

  .b_buff : {
    . = ALIGN(4096);
    _b_buff_start = .;
     *(.b_buff*)
     . = ALIGN(4096);
    _b_buff_end = .;
  }

  .c_buff : {
    . = ALIGN(4096);
    _c_buff_start = .;
     *(.C_buff*)
     . = ALIGN(4096);
    _c_buff_end = .;
  }

  .a_buff : {
    . = ALIGN(4096);
    _a_buff_start = .;
     *(.a_buff*)
     . = ALIGN(4096);
    _a_buff_end = .;
  }

  .d_buff : {
    . = ALIGN(4096);
    _d_buff_start = .;
     *(.D_buff*)
     . = ALIGN(4096);
    _d_buff_end = .;
  }

  .e_buff : {
    . = ALIGN(4096);
    _E_buff_start = .;
     *(.E_buff*)
     . = ALIGN(4096);
    _e_buff_end = .;
  }
    .f_buff : {
    . = ALIGN(4096);
    _f_buff_start = .;
     *(.F_buff*)
     . = ALIGN(4096);
    _f_buff_end = .;
  }
    .g_buff : {
    . = ALIGN(4096);
    _g_buff_start = .;
     *(.G_buff*)
     . = ALIGN(4096);
    _g_buff_end = .;
  }

  .h_buff : {
    . = ALIGN(4096);
    _h_buff_start = .;
     *(.H_buff*)
     . = ALIGN(4096);
    _h_buff_end = .;
  }

  .i_buff : {
    . = ALIGN(4096);
    _i_buff_start = .;
     *(.I_buff*)
     . = ALIGN(4096);
    _i_buff_end = .;
  }

  j._buff : {
    . = ALIGN(4096);
    _j_buff_start = .;
     *(.J_buff*)
     . = ALIGN(4096);
    _j_buff_end = .;
  }
  .k_buff : {
    . = ALIGN(4096);
    _k_buff_start = .;
     *(.K_buff*)
     . = ALIGN(4096);
    _k_buff_end = .;
  }  
  
  .l_buff : {
    . = ALIGN(4096);
    _l_buff_start = .;
     *(.L_buff*)
     . = ALIGN(4096);
    _l_buff_end = .;
  }

  .m_buff : {
    . = ALIGN(4096);
    _m_buff_start = .;
     *(.M_buff*)
     . = ALIGN(4096);
    _m_buff_end = .;
  }
  .shared_buff  :{
    . = ALIGN(4096);
    _shared_buff_start = .;
     *(.shared_buff*)
     . = ALIGN(4096);
    _shared_buff_end = .;
  }
  
  .ro_trampoline_data : {
    . = ALIGN(4096);
    _ro_trampoline_data_start = .;
     *(.ro_trampoline_data*)
     . = ALIGN(4096);
    _ro_trampoline_data_end = .;
  }
  
    .trampoline_data : {
    . = ALIGN(4096);
    _trampoline_data_start = .;
     *(.trampoline_data*)
     . = ALIGN(4096);
    _trampoline_data_end = .;
  }


  . = .;
  __bss_start = .;
  .bss            :
  {
   *(.dynbss)
   *(.bss .bss.* .gnu.linkonce.b.*)
   *(COMMON)
   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections.
      FIXME: Why do we need it? When there is no .bss section, we do not
      pad the .data section.  */
   . = ALIGN(. != 0 ? 64 / 8 : 1);
  }
  .lbss   :
  {
    *(.dynlbss)
    *(.lbss .lbss.* .gnu.linkonce.lb.*)
    *(LARGE_COMMON)
  }
  . = ALIGN(64 / 8);
  . = SEGMENT_START("ldata-segment", .);
  .lrodata   ALIGN(CONSTANT (MAXPAGESIZE)) + (. & (CONSTANT (MAXPAGESIZE) - 1)) :
  {
    *(.lrodata .lrodata.* .gnu.linkonce.lr.*)
  }
  .ldata   ALIGN(CONSTANT (MAXPAGESIZE)) + (. & (CONSTANT (MAXPAGESIZE) - 1)) :
  {
    *(.ldata .ldata.* .gnu.linkonce.l.*)
    . = ALIGN(. != 0 ? 64 / 8 : 1);
  }
  . = ALIGN(64 / 8);
  _end = .; PROVIDE (end = .);
  . = DATA_SEGMENT_END (.);
  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  .gnu.build.attributes : { *(.gnu.build.attributes .gnu.build.attributes.*) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* DWARF 3 */
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges   0 : { *(.debug_ranges) }
  /* DWARF Extension.  */
  .debug_macro    0 : { *(.debug_macro) }
  .debug_addr     0 : { *(.debug_addr) }
  .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
  /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) }
}