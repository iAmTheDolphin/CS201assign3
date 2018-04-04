#include "dll.h"

//given by Dr. Lusth on the beastie website
//--------------------------------------------------------
//region Node
typedef struct node NODE;

struct node {
    void *value;
    NODE *next;
    NODE *last;
};

static NODE *newNODEdll(void *v, NODE *n, NODE *l) {
    NODE *p = malloc(sizeof(NODE));
    if (p == 0) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    p->value = v;
    p->next = n;
    p->last = l;
    return p;
}

/* accessors */

static void *getNODEvalue(NODE *n) { return n->value; }

static NODE *getNODEnext(NODE *n) { return n->next; }

static NODE *getNODElast(NODE *n) { return n->last; }

/* mutators */

static void setNODEvalue(NODE *n, void *v) { n->value = v; }

static void setNODEnext(NODE *n, NODE *p) { n->next = p; }

static void setNODElast(NODE *n, NODE *p) { n->last = p; }

/* visualizers */
/*
static void displayNODE(NODE *n, FILE *fp, void (*d)(FILE *, void *)) {
    fprintf(fp, "[[");
    d(fp, n->value);
    fprintf(fp, "]]");
}

static void displayNODEdebug(NODE *n, FILE *fp, void (*d)(FILE *, void *)) {
    fprintf(fp, "[[");
    d(fp, n->value);
    fprintf(fp, "@%p->%p]]", n, n->next);
}
*/
static void
freeNODE(NODE *n, void (*release)(void *)) {
    if (release != 0 && n->value != 0) release(n->value);
    free(n);
}
//endregion





//
// Created by Parker Jones on 1/15/18.
//


int debugDLL = 0;
//if(debugDLL) printf("_DLL - \n");

struct dll {
    NODE *head;
    NODE *tail;
    int size;
    void (*display)(void *, FILE *);
    void (*free)(void *);
};

DLL *newDLL(void (*d)(void *, FILE *), void (*f)(void *)) {
    DLL *items = malloc(sizeof(DLL));
    assert(items != 0);
    items->head = 0;
    items->tail = 0;
    items->size = 0;
    items->display = d;
    items->free = f;
    return items;
}

/*
//getting the node before the index passed in is an arifact from
//when I wrote SLL and brought it over. while it is not the most
//logical solution to this problem, strictly speaking, it is the
//solution that had already been implemented in the code, thus I
//considered it dangerous to change since almost every method
//was already accounting for recieving the node before the index
 */
static NODE *getNodeBefore(DLL *items, int index) {
    if(index <= items->size / 2) {
        NODE *n = items->head;
        for (int x = 1; x < index; x++) {
            n = getNODEnext(n);
        }
        return n;
    }
    else { //allows access to the end of the list in constant time
        NODE *n = items->tail;
        for (int x = items->size -1; x > index-1; x--) {
            n = getNODElast(n);
        }
        return n;
    }
}

void insertDLL(DLL *items, int index, void *value) {
    if (debugDLL) printf("_DLL - inserting into DLL at index %d\n", index);
    assert(index <= items->size);
    assert(index >= 0);
    NODE *n;
    if (items->size == 0) {
        n = newNODEdll(value, 0, 0);
        items->head = n;
        items->tail = n;
        if (debugDLL) printf("_DLL - - first element added to list\n");
    } else if (index == 0) {
        n = newNODEdll(value, items->head, 0);
        setNODElast(items->head, n);
        items->head = n;
        if (debugDLL) printf("_DLL - - element added to front\n");
    } else {
        NODE *b4 = getNodeBefore(items, index);
        NODE *next = getNODEnext(b4);
        n = newNODEdll(value, next, b4);
        if (next == 0) items->tail = n;
        else setNODElast(next, n);
        setNODEnext(b4, n);
        if (debugDLL) printf("_DLL - - element added to "
                                     "middle/end of list\n");
    }
    items->size++;
    if (debugDLL) printf("_DLL - - size of list after "
                                 "insertion : %d\n", items->size);
}

void *removeDLL(DLL *items, int index) {
    assert(index < items->size);
    assert(index >= 0);
    assert(items->size > 0);

    NODE *n = getNodeBefore(items, index + 1);
    NODE *last = getNODElast(n);
    NODE *next = getNODEnext(n);
    void *value = getNODEvalue(n);

    if (items->size == 1) { //removing the only node
        items->head = 0;
        items->tail = 0;
    } else if (index == 0) { //removing from index 0
        setNODElast(next, 0);
        items->head = next;
    } else if (index == items->size - 1) { //removing from the last index
        setNODEnext(last, 0);
        items->tail = last;
    } else {                //removing from somewhere in the middle
        setNODEnext(last, next);
        setNODElast(next, last);
    }
    free(n);
    items->size--;
    if (debugDLL) printf("_DLL - removed item at index %d: remaining "
                                 "items: %d\n", index, items->size);

    return value;
}

void unionDLL(DLL *recipient, DLL *donor) {
    if (debugDLL) printf("_DLL - union of recipient size:%i and donor "
                                 "size:%i \n", recipient->size, donor->size);
    if (recipient->size == 0) {
        recipient->head = donor->head;
        recipient->tail = donor->tail;
    } else if (donor->size > 0){
        setNODEnext(recipient->tail, donor->head);
        if (donor->head != 0) setNODElast(donor->head, recipient->tail);
        recipient->tail = donor->tail;
    }
    recipient->size += donor->size;
    donor->head = donor->tail = 0;
    donor->size = 0;
}

void *getDLL(DLL *items, int index) {
    if (debugDLL) printf("_DLL - getting value from index %i\n", index);
    assert(index >= 0);
    assert(index < items->size);
    if (index == 0) {
        return getNODEvalue(items->head);
    } else if (index == items->size - 1) {
        return getNODEvalue(items->tail);
    } else {
        return getNODEvalue(getNODEnext(getNodeBefore(items, index)));
    }
}

void *setDLL(DLL *items, int index, void *value) {
    if (debugDLL) printf("_DLL - setting value in index %i\n", index);
    assert(index >= 0);
    assert(index <= items->size);
    void *data = 0;
    if (index == items->size) { //inserts it at the end
        insertDLL(items, index, value);
    } else if (index == 0) {
        data = getNODEvalue(items->head);
        setNODEvalue(items->head, value);
    } else {
        NODE *n = getNODEnext(getNodeBefore(items, index));
        data = getNODEvalue(n);
        setNODEvalue(n, value);
    }
    return data;
}

int sizeDLL(DLL *items) {
    return items->size;
}

void displayDLL(DLL *items, FILE *fp) {
    NODE *n = items->head;
    printf("{{");
    if (items->size != 0){
        for (int x = 0; x < items->size; x++) {
            if (x > 0) printf(",");
            items->display(getNODEvalue(n), fp);
            n = getNODEnext(n);
        }
    }
    printf("}}");
}

void displayDLLdebug(DLL *items, FILE *fp) {
    printf("head->");
    displayDLL(items, fp);
    printf(",tail->{{");
    if(items->tail != 0)items->display(getNODEvalue(items->tail), fp);
    printf("}}");
}

void freeDLL(DLL *items) {
    NODE *n = items->head;
    NODE *next = 0;
    if (items->size > 1) next = getNODEnext(n);
    while (n != 0) {
        if (debugDLL) printf("_DLL - - Freeing an element");
        freeNODE(n, items->free);
        n = next;
        if (n != 0)next = getNODEnext(n);
    }
    free((DLL *) items);
}