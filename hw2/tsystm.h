#ifndef TSYSTM_H
#define TSYSTM_H

void ioinit(void);
int sysread(int dev, char *buf, int nchar);
int syswrite(int dev, char *buf, int nchar);
int syscontrol(int dev, int fncode, int val);
#endif
