# asm startup file
# very first module in load
.globl _ustart 
           
_ustart:     
           call _main
           pushl %eax             # call C startup, which calls user main
           call _exit

