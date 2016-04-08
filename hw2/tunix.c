#include<stdio.h>
#include <cpu.h>
#include <gates.h>
#include <pic.h>
#include <serial.h>
#include "tsyscall.h"
#include "tunistd.h"
#include "io_public.h"

void k_init(void);
void syscallc(int user_eax, int devcode, char *buf , int bufflen);
void debug_set_trap_gate(int n, IntHandler *inthand_addr, int debug);
void set_trap_gate(int n, IntHandler *inthand_addr);
int  sysexit(int);
int sysread(int dev, char *buf, int nchar);
int syswrite(int dev, char *buf, int nchar);
int syscontrol(int dev, int fncode, int val);

extern IntHandler syscall;
extern void locate_idt(unsigned int *limitp,char **idtp);
extern void ustart(void);
extern void last(void);

void k_init()
{
  cli();
  ioinit();
  set_trap_gate(0x80, &syscall);   /* SET THE TRAP GATE*/
  ustart();                        /* to call main in crt0.s */
}

void syscallc( int user_eax, int devcode, char *buf , int bufflen)
{
  int syscallnum= user_eax;
  //kprintf("\nsyscall num: %d",syscallnum);
  //kprintf("\n in syscallc \n devcode:%d buf : %s buflen: %d",devcode , buf, bufflen);
  switch(syscallnum)
  {
      case S_WRITE :
          user_eax = syswrite(devcode, buf, bufflen);
          break;
      case S_EXIT :
          user_eax= sysexit(devcode);
          break;
      case S_READ:
          user_eax= sysread(devcode, buf , bufflen);
          break;
   
  } 
}
int sysexit(int exit_code)
{
cli();
last();
return 0;
}

void set_trap_gate(int n, IntHandler *inthand_addr)
{
  debug_set_trap_gate(n, inthand_addr, 0);
}

/* write the nth idt descriptor as an interrupt gate to inthand_addr */
void debug_set_trap_gate(int n, IntHandler *inthand_addr, int debug)
{
  char *idt_LAaddr;		/* linear address of IDT */
  char *idt_VAaddr;		/* virtual address of IDT */
  Gate_descriptor *idt, *desc;
  unsigned int limit = 0;

  if (debug)
    kprintf("Calling locate_idt to do sidt instruction...\n");
  locate_idt(&limit,&idt_LAaddr);
  /* convert to virtual address, i.e., ordinary address */
  idt_VAaddr = idt_LAaddr - KERNEL_BASE_LA;  /* usable address of IDT */
  /* convert to a typed pointer, for an array of Gate_descriptor */
  idt = (Gate_descriptor *)idt_VAaddr;
  if (debug)
    kprintf("Found idt at %x, lim %x\n",idt, limit);
  desc = &idt[n];		/* select nth descriptor in idt table */
  /* fill in descriptor */
  if (debug)
    kprintf("Filling in desc at %x with addr %x\n",(unsigned int)desc,
	   (unsigned int)inthand_addr);
  desc->selector = KERNEL_CS;	/* CS seg selector for int. handler */
  desc->addr_hi = ((unsigned int)inthand_addr)>>16; /* CS seg offset of inthand  */
  desc->addr_lo = ((unsigned int)inthand_addr)&0xffff;
  desc->flags = GATE_P|GATE_DPL_KERNEL|GATE_TRAPGATE; /* valid, interrupt */
  desc->zero = 0;
}
