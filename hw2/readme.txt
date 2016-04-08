hw2 fiolder contains 
io_public.h--applications-level header: dev indep i/o API protos
tty_public.h--app-level header: tty-specific info

ioconf.h--def of device struct, top-level info on device
ioconf.c--actual device table, array of device structs, one for each device
io.c--top-level device-independent calls: ioinit, read, write, control

Device type tty (for COM lines):
tty.h--internal header file
tty.c--tty device driver, i.e., device-specific code
tty_secondary_method.c: using the TX interrupt acknowledge (you can ignore this)

makefile--  make testio.lnx   builds testio.c with io package

uprog.c--applications-level program exercising io package

ulib.s: start on user library for syscalls
crt0.s: calls main , reinitialize stack and exit
startup0.s: starting module
sysentry.s: contains syscall trap handler envelope
startup.c: empty now, you start from $pclibsrc/startup.c, edit to call kernel init, not main
tsyscall.h: this file define system call numbers 
tunistd.h: contains system call header file for user programs
tunix.c: contains kernel init, syscallc, sysexit, etc. 