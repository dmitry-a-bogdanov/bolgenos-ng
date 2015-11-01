target remote localhost:1234
display /i ($cs*16)+$pc
set architecture i8086
hbr *0x7c00
