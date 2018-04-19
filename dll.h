//
// Given on the Beastie.cs.ua.edu website
//
#ifndef __DLL_INCLUDED__
#define __DLL_INCLUDED__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct dll DLL;

extern DLL *newDLL(void (*d)(void *,FILE *),void (*f)(void *));
extern void *insertDLL(DLL *items,int index,void *value);
extern void *removeDLL(DLL *items,int index);
extern void unionDLL(DLL *recipient,DLL *donor);
extern void *getDLL(DLL *items,int index);
extern void *setDLL(DLL *items,int index,void *value);
extern int sizeDLL(DLL *items);
extern void displayDLL(DLL *items,FILE *);
extern void displayDLLdebug(DLL *items,FILE *);
extern void freeDLL(DLL *items);

extern void *removeDLLnode(DLL *items, void *node);
extern void removeDLLall(DLL *items);
extern void firstDLL(DLL *items);
extern void lastDLL(DLL *items);
extern int moreDLL(DLL *items);
extern void nextDLL(DLL *items);
extern void prevDLL(DLL *items);
extern void *currentDLL(DLL *items);


#endif

