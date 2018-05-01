//
// Created by Parker Jones on 1/22/18.
//

#include "queue.h"


struct queue {
    SLL *items;
    void (*display) (void *, FILE *);
    void (*free) (void *);
};

QUEUE *newQUEUE(void (*display)(void *,FILE *),void (*free)(void *)) {

    QUEUE *queue = malloc(sizeof(QUEUE));
    SLL *items = newSLL(display, free);
    assert(items != 0);
    assert(queue != 0);
    queue->free = free;
    queue->display = display;
    queue->items = items;
    return queue;

}

void enqueue(QUEUE *queue,void *value) {
    insertSLL(queue->items, sizeSLL(queue->items), value);
}


void *dequeue(QUEUE *queue) {
    assert(sizeQUEUE(queue));
    void *value = removeSLL(queue->items, 0);
    return value;
}

void *peekQUEUE(QUEUE *queue) {
    assert(sizeQUEUE(queue) > 0);
    void *value = getSLL(queue->items, 0);
    return value;
}

int sizeQUEUE(QUEUE *queue) {
    return sizeSLL(queue->items);
}

void displayQUEUE(QUEUE *queue,FILE *fp) {
    int size = sizeSLL(queue->items);
    printf("<");
    for(int x = 0; x < size; x++) {
        if(x > 0) printf(",");
        queue->display(getSLL(queue->items, x), fp);
    }
    printf(">");
}

void displayQUEUEdebug(QUEUE *queue,FILE *fp) {
     displaySLLdebug(queue->items, fp);
}

void freeQUEUE(QUEUE *queue) {
    freeSLL(queue->items);
    free(queue);
}