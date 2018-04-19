//
// Created by Parker Jones on 4/4/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "binomial.h"
#include "dll.h"
#include "queue.h"

#define DEBUG 0

#if DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif


typedef struct binomialNode BINOMIALNODE;

static void displayBINOMIALNODE(void *node, FILE *fp);

BINOMIALNODE *newBINOMIALNODE(void (*display)(void *, FILE *),
                              int (*compare)(void *, void *),
                              void (*free)(void *),
                              void *value);

static void freeBINOMIALNODE(BINOMIALNODE *b, void (*freer)(void *));

static BINOMIALNODE *bubbleUp(BINOMIAL *b, BINOMIALNODE *n);

static BINOMIALNODE *combine(BINOMIAL *b, BINOMIALNODE *x, BINOMIALNODE *y);

static void consolodate(BINOMIAL *b);


struct binomialNode {

    BINOMIALNODE *parent;
    DLL *children;
    void *value;

    void (*display)(void *, FILE *);

    int (*compare)(void *, void *);

    void (*free)(void *);

};


struct binomial {
    void (*display)(void *, FILE *);

    int (*compare)(void *, void *);

    void (*update)(void *, void *);

    void (*free)(void *);

    DLL *rootlist;
    int size;
    BINOMIALNODE *extreme;

};


BINOMIALNODE *newBINOMIALNODE(void (*display)(void *, FILE *),
                              int (*compare)(void *, void *),
                              void (*free)(void *),
                              void *value) {
    BINOMIALNODE *b = malloc(sizeof(BINOMIALNODE));
    b->value = value;
    b->parent = 0;
    b->display = display;
    b->compare = compare;
    b->free = free;
    b->children = newDLL(displayBINOMIALNODE, free);

    if (DEBUG) {
        printf("_binomialNode : newBINOMIALNODE -  Created BinomialNode with value ");
        b->display(b->value, stdout);
        printf("\n");
    }

    return b;
}

static void freeBINOMIALNODE(BINOMIALNODE *b, void (*freer)(void *)) {
    freeDLL(b->children);
    if (freer) freer(b->value);
    free(b);
}


static void displayBINOMIALNODE(void *node, FILE *fp) {
    BINOMIALNODE *n = node;
    void *value = n->value;
    n->display(value, fp);
    dprintf("[%d]", sizeDLL(n->children));
}


BINOMIAL *newBINOMIAL(
        void (*display)(void *, FILE *),
        int (*compare)(void *, void *),
        void (*update)(void *, void *),
        void (*free)(void *)) {
    dprintf("_binomial : newBINOMIAL -  \n");

    BINOMIAL *b = malloc(sizeof(BINOMIAL));

    b->rootlist = newDLL(displayBINOMIALNODE, free);

    b->display = display;
    b->compare = compare;
    b->update = update;
    b->free = free;
    b->size = 0;
    b->extreme = 0;

    return b;
}

//done
static BINOMIALNODE *bubbleUp(BINOMIAL *b, BINOMIALNODE *n) {
    BINOMIALNODE *p = n->parent;
    if (n->parent == 0) {
        if (b->compare(n->value, b->extreme->value) >= 0) b->extreme = n->value;
        return n;
    }
    if (b->compare(n->value, p->value) <= 0) return n;
    if (b->update) b->update(n->value, p);
    if (b->update) b->update(p->value, n);
    void *temp = n->value;
    n->value = p->value;
    p->value = temp;
    return bubbleUp(b, p);
}


static BINOMIALNODE *combine(BINOMIAL *b, BINOMIALNODE *x, BINOMIALNODE *y) {

    if (b->compare(x->value, y->value) < 0) {
        if (DEBUG) {
            printf("_binomial : combine -  inserting Y:");
            displayBINOMIALNODE(y, stdout);
            printf(" as child of  X:");
            displayBINOMIALNODE(x, stdout);
            printf("\n");
        }
        insertDLL(x->children, 0, y);
        y->parent = x;
        return x;
    } else {
        if (DEBUG) {
            printf("_binomial : combine -  inserting X:");
            displayBINOMIALNODE(x, stdout);
            printf(" as child of  Y:");
            displayBINOMIALNODE(y, stdout);
            printf("\n");
        }
        insertDLL(y->children, 0, x);
        x->parent = y;
        return y;
    }
}

