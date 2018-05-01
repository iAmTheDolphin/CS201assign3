
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
#include "queue.h"

#include <unistd.h>


#define DEBUG 0

#if DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif


VERTEX *start = 0;
AVL *MSTNODES = 0;
BINOMIAL *MST = 0;
int totalweight = 0;


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


static int compareVertexNumbers(void *x, void *y) {
    if(!x && !y) return 0;
    if(!x) return -1;
    if(!y) return 1;
    int xint = getVERTEXnumber(x);
    int yint = getVERTEXnumber(y);
    return xint - yint;
}


static int findEdgeWeight(AVL *edgeAVL, VERTEX *v1, VERTEX*v2) {
    EDGE *e = 0;
    if(compareVertexNumbers(v1, v2) > 0) {
        e = newEDGE(getVERTEXnumber(v2), getVERTEXnumber(v1), 1);
    }
    else {
        e = newEDGE(getVERTEXnumber(v1), getVERTEXnumber(v2), 1);
    }

    EDGE *temp = findAVL(edgeAVL, e);

    freeEDGE(e);

    return getEDGEweight(temp);
}


static void sortDLL(DLL *items) {
    BINOMIAL *b = newBINOMIAL(displayVERTEX, compareVertexNumbers, 0, freeVERTEX);

    while(sizeDLL(items)){
        dprintf("_prim : sortDLL -  \n");
        insertBINOMIAL(b,removeDLL(items, 0));
    }
    while(sizeBINOMIAL(b)){
        dprintf("_prim : sortDLL -  \n");
        insertDLL(items, sizeDLL(items), extractBINOMIAL(b));
    }
    freeBINOMIAL(b);
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


static void decreaseKeyOfNeighbors(BINOMIAL *b, VERTEX *v) {
    firstDLL(getVERTEXneighbors(v));
    firstDLL(getVERTEXweights(v));

    for (int x = 0; x < sizeDLL(getVERTEXneighbors(v)); x++) {
        VERTEX *neighbor = currentDLL(getVERTEXneighbors(v));

        int *weight = (int *) currentDLL(getVERTEXweights(v));

        //if the key of the neighbor is greater than the new weight or the key of the new neighbor is -1
        //then set the key to the new weight
        if (getVERTEXkey(neighbor) == -1 || getVERTEXkey(neighbor) > *weight) {
            setVERTEXkey(neighbor, *weight);
            setVERTEXpred(neighbor, v);
        }

        //run decrease key to bubble it up
        decreaseKeyBINOMIAL(b, getVERTEXowner(neighbor), neighbor);


        nextDLL(getVERTEXneighbors(v));
        nextDLL(getVERTEXweights(v));
    }
}


static void *vertexExists(AVL *vertexAVL, int num) {

    VERTEX *vertex1 = newVERTEX(num);
    VERTEX *t = findAVL(vertexAVL, vertex1);
    freeVERTEX(vertex1);
    return t;

}



void readEverything(FILE *fp, AVL *edgesAVL, AVL *vertexAVL, BINOMIAL *b) {
    if (fp) {
        int v1;
        int v2;
        int w;

        int swapped = 0;

        while (!feof(fp)) {
            char *read = readToken(fp);
            if (read) {
                dprintf("_prim : readEverything -  first vertex is [%s]\n", read);
                v1 = atoi(read);

                free(read);
                read = readToken(fp);
                dprintf("_prim : readEverything -  second vertex is [%s]\n", read);
                v2 = atoi(read);

                if (v1 > v2) { //so that the smaller vertex is always first
                    int temp = v1;
                    v1 = v2;
                    v2 = temp;
                    swapped = 1;
                }
                read = readToken(fp);
                if (strncmp(read, ";", 1) == 0) {
                    w = 1;
                } else {
                    w = atoi(read);
                    readToken(fp);
                }

                dprintf("_prim : readEverything -  edge read in. line is %d, %d, %d ;\n", v1, v2, w);


                if(v1 != v2) {

                    //make an edge here
                    EDGE *e = newEDGE(v1, v2, w);
                    VERTEX *vertex1 = vertexExists(vertexAVL, v1);
                    if (!vertex1) {
                        vertex1 = newVERTEX(v1);
                    }
                    VERTEX *vertex2 = vertexExists(vertexAVL, v2);
                    if (!vertex2) {
                        vertex2 = newVERTEX(v2);
                    }


                    int selector = 0;
                    //case 0 : both vertices already exist
                    //case 1 : vertex 2 exists. vertex 1 does not
                    //case 2 : vertex 1 exists. vertex 2 does not
                    //case 3 : niether vertex exists
                    //case 4 : first vertex in. Setting start vertex
                    if (!findAVL(vertexAVL, vertex1)) selector += 1;
                    if (!findAVL(vertexAVL, vertex2)) selector += 2;
                    if (!start) { selector = 4; }

                    if(DEBUG) {
                        displayAVL(vertexAVL, stdout);
                    }
                    dprintf("_prim : readEverything -  selector is %d\n", selector);

                    if (selector != 0) {
                        switch (selector) {
                            case 1 :
                                insertAVL(vertexAVL, vertex1);
                                setVERTEXowner(vertex1, insertBINOMIAL(b, vertex1));
                                break;
                            case 2 :
                                insertAVL(vertexAVL, vertex2);
                                setVERTEXowner(vertex2, insertBINOMIAL(b, vertex2));
                                break;
                            case 3 :
                                insertAVL(vertexAVL, vertex1);
                                insertAVL(vertexAVL, vertex2);
                                setVERTEXowner(vertex1, insertBINOMIAL(b, vertex1));
                                setVERTEXowner(vertex2, insertBINOMIAL(b, vertex2));
                                break;
                            case 4 :
                                insertAVL(vertexAVL, vertex1);
                                insertAVL(vertexAVL, vertex2);
                                if (swapped) {
                                    start = vertex2;
                                    setVERTEXkey(vertex2, 0);
                                } else {
                                    start = vertex1;
                                    setVERTEXkey(vertex1, 0);
                                }
                                setVERTEXowner(vertex1, insertBINOMIAL(b, vertex1));
                                setVERTEXowner(vertex2, insertBINOMIAL(b, vertex2));
                                break;
                            default :;
                        }
                        if (DEBUG) {
                            printf("__________________________\n");
                            displayVERTEX(vertex1, stdout);
                            displayDLL(getVERTEXneighbors(vertex1), stdout);
                            printf("\n");
                            displayVERTEX(vertex2, stdout);
                            displayDLL(getVERTEXneighbors(vertex2), stdout);
                            printf("\n");
                        }
                    }

                    if (!findAVL(edgesAVL, e)) {
                        dprintf("_prim : readEverything -  inserting edge\n");
                        insertAVL(edgesAVL, e);
                        insertVERTEXneighbor(vertex1, vertex2);
                        insertVERTEXneighbor(vertex2, vertex1);
                        insertVERTEXweight(vertex1, w);
                        insertVERTEXweight(vertex2, w);
                    } else {
                        dprintf("_prim : readEverything -  discarding edge\n");
                    }
                }
                else{
                    //v1 and v2 are the same
                    dprintf("inserting same same\n");
                    VERTEX *v = newVERTEX(v1);
                    if(!findAVL(vertexAVL, v)) {
                        insertAVL(vertexAVL, v);
                        setVERTEXowner(v, insertBINOMIAL(b, v));
                        if(!start) {
                            start = v;
                            setVERTEXkey(v, 0);
                        }
                    }
                    else {
                        freeVERTEX(v);
                    }
                }
            }
            if (DEBUG)displayBINOMIALdebug(b, stdout);
            free(read);
        }
    }
}


void updateVertexOwner(void *v, void *owner) {
    VERTEX *vertex = v;
    setVERTEXowner(vertex, owner);
}


//extracts from binomial, adds the vertex as a successor of the pred. Then removes
//the vertex from the neighbor list of its neighbors.
static VERTEX *extractBinomialForMST(BINOMIAL *b) {

    VERTEX *vertex = peekBINOMIAL(b);

    if(DEBUG) {
        displayBINOMIAL(b, stdout);
        printf("_prim : extractBinForMST -  extracting ");
        displayVERTEX(vertex, stdout);
        printf(" and decrease keying ");
        displayDLL(getVERTEXneighbors(vertex), stdout);
        printf("\n_prim : extractBinForMST -  setting as successor of ");
        displayVERTEX(getVERTEXpred(vertex),stdout);
        printf("\n");
        displayBINOMIAL(b, stdout);
    }

    totalweight += getVERTEXkey(vertex);




    insertAVL(MSTNODES, vertex);
    insertBINOMIAL(MST, vertex);




    if(compareVERTEX(getVERTEXpred(vertex), vertex) != 0)insertVERTEXsuccessor(getVERTEXpred(vertex), vertex);




    firstDLL(getVERTEXweights(vertex));
    firstDLL(getVERTEXneighbors(vertex));

    //goes through each of the neighbors of vertex
    for(int x = 0; x < sizeDLL(getVERTEXneighbors(vertex)); x ++) {
        VERTEX *neighbor = getDLL(getVERTEXneighbors(vertex), x);

        //goes through all the neighbors of the neighbor
        for(int y = 0; y < sizeDLL(getVERTEXneighbors(neighbor)) ; y ++) {
            VERTEX *curr = getDLL(getVERTEXneighbors(neighbor), y);

            //removes the neighbor and weight from the neighbor list of the neighbor
            if(compareVERTEX(curr, vertex) == 0) {
                removeDLL(getVERTEXneighbors(neighbor), y);
                removeDLL(getVERTEXweights(neighbor), y);
                break;
            }
        }
    }

    peekBINOMIAL(b);
    dprintf("_prim : extractBinForMST -  \n");
    extractBINOMIAL(b);
    if(DEBUG) displayBINOMIAL(b, stdout);

    return vertex;
}


static void enqueueDLL(QUEUE *items, DLL *list) {
    sortDLL(list);
    firstDLL(list);
    if(DEBUG) {
        printf("_prim : enqueueDLL -  Enqueueing ");
        displayDLL(list, stdout);
        printf("\n");
    }
    for (int i = 0; i < sizeDLL(list); i++) {

        enqueue(items, currentDLL(list));
        nextDLL(list);
    }
    firstDLL(list);
}


static int compareForDisplay (void *a, void *b) {
    /*
    if(!a)return -1;
    if(!b)return 1;

    VERTEX *x = a;
    VERTEX *y = b;

    if (x->key == -1 && y->key != -1)
        return 1;
    else if (x->key != -1 && y->key == -1)
        return -1;

    int r = x->key - y->key;
    if (r == 0)
        return x->number - y->number;
    else
        return r;
     */

    if(!a) return -1;
    if(!b) return -1;

    VERTEX *x = a;
    VERTEX *y = b;

    if(getVERTEXnumber(getVERTEXpred(x)) - getVERTEXnumber(getVERTEXpred(y)) == 0 ) {
        return getVERTEXnumber(x) - getVERTEXnumber(y);
    }
    else return getVERTEXnumber(getVERTEXpred(x)) - getVERTEXnumber(getVERTEXpred(y));

}


static void displayMST(AVL *edgeAVL, VERTEX *start) {

    VERTEX *v = start;

    QUEUE *items = newQUEUE(displayVERTEX, 0);

    DLL *levelDLL = newDLL(0,0);

    if(v) enqueue(items, v);

    int nodes = sizeQUEUE(items);

    int level = 0;
    while(nodes > 0) {

        printf("%d: ", level);

        //loads nodes in a level into a DLL
        while (nodes > 0) {
            v = peekQUEUE(items);
            insertDLL(levelDLL, 0, v);
            if(DEBUG) {
                printf("_prim : displayMST -  inserting ");
                displayVERTEX(v, stdout);
                printf(" into DLL at level : %d\n", level);
            }
            dequeue(items);
            enqueueDLL(items, getVERTEXsuccessors(v));
            nodes--;

           /* v = peekQUEUE(items);
            if(v) {

                displayVERTEX(v, stdout);
                if (compareVERTEX(v, getVERTEXpred(v)) != 0) {
                    printf("(");
                    displayVERTEX(getVERTEXpred(v), stdout);
                    printf(")%d", findEdgeWeight(edgeAVL, v, getVERTEXpred(v)));
                }

                insertDLL(levelDLL, 0, v);

                dequeue(items);

                enqueueDLL(items, getVERTEXsuccessors(v));

                //if (nodes > 1) printf(" ");
                nodes--;
            }*/
        }

        //sorts then prints the DLL
        sortDLL(levelDLL);
        for (int x = 0;sizeDLL(levelDLL); x++) {
            v = removeDLL(levelDLL, 0);
            if(x > 0) printf(" ");
            displayVERTEX(v, stdout);
            if (compareVERTEX(v, getVERTEXpred(v)) != 0) {
                printf("(");
                displayVERTEX(getVERTEXpred(v), stdout);
                printf(")%d", findEdgeWeight(edgeAVL, v, getVERTEXpred(v)));
            }
        }
        assert(sizeDLL(levelDLL) == 0);

        printf("\n");
        level ++;
        nodes = sizeQUEUE(items);
    }
    printf("weight: %d\n", totalweight);
    freeDLL(levelDLL);
}







int
main(int argc, char **argv) {

    dprintf("First Arg: %s\nSecond Arg: %s\n", argv[1], argv[2]);
    FILE *fp;
    int argIndex;

    MSTNODES = newAVL(displayVERTEX, compareVERTEX, freeVERTEX);
    MST = newBINOMIAL(displayVERTEX, compareForDisplay, updateVertexOwner, freeVERTEX);

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
    else {
        dprintf("opened file!\n");
    }



    BINOMIAL *b = newBINOMIAL(displayVERTEX, compareVERTEX, updateVertexOwner, freeVERTEX);
    AVL *edgesAVL = newAVL(displayEDGE, compareEDGE, freeEDGE);
    AVL *vertexAVL = newAVL(displayVERTEX, compareVertexNumbers, freeVERTEX);


    readEverything(fp, edgesAVL, vertexAVL, b);

    if(!start) {
        printf("EMPTY\n");
        exit(0);
    }

    firstDLL(getVERTEXneighbors(start));
    firstDLL(getVERTEXweights(start));

    setVERTEXpred(start, start);

    //extracts start
    start = extractBinomialForMST(b);


    //decrease key on all of the first vertex's neighbors
    decreaseKeyOfNeighbors(b, start);


    //ok so by here we should have a start node, and a binomial heap with
    //all of the start nodes neighbors set to the correct weight.


    //this loop will go until the binary heap is empty or there arent remaining
    //vertices with a valid weight.
    int n = 0;

    while (sizeBINOMIAL(b) != 0 && getVERTEXkey(peekBINOMIAL(b)) != -1) {

        if(DEBUG) {
            printf("_prim : main -  next in the MST is ");
            displayVERTEX(peekBINOMIAL(b), stdout);
            printf("\n");
        }

        VERTEX *current = extractBinomialForMST(b);

        decreaseKeyOfNeighbors(b, current);

        n++;
    }

    displayMST(edgesAVL, start);

    freeAVL(edgesAVL);
    //freeAVL(vertexAVL);
    freeBINOMIAL(b);

    return 0;
}


