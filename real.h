#ifndef __REAL_INCLUDED__
#define __REAL_INCLUDED__

#include <stdio.h>

typedef struct REAL REAL;

extern REAL *newREAL(double);
extern double getREAL(REAL *);
extern double setREAL(REAL *,double);
extern int compareREALmax(void *,void *);
extern int compareREALmin(void *,void *);
extern int compareREALmin(void *v, void *w);
extern void displayREAL(void *,FILE *);
extern void freeREAL(void *);

#endif