/*

static void updateConsolodationArray(BINOMIALNODE *consolodationArray[], BINOMIALNODE *node, BINOMIAL *b) {

    int degree = sizeDLL(node->children);
    while (consolodationArray[degree] != 0) {
        if (DEBUG) {
            dprintf("_binomial : updateConsolodationArray -  collision detected. combining X:");
            displayBINOMIALNODE(node, stdout);
            printf(" and Y:");
            displayBINOMIALNODE(consolodationArray[degree], stdout);
            printf("\n");
        }
        combine(b, node, consolodationArray[degree]);
        consolodationArray[degree] = 0;
        degree++;
    }
    consolodationArray[degree] = node;
}
*/

//done?
static void consolodate(BINOMIAL *b) {

    //the size of the rootlist
    int size = (int) (log(b->size) / log(2) + 0.0000001) + 1;

    dprintf("_binomial : consolodate -  Consolodating! Rootlist size is %d\n", size);


    //initialize array to 0
    BINOMIALNODE *consolodationArray[size];
    for (int i = 0; i < size; i++) {
        consolodationArray[i] = 0;
    }


    //go throught the rootlist and merge things in the consolodation array
    while (sizeDLL(b->rootlist)) {
        BINOMIALNODE *node = removeDLL(b->rootlist, 0);
        dprintf("_binomial : consolodate -  adding to the consolodation array\n");
        //updateConsolodationArray(consolodationArray, node, b);

        int degree = sizeDLL(node->children);
        while (consolodationArray[degree] != 0) {
            if (DEBUG) {
                dprintf("_binomial : updateConsolodationArray -  collision detected. combining X:");
                displayBINOMIALNODE(node, stdout);
                printf(" and Y:");
                displayBINOMIALNODE(consolodationArray[degree], stdout);
                printf("\n");
            }
            node = combine(b, node, consolodationArray[degree]);
            consolodationArray[degree] = 0;
            degree++;
        }
        if (DEBUG) {
            printf("_binomial : consolodate -  node getting assigned is ");
            displayBINOMIALNODE(node, stdout);
            printf("\n");
        }
        consolodationArray[degree] = node;

    }


    b->extreme = 0;
    //put stuff back in the rootlist

    for (int i = 0; i < size; i++) {
        if (consolodationArray[i]) {

            if (DEBUG) {
                printf("_binomial : consolodate -  inserting ");
                displayBINOMIALNODE(consolodationArray[i], stdout);
                printf("\n");
            }

            insertDLL(b->rootlist, sizeDLL(b->rootlist), consolodationArray[i]);
            //if extreme is null or if consolodationArray[i] is more extreme
            if (!b->extreme || b->compare(b->extreme->value, consolodationArray[i]->value) > 0) {
                b->extreme = consolodationArray[i];
            }
        }
    }
}

//done?
void *insertBINOMIAL(BINOMIAL *b, void *value) {
    //takes in the generic value

    BINOMIALNODE *n = newBINOMIALNODE(b->display, b->compare, b->free, value);

    dprintf("_binomial : insertBINOMIAL -  inserting ");
    if (DEBUG) {
        displayBINOMIALNODE(n, stdout);
        dprintf("\n");
    }

    insertDLL(b->rootlist, sizeDLL(b->rootlist), n);

    b->size++;

    if (DEBUG) {
        dprintf("_binomial : insertBINOMIAL -  rootlist is now ");
        displayDLLdebug(b->rootlist, stdout);
        printf("\n");
    }

    consolodate(b);

    if (DEBUG) {
        dprintf("_binomial : insertBINOMIAL -  insertion finished with rootlist :  ");
        displayDLLdebug(b->rootlist, stdout);
        printf("\n");
    }

    return n;
}

//done?
int sizeBINOMIAL(BINOMIAL *b) {
    dprintf("_binomial : sizeBINOMIAL -  \n");
    return b->size;
}

//done?
void unionBINOMIAL(BINOMIAL *a, BINOMIAL *b) {
    dprintf("_binomial : unionBINOMIAL -  \n");

    unionDLL(a->rootlist, b->rootlist);
    a->size += b->size;

    b->extreme = 0;

    consolodate(a);
}


void deleteBINOMIAL(BINOMIAL *b, void *node) {
    dprintf("_binomial : deleteBINOMIAL -  \n");
    BINOMIALNODE *n = node;
    decreaseKeyBINOMIAL(b, n, NULL);
    b->free(extractBINOMIAL(b));
}


