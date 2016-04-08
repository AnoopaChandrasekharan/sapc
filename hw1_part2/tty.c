/*********************************************************************
*
*       file:           tty.c
*       date:           03/02/2016
*       edited by :     Anoopa Chandrasekharan Jayasree
*       author:         betty o'neil
*       tty driver--device-specific routines for ttys 
*
*/
#include <stdio.h>               /* for kprintf prototype */
#include <serial.h>
#include <cpu.h>
#include <pic.h>
#include "ioconf.h"
#include "tty_public.h"
#include "tty.h"
#include "queue.h"
#define MAXQ 6                   /* macro for size of queue */
struct tty ttytab[NTTYS];        /* software params/data for each SLU dev */

/* tell C about the assembler shell routines */
extern void irq3inthand(void), irq4inthand(void);

/* C part of interrupt handlers--specific names called by the assembler code */
extern void irq3inthandc(void), irq4inthandc(void); 

/* the common code for the two interrupt handlers */                         
static void irqinthandc(int dev); 

/*====================================================================
*
*       tty specific initialization routine for COM devices
*
*/

void ttyinit(int dev)
{
  int baseport, ret;                        /* initialize baseport and return val variable for queue operations*/
  struct tty *tty;		                      /* ptr to tty software params/data block */
  baseport = devtab[dev].dvbaseport;        /* pick up hardware addr */
  tty = (struct tty *)devtab[dev].dvdata;   /* and software params struct */
  cli();
  if (baseport == COM1_BASE) 
  {
                                            /* arm interrupts by installing interrupt vector address */
    set_intr_gate(COM1_IRQ+IRQ_TO_INT_N_SHIFT, &irq4inthand);
    pic_enable_irq(COM1_IRQ);
  } 
  else if (baseport == COM2_BASE) 
  {
                                            /* arm interrupts by installing interrupt vector address */
    set_intr_gate(COM2_IRQ+IRQ_TO_INT_N_SHIFT, &irq3inthand);
    pic_enable_irq(COM2_IRQ);
  } 
  else 
  {
    kprintf("Bad TTY device table entry, dev %d\n", dev);
    return;			                            /* give up */
  }
  tty->echoflag = 1;	                                   /* default to echoing */
  ret = init_queue((tty->rqu), MAXQ);                    /* init RX queue */
  ret = init_queue((tty->tqu), MAXQ);                    /* init  TX queue */
                                                         
  outpt(baseport+UART_IER, UART_IER_RDI|UART_IER_THRI);  /* enable interrupts on receiver and transmitter */
}


/*====================================================================
*
*       Useful function when emptying/filling the read/write buffers
*
*/

#define min(x,y) (x < y ? x : y)


/*====================================================================
*
*       tty-specific read routine for TTY devices
*
*/

int ttyread(int dev, char *buf, int nchar)
{
  int baseport, saved_eflags;
  struct tty *tty;
  int i=0;
  baseport = devtab[dev].dvbaseport;        /* hardware addr from devtab */
  tty = (struct tty *)devtab[dev].dvdata;   /* software data for line */
  
  while (i < nchar)                         /* loops till all chars are dequeued by cpu */
  {
    saved_eflags=get_eflags();              /*  saving cpu flag status */
    cli();                                  /* disable cpu ints */
    if(queuecount(tty->rqu))                /*if anything in rx queue */
      buf[i++]= dequeue((tty->rqu));        /*dequeue char from receiver queue */
    set_eflags(saved_eflags);               /*restoring flag status */
  }
  return nchar;                             
}
  
  
  


/*====================================================================
*
*       tty-specific write routine for SAPC devices
*       (cs444: note that the buffer tbuf is superfluous in this code, but
*        it still gives you a hint as to what needs to be done for
*        the interrupt-driven case)
*
*/

int ttywrite(int dev, char *buf, int nchar)
{
  int baseport, saved_eflags;
  struct tty *tty;
  int i=0;
  baseport = devtab[dev].dvbaseport;               /* hardware addr from devtab */
  tty = (struct tty *)devtab[dev].dvdata;          /* software data for line */
  saved_eflags=get_eflags();                       /* cpu flags saved */
  cli();                                           /* disable interrupts */
  outpt( baseport+UART_IER, UART_IER_RDI);         /* enable receiver interrupt */
  
  while ((i <nchar) &&  (enqueue((tty->tqu), buf[i]) != FULLQUE ))     /* loop for enqueue till the queue is full */
    i++;
  
  sti();                                                               /* allow cpu interrupts */
  outpt( baseport+UART_IER, UART_IER_RDI | UART_IER_THRI);             /*enable receiver and transmitter interrupts */
  set_eflags(saved_eflags);                                            /*restoring flag status*/
    
  while (i<nchar)                                                      /* loop for enqueue of remaining nchar */
  {
    cli();
    outpt( baseport+UART_IER, UART_IER_RDI);                           /* enable receiver interrupt*/
    if(enqueue((tty->tqu), buf[i]) != FULLQUE)                         /* check if queue full */
      i++;
    outpt( baseport+UART_IER, UART_IER_RDI | UART_IER_THRI);           /* enable Rx and Tx interrupts */
    set_eflags(saved_eflags);                                          /* restore flags*/
  } 
  return nchar;
}
   

/*====================================================================
*
*       tty-specific control routine for TTY devices
*
*/

int ttycontrol(int dev, int fncode, int val)
{
  struct tty *this_tty = (struct tty *)(devtab[dev].dvdata);
  if (fncode == ECHOCONTROL)
    this_tty->echoflag = val;
  else return -1;
  return 0;
}



/*====================================================================
*
*       tty-specific interrupt routine for COM ports
*
*   Since interrupt handlers don't have parameters, we have two different
*   handlers.  However, all the common code has been placed in a helper 
*   function.
*/
  
void irq4inthandc()
{
  irqinthandc(TTY0);
}                              
  
void irq3inthandc()
{
  irqinthandc(TTY1);
}                              

void irqinthandc(int dev){  
  int ch, f;                                         /* declare variable for int identification*/
  struct tty *tty = (struct tty *)(devtab[dev].dvdata);
  int baseport = devtab[dev].dvbaseport;             /* hardware i/o port */
  pic_end_int();                                     /* notify PIC that its part is done */
   
  f=inpt(baseport+UART_IIR);                         /* interrupt identification register */
  
  switch (f&UART_IIR_ID)                             /* mask 2-bit id field*/
  {
    case UART_IIR_RDI:                               /* if receiver interrupt */
      ch = inpt(baseport+UART_RX); 	                 /* read char, ack the device */
      enqueue((tty->rqu), ch);                       /* enqueue char to input queue */
      break;
  
    case UART_IIR_THRI:                              /* if transmitter interrupt */
      if(!emptyqueue(tty->tqu))                      /* if Tx queue not empty */
        outpt( baseport+UART_TX , dequeue(tty->tqu));/* output char to com port*/
      break;
  }
}
