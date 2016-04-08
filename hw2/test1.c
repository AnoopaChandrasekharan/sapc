#include <stdio.h> 
#include "tunistd.h"
#include "ioconf.h"
#include "tty_public.h"

int main(void);
int main()
{
char buf[10];
int got = 0;
kprintf("\nWRITE system call");
got = write(TTY0,"h3i!!",5 );          /* call write function */
kprintf("\n wrote %d chars", got);
kprintf("\nREAD system call");  /* call read function */
got = read(TTY0,buf,10);
kprintf("\n Read %d chars", got);
return 0;
}

