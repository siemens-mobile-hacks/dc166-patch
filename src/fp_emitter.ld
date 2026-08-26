SECTIONS
{
  . = 0x0050e0c0;
  .text : { *(.text) }
  /DISCARD/ : { *(.note*) *(.comment*) *(.eh_frame*) }
}
