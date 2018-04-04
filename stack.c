//
// Created by Parker Jones on 1/17/18.
//

int debugSTACK = 0;

#include "stack.h"

struct stack {
    void (*display)(void *, FILE *);

    void (*free)(void *);

    DLL *items;
};

STACK *newSTACK(void (*display)(void *, FILE *), void (*free)(void *)) {

    STACK *stack = malloc(sizeof(STACK));
    DLL *items = newDLL(display, free);

    assert(items != 0);
    assert(stack != 0);
    stack->display = display;
    stack->free = free;
    stack->items = items;
    return stack;
}

void push(STACK *stack, void *value) {
    if (debugSTACK) printf("_STACK - - pushing into stack...\n");
    insertDLL(stack->items, 0, value);
    if (debugSTACK)
        printf("_STACK - - done pushing. Size is now "
                       ":%d\n", sizeSTACK(stack));

}

void *pop(STACK *stack) {
    if (debugSTACK)
        printf("_STACK - - size of stack pre pop "
                       ":%d\n", sizeSTACK(stack));
    void *value = removeDLL(stack->items, 0);
    assert(value != 0);
    if (debugSTACK)
        printf("_STACK - - size of stack post pop "
                       ":%d\n", sizeSTACK(stack));
    return value;
}

void *peekSTACK(STACK *stack) {
    void *value = getDLL(stack->items, 0);
    assert(value != 0);
    return value;
}

int sizeSTACK(STACK *stack) {
    return sizeDLL(stack->items);
}

void displaySTACK(STACK *stack, FILE *fp) {
    int size = sizeDLL(stack->items);
    printf("|");
    for (int x = 0; x < size; x++) {
        if (x > 0) printf(",");
        stack->display(getDLL(stack->items, x), fp);
    }
    printf("|");
}

void displaySTACKdebug(STACK *stack, FILE *fp) {
    displayDLLdebug(stack->items, fp);
}

void freeSTACK(STACK *stack) {
    freeDLL(stack->items);
    free(stack);
}