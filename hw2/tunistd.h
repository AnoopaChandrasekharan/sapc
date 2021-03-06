#ifndef TUNISTD_H
#define TUNISTD_H

#include "tty_public.h"



/* read nchar bytes into buf from dev */
int read(int dev, char *buf, int nchar);
/* write nchar bytes from buf to dev */
int write(int dev, char *buf, int nchar);
/* misc. device functions */
int control(int dev, int fncode, int val);
//int exit(int exitcode);
#endif
