
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include "scanner.h"
#include "avl.h"
#include "binomial.h"
#include "string.h"
#include "vertex.h"
#include "edge.h"
#include "integer.h"

#include <unistd.h>



#define DEBUG 1

#if DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif






/* options */
char *filename;


void Fatal(char *fmt, ...) {
    va_list ap;

    fprintf(stderr, "An error occured: ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    exit(-1);
}


static void printName() {
    printf("Parker Jones\n");
    exit(0);
}


static int
processOptions(int argc, char **argv) {
    int argIndex;
    int argUsed;
    int separateArg;
    argIndex = 1;


    while (argIndex < argc && *argv[argIndex] == '-') {
        dprintf("Checking Options\n");


/* check if stdin, represented by "-" is an argument */


/* if so, the end of options has been reached */

        if (argv[argIndex][1] == '\0') return argIndex;

        separateArg = 0;
        argUsed = 0;

        if (argv[argIndex][2] == '\0') {
            //filename = argv[argIndex+1];
            separateArg = 1;
        }
        //filename = argv[argIndex]+2;

        switch (argv[argIndex][1]) {

/*
             * when option has an argument, do this
             *
             *     examples are -m4096 or -m 4096
             *
             *     case 'm':
             *         MemorySize = atol(arg);
             *         argUsed = 1;
             *         break;
             *
             *
             * when option does not have an argument, do this
             *
             *     example is -a
             *
             *     case 'a':
             *         PrintActions = 1;
             *         break;
             */


            break;
            case 'v':
                printName();
                break;
            default:
                Fatal("option %s not understood\n", argv[argIndex]);
        }

        if (separateArg && argUsed) {
            dprintf("separate arg and argUsed\n");
            //++argIndex;
        }

        ++argIndex;
    }
    dprintf("%d\n", argIndex);

    return argIndex;
}




void readEverything(FILE *fp, AVL *edgesAVL, AVL *vertexAVL, BINOMIAL *b, VERTEX *start) {
    if (fp) {
        int v1;
        int v2;
        int w;


        while (!feof(fp)) {
            char *read = readToken(fp);
            if (read) {
                dprintf("_prim : readEverything -  first vertex is [%s]\n", read);
                v1 = atoi(read);

                read = readToken(fp);
                dprintf("_prim : readEverything -  second vertex is [%s]\n", read);
                v2 = atoi(read);

                if (v1 > v2) { //so that the smaller vertex is always first
                    int temp = v1;
                    v1 = v2;
                    v2 = temp;
                }
                read = readToken(fp);
                if (strncmp(read, ";", 1) == 0) {
                    w = 1;
                } else {
                    w = atoi(read);
                    readToken(fp);
                }

                dprintf("_prim : readEverything -  edge read in. line is %d, %d, %d ;\n", v1, v2, w);



                //make an edge here
                EDGE *e = newEDGE(v1, v2, w);
                VERTEX *vertex1 = newVERTEX(v1);
                VERTEX *vertex2 = newVERTEX(v2);

                if (!findAVL(edgesAVL, e)) {
                    dprintf("");
                    insertAVL(edgesAVL, e);
                }
                if (!findAVL(vertexAVL, vertex1)) {
                    insertAVL(vertexAVL, vertex1);
                    insertVERTEXneighbor(vertex1, vertex2);
                    insertVERTEXneighbor(vertex2, vertex1);
                    setVERTEXowner(vertex1, insertBINOMIAL(b, vertex1));
                    if (sizeAVL(vertexAVL) == 1) {
                        setVERTEXkey(vertex1, 0);
                        start = vertex1;
                    }
                }
                if (!findAVL(vertexAVL, vertex2)) {
                    insertAVL(vertexAVL, vertex2);
                    insertVERTEXneighbor(vertex1, vertex2);
                    insertVERTEXneighbor(vertex2, vertex1);
                    setVERTEXowner(vertex2, insertBINOMIAL(b, vertex2));
                }
            }
        }
    }
}



void updateVertexOwner(void *v, void *owner) {
    VERTEX *vertex = v;
    setVERTEXowner(vertex, owner);
}



int
main(int argc, char **argv) {

    dprintf("First Arg: %s\nSecond Arg: %s\n", argv[1], argv[2]);
    FILE *fp;
    int argIndex;
    void *tree;

    if (argc == 1) Fatal("%d arguments!\n", argc - 1);

    argIndex = processOptions(argc, argv);


    if (argIndex == argc)
        printf("No arguments\n");
    else {
        filename = argv[argIndex];
        if (DEBUG) {
            printf("Remaining arguments:\n");
            for (int i = argIndex; i < argc; ++i)
                printf("    %s\n", argv[i]);
        }
    }

    dprintf("Trying to open %s ", filename);
    fp = fopen(filename, "r");
    if (!fp && DEBUG) printf("couldn't open file!\n");
    else dprintf("opened file!\n");



    BINOMIAL *b = newBINOMIAL(displayVERTEX, compareVERTEX, updateVertexOwner, freeVERTEX);
    AVL *edgesAVL = newAVL(displayEDGE, compareEDGE, freeEDGE);
    AVL *vertexAVL = newAVL(displayVERTEX, compareVERTEX, freeVERTEX);

    VERTEX *start;

    //readEverything(fp, edgesAVL, vertexAVL,b, start);


    insertBINOMIAL(b, newVERTEX(1));
    insertBINOMIAL(b, newVERTEX(2));
    insertBINOMIAL(b, newVERTEX(3));
    insertBINOMIAL(b, newVERTEX(4));
    insertBINOMIAL(b, newVERTEX(5));
    insertBINOMIAL(b, newVERTEX(6));
    insertBINOMIAL(b, newVERTEX(7));
    //insertBINOMIAL(b, newVERTEX(8));
    displayBINOMIAL(b, stdout);
    displayBINOMIALdebug(b, stdout);





    freeAVL(edgesAVL);
    freeAVL(vertexAVL);
    //printf("\n");

    return 0;
}



