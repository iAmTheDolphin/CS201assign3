//
// Created by Parker Jones on 2/14/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include "string.h"


static int debug = 0;
struct String {
    char *value;
};

STRING *
newSTRING(char *x) {
    STRING *p = malloc(sizeof(STRING));
    assert(p != 0);
    p->value = x;
    return p;
}

char
*getSTRING(STRING *v) {
    return v->value;
}

char
*setSTRING(STRING *v, char *x) {
    char *old = v->value;
    v->value = x;
    return old;
}

void
displaySTRING(void *v, FILE *fp) {
    fprintf(fp, "%s", getSTRING((STRING *) v));
}

int
compareSTRINGmax(void *v, void *w) {
    return (strcmp(getSTRING(w), getSTRING(v)));
}

int
compareSTRINGmin(void *v, void *w) {
    return strcmp(getSTRING(v), getSTRING(w));
}

int
compareSTRING(void *v, void *w) {
    if(debug) printf("%s -- %s\n", getSTRING(v), getSTRING(w));
    return strcmp(getSTRING(v), getSTRING(w));
}

void
freeSTRING(void *v) {
    char *s = getSTRING(v);
    if(debug) printf("Freeing : %s\n", s);

    /* fixme this is broken. it should be freeing but it isn't. It is segfaulting like
     * s was never allocated.
     */

    //if(s)free(s);
    free(v);
}
