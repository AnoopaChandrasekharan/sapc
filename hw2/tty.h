/*********************************************************************
*
*       file:           tty.h
*       author:         betty o'neil
*
*       private header file for tty drivers
*       apps should not include this header
*/
#include "queue/queue.h"
#ifndef TTY_H
#define TTY_H

#define MAXQ 6          /* declare queue size*/

struct tty {
  int echoflag;
  Queue *rqu;            /* pointer to receiver queue for typeahead chars */
  Queue *tqu;            /* pointer to transmitter queue for output chars */
};

extern struct tty ttytab[];

/* tty-specific device functions */
void ttyinit(int dev);
int ttyread(int dev, char *buf, int nchar);
int ttywrite(int dev, char *buf, int nchar);
int ttycontrol(int dev, int fncode, int val);

#endif 