//done?
void decreaseKeyBINOMIAL(BINOMIAL *b, void *node, void *value) {
    dprintf("_binomial : decreaseKeyBINOMIAL -  \n");
    BINOMIALNODE *n = (BINOMIALNODE *) node;

    n->value = value;
    bubbleUp(b, n);
}

//done
void *peekBINOMIAL(BINOMIAL *b) {
    dprintf("_binomial : peekBINOMIAL -  \n");
    return b->extreme->value;
}


//done?
void *extractBINOMIAL(BINOMIAL *b) {
    dprintf("_binomial : extractBINOMIAL -  \n");
    BINOMIALNODE *y = b->extreme;
    firstDLL(b->rootlist);
    //removes the extreme value from the root list
    for (int i = 0; sizeDLL(b->rootlist); i++) {
        if (currentDLL(b->rootlist)) {
            if (b->compare(y->value, ((BINOMIALNODE *) currentDLL(b->rootlist))->value) == 0) {
                removeDLL(b->rootlist, i);
                break;
            }
        }
        nextDLL(b->rootlist);
    }


    //sets the parent of each of them to 0
    firstDLL(y->children);
    for (int i = 0; i < sizeDLL(y->children); i++) {
        BINOMIALNODE *currnode = currentDLL(y->children);
        currnode->parent = 0;
        nextDLL(y->children);
    }


    unionDLL(b->rootlist, y->children);
    consolodate(b);

    b->size--;

    void *temp = y->value;

    if (DEBUG) {
        printf("_binomial : extractBINOMIAL -  extracted: [");
        displayBINOMIALNODE(y, stdout);
        printf("]\n");
    }

    freeBINOMIALNODE(y, 0);

    return temp;

}

//done?
void statisticsBINOMIAL(BINOMIAL *b, FILE *fp) {
    dprintf("_binomial : statisticsBINOMIAL -  \n");
    printf("size: %d, rootlist size: %d\n", b->size, sizeDLL(b->rootlist));
    if (b->size) {
        printf("extreme: ");
        b->display(b->extreme->value, fp);
        printf("\n");
    }
}


void displayBINOMIAL(BINOMIAL *b, FILE *fp) {

    int size = (int) (log(b->size) / log(2) + 0.000001) + 1;
    BINOMIALNODE *nodeList[size];

    //initialize it to 0
    for (int x = 0; x < size; x++) { nodeList[x] = 0; }

    dprintf("_binomial : displayBINOMIAL -  \n");
    firstDLL(b->rootlist);
    for (int x = 0; x < sizeDLL(b->rootlist); x++) {
        BINOMIALNODE *n = currentDLL(b->rootlist);
        nodeList[sizeDLL(n->children)] = n;
        nextDLL(b->rootlist);
    }
    firstDLL(b->rootlist);

    printf("rootlist: ");

    for (int x = 0; x < size; x++) {
        if (nodeList[x]) {
            displayBINOMIALNODE(nodeList[x], fp);
            if (b->compare(nodeList[x]->value, b->extreme->value) == 0) printf("*");
        } else {
            printf("NULL");
        }
        if (x != size - 1) printf(" ");
    }
    printf("\n");
}


static void enqueueDLL(QUEUE *items, DLL *list) {
    firstDLL(list);
    for (int i = 0; i < sizeDLL(list); i++) {
        enqueue(items, currentDLL(list));
        nextDLL(list);
    }
    firstDLL(list);
}


void displayBINOMIALdebug(BINOMIAL *b, FILE *fp) {
    dprintf("_binomial : displayBINOMIALdebug -  \n");

    QUEUE *items = newQUEUE(displayBINOMIALNODE, 0);

    enqueueDLL(items, b->rootlist);

    int nodes = sizeQUEUE(items);

    while (nodes > 0) {

        printf("{{");
        while (nodes > 0) {

            BINOMIALNODE *n = dequeue(items);

            displayBINOMIALNODE(n, fp);

            enqueueDLL(items, n->children);

            nodes--;

            if (nodes != 0)printf(",");
        }

        nodes = sizeQUEUE(items);
        printf("}}\n");
    }
    freeQUEUE(items);
}


void freeBINOMIAL(BINOMIAL *b) {
    dprintf("_binomial : freeBINOMIAL -  \n");

}

