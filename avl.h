//
// Created by Parker Jones on 2/28/18.
//
/*** AVL binary search tree class ***/

#ifndef __AVL_INCLUDED__
#define __AVL_INCLUDED__

#include <stdio.h>

typedef struct avl AVL;

extern AVL *newAVL(
        void (*)(void *,FILE *),           //display
        int (*)(void *,void *),            //comparator
        void (*)(void *));                 //freeing function
extern void insertAVL(AVL *,void *);
extern int findAVLcount(AVL *,void *);
extern void *findAVL(AVL *,void *);
extern void *deleteAVL(AVL *,void *);
extern int sizeAVL(AVL *);
extern int duplicatesAVL(AVL *);
extern void statisticsAVL(AVL *,FILE *);
extern void displayAVL(AVL *,FILE *);
extern void displayAVLdebug(AVL *,FILE *);
extern void freeAVL(AVL *);

#endif