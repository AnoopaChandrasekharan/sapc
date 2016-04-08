/*********************************************************************
*
*       file:           testio.c
*       author:         betty o'neil
*       edited by:      Anoopa Chandrasekharan Jayasree
*
*       test program (application code) for 
*       the device-independent i/o package for SAPC
*
*/

#include <stdio.h>              /* for lib's device # defs, protos */
#include "io_public.h"		/* for our packages devs, API prototypes */
#define BUFLEN 80



int main(void)
{
  char buf[BUFLEN];
  int got, i, lib_console_dev, ldev;

  /* Determine the SAPC's "console" device, the serial port for user i/o */
  lib_console_dev = sys_get_console_dev();  /* SAPC support lib fn */
  switch(lib_console_dev) {
  case COM1: ldev = TTY0;	/* convert to our dev #'s */
    break;
  case COM2: ldev = TTY1;
    break;
  default: printf("Unknown console device\n");
    return 1;
  }
  kprintf("Running with device TTY%d\n",ldev);
  /* Now have a usable device to talk to with i/o package-- */

  
  kprintf("\nTrying system call write(4 chars)...\n");
  got = write(ldev,"hi!\n",4);
  kprintf("write of 4 returned %d\n",got);
  
  kprintf("Trying longer system call write (9 chars)\n");
  got = write(ldev, "abcdefghi", 9);
  kprintf("write of 9 returned %d\n",got);
  

  for (i = 0; i < BUFLEN; i++)
      buf[i] = 'A'+ i/2;
  kprintf("\nTrying write of %d-char string...\n", BUFLEN);
  got = write(ldev, buf, BUFLEN); /* will write 80 chars*/
  kprintf("\nwrite returned %d\n", got);
  

  kprintf("\nType 10 chars input to test typeahead while looping for delay...\n");
  
  got = read(ldev, buf, 10); /* will read 10 chars*/	
  kprintf("\nGot %d chars into buf. Trying write of buf...\n", got);
  write(ldev, buf, got);

  kprintf("\nTrying another 10 chars read right away...\n");
  got = read(ldev, buf, 10);	/* will wait for input */
  kprintf("\nGot %d chars on second read\n",got);
  
  if (got == 0) 
      kprintf("nothing in buffer\n");
  else 
      write(ldev, buf, got);	/* will write 10 chars*/

  kprintf("\n\nNow turning echo off--\n");
  
  kprintf("\nType 20 chars input, note lack of echoes...\n");
  got = read(ldev, buf, 20);
  kprintf("\nTrying write of buf...\n");
  write(ldev, buf, got);
  kprintf("\nAsked for 20 characters; got %d\n", got);
  return 0;
}


