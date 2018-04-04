//
// Created by Parker Jones on 2/14/18.
//

#ifndef ASSIGN1_STRING_H
#define ASSIGN1_STRING_H

#include <stdio.h>

typedef struct String STRING;

extern STRING *newSTRING(char *);
extern char *getSTRING(STRING *);
extern char *setSTRING(STRING *,char *);
extern int compareSTRINGmax(void *,void *);
extern int compareSTRINGmin(void *,void *);
extern int compareSTRING(void *, void *);
extern void displaySTRING(void *,FILE *);
extern void freeSTRING(void *);


#endif //ASSIGN1_STRING_H
