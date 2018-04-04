//
// Created by Parker Jones on 2/28/18.
//

#include <stdlib.h>
#include <assert.h>
#include "bst.h"
#include "avl.h"


static int debug = 0;


typedef struct avlnode AVLNODE;
static int maxHeight(BSTNODE *l, BSTNODE *r);

struct avlnode {
    int leftHeight;
    int rightHeight;
    int height;
    int count;
    void *value;
    BST *tree;

    void (*display)(void *v, FILE *fp);

    int (*compare)(void *v, void *w);

    void (*freer)(void *n);
};

static AVLNODE *newAVLNODE(void *v,
                           BST *tree,
                           void (*display)(void *v, FILE *fp),
                           int (*compare)(void *v, void *w),
                           void (*freer)(void *n)) {
    AVLNODE *node = malloc(sizeof(AVLNODE));
    assert(node != 0);
    node->value = v;
    node->display = display;
    node->compare = compare;
    node->freer = freer;
    node->count = 1;
    node->height = 1;
    node->leftHeight = 0;
    node->rightHeight = 0;
    node->tree = tree;

    return node;
}


static void *getAVLNODEvalue(AVLNODE *n) {
    if (n)return n->value;
    else return 0;
}

static int getAVLNODEcount(AVLNODE *n) {
    if (n)return n->count;
    else return 0;
}

static void incrementAVLNODEcount(AVLNODE *n) {
    if (n) n->count++;
}

static void decrementAVLNODEcount(AVLNODE *n) {
    if (n) n->count--;
}


static int getAVLNODEbalance(AVLNODE *n) {
    return n->leftHeight - n->rightHeight;
}

static void displayAVLNODE(void *n, FILE *fp) {
    AVLNODE *node = n;
    if (node)node->display(getAVLNODEvalue(node), fp);
    if (getAVLNODEcount(node) > 1) {
        printf("[");
        printf("%d", getAVLNODEcount(node));
        printf("]");
    }
    if (getAVLNODEbalance(node) == 1) {
        printf("+");
    } else if (getAVLNODEbalance(node) == -1) {
        printf("-");
    }
    //printf("[%d]", getAVLNODEcount(n));
}

static int compareAVLNODE(void *v, void *w) {

    if (w && v)return ((AVLNODE *) v)->compare(getAVLNODEvalue(v), getAVLNODEvalue(w));
    else if (w) return -1;
    else if (v) return 1;
    else return 0;
}

static void freeAVLNODE(void *n) {
    if(debug) {
        printf("_AVL : freeAVLNODE -  freeing [");
        displayAVLNODE(n, stdout);
        printf("]\n");
    }
    ((AVLNODE *) n)->freer(((AVLNODE *) n)->value);
    free(n);
}


static void freeAVLNODEandSaveVal(void *n) {
    if (n)free(n);
}


static BST *getAVLNODEtree(AVLNODE *n) {
    if (n) return n->tree;
    else return 0;
};


static int getAVLNODEheight(AVLNODE *n) {
    if (n) return n->height;
    else return 0;
}


static void setBSTNODEbalance(BSTNODE *b) {
    BSTNODE *left = getBSTNODEleft(b);
    BSTNODE *right = getBSTNODEright(b);
    AVLNODE *n = getBSTNODEvalue(b);
    if(left)n->leftHeight = getAVLNODEheight(getBSTNODEvalue(left));
    else n->leftHeight = 0;
    if(right)n->rightHeight = getAVLNODEheight(getBSTNODEvalue(right));
    else n->rightHeight = 0;
    n->height = maxHeight(left, right) + 1;
}







static void swapAVLNODE(BSTNODE *bv, BSTNODE *bw) {
    //value and count need to switch not height and balance

    AVLNODE *v = getBSTNODEvalue(bv);
    AVLNODE *w = getBSTNODEvalue(bw);

    void *value = v->value;
    int count = v->count;

    v->value = w->value;
    v->count = w->count;

    w->value = value;
    w->count = count;
}


static void fixupAVL(BSTNODE *b);


struct avl {
    void (*display)(void *, FILE *);

    int (*compare)(void *, void *);

    void (*free)(void *);

    BST *tree;

    int duplicates;
};


AVL *newAVL(
        void (*display)(void *, FILE *),           //display
        int (*compare)(void *, void *),            //comparator
        void (*free)(void *)) {

    AVL *a = malloc(sizeof(AVL));
    a->display = display;
    a->compare = compare;
    a->free = free;
    a->duplicates = 0;
    a->tree = newBST(displayAVLNODE, compareAVLNODE, swapAVLNODE, freeAVLNODE);
    return a;
}


static int isLeftChild(BSTNODE *leaf) {
    if (getBSTNODEparent(leaf) && getBSTNODEleft(getBSTNODEparent(leaf))) {
        if (compareAVLNODE(getBSTNODEvalue(leaf), getBSTNODEvalue(getBSTNODEleft(getBSTNODEparent(leaf)))) == 0)
            return 1;
        else return 0;
    } else return 0;
}

static int maxHeight(BSTNODE *l, BSTNODE *r) {
    if (!l && !r) { //if it doesnt have a left or right child
        return 0;
    } else if (!r) {
        return getAVLNODEheight(getBSTNODEvalue(l));
    } else if (!l) {
        return getAVLNODEheight(getBSTNODEvalue(r));
    } else {
        int left = getAVLNODEheight(getBSTNODEvalue(l));
        int right = getAVLNODEheight(getBSTNODEvalue(r));
        return (left > right) ? left : right;
    }
}

/*

static int getBalance(BSTNODE *lb, BSTNODE *rb) {
    AVLNODE *l = 0;
    AVLNODE *r = 0;
    if (lb) l = getBSTNODEvalue(lb);
    if (rb) r = getBSTNODEvalue(rb);

    if (l && r) return getAVLNODEheight(l) - getAVLNODEheight(r);
    else if (l) return getAVLNODEheight(l);
    else if(r) return 0 - getAVLNODEheight(r);
    else return 0;
}
*/

static BSTNODE *getFavorite(BSTNODE *node) {
    if(node) {
        AVLNODE *n = getBSTNODEvalue(node);
        if (getAVLNODEbalance(n) > 0) {
            if (debug) printf("_AVL : getFavorite -  favorite is the left child\n");
            return getBSTNODEleft(node);
        } else if (getAVLNODEbalance(n) < 0) {
            if (debug) printf("_AVL : getFavorite -  favorite is the right child\n");
            return getBSTNODEright(node);
        } else {
            if (debug) printf("_AVL : getFavorite -  no favorite child\n");
            return 0;
        }
    }
    else return 0;
}


static int isFavorite(BSTNODE *parent, BSTNODE *node) {
    AVLNODE *pNode = getBSTNODEvalue(parent);

    if(!node) return 0;

    if (isLeftChild(node)) {
        if (getAVLNODEbalance(pNode) > 0) { //it is a left child
            return 1;
        } else return 0;
    } else {
        if (getAVLNODEbalance(pNode) < 0) {
            return 1;
        } else return 0;
    }
}


static int areLinear(BSTNODE *node, BSTNODE *favorite) {
    AVLNODE *favNode = getBSTNODEvalue(favorite);

    if (isLeftChild(node)) {
        if(getBSTNODEleft(node)) {
            if (compareAVLNODE(favNode, getBSTNODEvalue(getBSTNODEleft(node))) == 0) {
                return 1;
            } else {
                return 0;
            }
        }
        else return 0;
    } else { //node is right child
        if(getBSTNODEright(node)) {
            if (compareAVLNODE(favNode, getBSTNODEvalue(getBSTNODEright(node))) == 0) {
                return 1;
            } else {
                return 0;
            }
        }
        else return 0;
    }
}


static BSTNODE *getSibling(BSTNODE *node) {
    BSTNODE *parent = getBSTNODEparent(node);
    if (parent) {
        if (isLeftChild(node)) {
            return getBSTNODEright(parent);
        } else return getBSTNODEleft(parent);
    } else {
        if (debug) printf("_AVL : getSibling -   ERROR: There was no parent!\n");
        return 0;
    }
}



static void rotateNodetoParent(BSTNODE *node, BSTNODE *parent) {

    if(debug) {
        printf("_AVL : rotateNodetoParent -  rotating [");
        displayAVLNODE(getBSTNODEvalue(node), stdout);
        printf("] to [");
        displayAVLNODE(getBSTNODEvalue(parent), stdout);
        printf("]\n");
    }

    BSTNODE *grandparent = 0;
    if(getBSTNODEparent(parent)) grandparent = getBSTNODEparent(parent);

    if(isLeftChild(node)) {
        //rightRotate(parent);
        if(debug) printf("_AVL : rotateNodetoParent -  RIGHT Rotation\n");


        if(grandparent && compareAVLNODE(getBSTNODEvalue(grandparent), getBSTNODEvalue(parent)) != 0) {
            if(debug) {
                printf("_AVL: rotateNodetoParent -  linking grandparent[");
                displayAVLNODE(getBSTNODEvalue(grandparent),stdout);
                printf("] to node[");
                displayAVLNODE(getBSTNODEvalue(node),stdout);
                printf("]\n");
            }
            if(isLeftChild(parent)) {
                setBSTNODEleft(grandparent, node);
            }
            else {
                setBSTNODEright(grandparent, node);
            }
        }
        else {
            if(debug) printf("_AVL : rotateNodetoParent -  set a new root\n");
            setBSTroot(getAVLNODEtree(getBSTNODEvalue(node)), node);
        }
        setBSTNODEparent(node, grandparent);

        BSTNODE *rightChild = getBSTNODEright(node);


        setBSTNODEparent(parent, node);
        setBSTNODEright(node, parent);

        setBSTNODEleft(parent, rightChild);
        // set leftChild's parent


        if(debug && rightChild) {
            printf("_AVL : rotateNodetoParent -  linking rightChild[");
            displayAVLNODE(getBSTNODEvalue(rightChild),stdout);
            printf("] to parent[");
            displayAVLNODE(getBSTNODEvalue(parent),stdout);
            printf("]\n");
        }
        setBSTNODEleft(parent, rightChild);
        if(rightChild) setBSTNODEparent(rightChild, parent);

    }

    else {
        //leftRotate(node);

        if(debug) printf("_AVL : rotateNodetoParent -  LEFT Rotation\n");

        if(grandparent && compareAVLNODE(getBSTNODEvalue(grandparent), getBSTNODEvalue(parent)) != 0) {
            if(debug) {
                printf("_AVL: rotateNodetoParent -  linking grandparent[");
                displayAVLNODE(getBSTNODEvalue(grandparent),stdout);
                printf("] to node[");
                displayAVLNODE(getBSTNODEvalue(node),stdout);
                printf("]\n");
            }
            if(isLeftChild(parent)) {
                setBSTNODEleft(grandparent, node);
            }
            else {
                setBSTNODEright(grandparent, node);
            }
        }
        else {
            if(debug) printf("_AVL : rotateNodetoParent -  set a new root\n");
            setBSTroot(getAVLNODEtree(getBSTNODEvalue(node)), node);
        }
        setBSTNODEparent(node, grandparent);

        if(debug) {
            printf("_AVL : rotateNodetoParent -  linking parent[");
            displayAVLNODE(getBSTNODEvalue(parent),stdout);
            printf("] to node[");
            displayAVLNODE(getBSTNODEvalue(node),stdout);
            printf("]\n");
        }
        setBSTNODEparent(parent, node);

        BSTNODE *leftChild = getBSTNODEleft(node);

        setBSTNODEleft(node, parent);


        if(leftChild) setBSTNODEparent(leftChild, parent);
        setBSTNODEright(parent, leftChild);

    }

    if(debug) {
        printf("_AVL : rotateNodetoParent -  done rotating.\n");
        displayBSTdecorated(getAVLNODEtree(getBSTNODEvalue(node)), stdout);
    }
}




//uses a loop and fixes the tree from the leaf up to the root
static void fixupAVL(BSTNODE *b) {
    BSTNODE *x = b;

    while(x) {
        if (debug) {
            printf("_AVL : fixupAVL -  fixing up [");
            displayAVLNODE(getBSTNODEvalue(x), stdout);
            printf("]\n");
        }
        BSTNODE *p = getBSTNODEparent(x);
        BSTNODE *z = 0;
        if (p)z = getSibling(x);

        if (!p) { //x is root (no parent)
            break;
        } else if (isFavorite(p, z)) {
            if (debug) printf("_AVL : fixupAVL -  sibling is favorite\n");
            setBSTNODEbalance(p);
            break;
        } else if (getAVLNODEbalance(getBSTNODEvalue(p)) == 0) {
            if (debug) {
                printf("_AVL : fixupAVL -  parent has no favorite. setting balance of parent [");
                displayAVLNODE(getBSTNODEvalue(p), stdout);
                printf("]\n");
            }
            setBSTNODEbalance(p);
            x = p;
            //continue looping
        } else {
            if (debug) printf("_AVL : fixupAVL -  x is favorite\n");

            BSTNODE *y = getFavorite(x);

            if (y && !areLinear(x, y)) {
                if (debug) printf("_AVL : fixupAVL -  non-linear! double rotation.\n");

                rotateNodetoParent(y, x);
                rotateNodetoParent(y, p);

                if (debug) printf("_AVL : fixupAVL -  setting balance of x, p, and y\n");
                setBSTNODEbalance(x);
                setBSTNODEbalance(p);
                setBSTNODEbalance(y);
            } else {
                if (debug) printf("_AVL : fixupAVL -  linear! single rotation\n");
                rotateNodetoParent(x, p);
                if (debug) printf("_AVL : fixupAVL -  setting balance of p and x\n");
                setBSTNODEbalance(p);
                setBSTNODEbalance(x);
            }
            break;
        }
    }
}

static void deleteAVLfixup(BSTNODE *b) {

    BSTNODE *x = b;
    AVLNODE *n = getBSTNODEvalue(b);
    n->height = 0;

    while (x) {

        if(debug) {
            printf("_AVL : deleteAVLfixup -  x is now [");
            displayAVLNODE(getBSTNODEvalue(x), stdout);
            printf("]\n");
        }


        BSTNODE *p = getBSTNODEparent(x);

        //if (x is the root)
        //    exit the loop
        if(!p || compareAVLNODE(getBSTNODEvalue(p), getBSTNODEvalue(x)) == 0) {
            if(debug) printf("_AVL : deleteAVLfixup -  x is root. breaking.\n");
            break;
        }

//        else if (parent favors x)                  //case 1
//        {
//            set the balance of parent
//            x = parent
//            //continue looping
//        }
        else if(isFavorite(p, x)) {
            if(debug) printf("_AVL : deleteAVLfixup -  parent favors x. Rebalancing parent\n");
            setBSTNODEbalance(p);
            x = p;
            //continue looping
        }

//        else if (parent has no favorite)           //case 2
//        {
//            set the balance of parent
//            exit the loop
//        }
        else if(getAVLNODEbalance(getBSTNODEvalue(p)) == 0) {
            AVLNODE *parent = getBSTNODEvalue(p);
            if(debug) printf("_AVL : deleteAVLfixup -  parent has no favorite. LEFTH - %d, RIGHTH - %d,  Rebalancing parent\n", parent->leftHeight, parent->rightHeight);
            setBSTNODEbalance(p);
            //rebalance(x);
            break;
        }


        else {
            if(debug) printf("_AVL : deleteAVLfixup -  parent favors sibling.\n");
            BSTNODE *z = getSibling(x);
            BSTNODE *y = getFavorite(z);

            //z and y can be 0
            if(y && !areLinear(z, y)) {
                if(debug) printf("_AVL : deleteAVLfixup -  nonLinear. Double Rotating!\n");

                rotateNodetoParent(y, z);
                rotateNodetoParent(y, p);
                setBSTNODEbalance(p);
                setBSTNODEbalance(z);
                setBSTNODEbalance(y);
                x = y;
                //continue looping
            }
            else {
                if(debug) printf("_AVL : deleteAVLfixup -  Linear. Single Rotating!\n");

                rotateNodetoParent(z, p);
                setBSTNODEbalance(p);
                setBSTNODEbalance(z);

                if(!y) break;
                else x = z;
                //continue looping
            }
        }
    }


    /*
    AVLNODE *n = getBSTNODEvalue(bnode);
    BSTNODE *parent = getBSTNODEparent(bnode);
    AVLNODE *pNode = getBSTNODEvalue(parent);
    n->height = 0;

    if (debug) {
        if (debug) printf("_AVL : deleteAVLfixup -  bnode is [");
        displayAVLNODE(getBSTNODEvalue(bnode), stdout);
        if (debug) printf("]\n");
    }

    while (bnode) {
        if (!parent) { //bnode is the root
            break;
        } else if (isFavorite(parent, bnode)) { // parent favors bnode
            if (debug) printf("_AVL : deleteAVLfixup -  Case 1! bnode is favorite\n");
            pNode->balance = getBalance(getBSTNODEleft(parent), getBSTNODEright(parent));
            bnode = parent;
            //continue looping
        } else if (pNode->balance == 0) { //parent has no favorite
            if (debug) printf("_AVL : deleteAVLfixup -  Case 2\n");
            pNode->balance = getBalance(getBSTNODEleft(parent), getBSTNODEright(parent));
            break;
        } else {
            BSTNODE *sibling = getSibling(bnode);
            BSTNODE *siblingFav = getFavorite(sibling);
            //if siblingFav exists and they are not linear
            if (siblingFav && !areLinear(sibling, siblingFav)) {

                if (debug) printf("_AVL : deleteAVLfixup -  Case 3!\n");

                rotateNodetoParent(siblingFav,sibling);
                rotateNodetoParent(siblingFav, parent);


                pNode->balance = getBalance(getBSTNODEleft(parent), getBSTNODEright(parent));
                AVLNODE *sNode = getBSTNODEvalue(sibling);
                sNode->balance = getBalance(getBSTNODEleft(sibling), getBSTNODEright(sibling));
                AVLNODE *favNode = getBSTNODEvalue(siblingFav);
                favNode->balance = getBalance(getBSTNODEleft(siblingFav), getBSTNODEright(siblingFav));
                bnode = siblingFav;
            } else {
                if (debug) printf("_AVL : deleteAVLfixup -  Case 4!\n");

                rotateNodetoParent(sibling, parent);

                pNode->balance = getBalance(getBSTNODEleft(parent), getBSTNODEright(parent));
                AVLNODE *sNode = getBSTNODEvalue(sibling);
                sNode->balance = getBalance(getBSTNODEleft(sibling), getBSTNODEright(sibling));
                if (!siblingFav) {
                    break;
                } else {
                    bnode = sibling;
                    //continue looping
                }
            }
        }
    }*/
}


void insertAVL(AVL *a, void *value) {
    AVLNODE *n = newAVLNODE(value, a->tree, a->display, a->compare, a->free);
    BSTNODE *bnode = findBST(a->tree, n);

    if (bnode) { //it already exists
        if (debug) {
            printf("_AVL : insertAVL -  NODE[");
            a->display(value, stdout);
            printf("] already exists. incrementing count\n");
        }
        incrementAVLNODEcount(getBSTNODEvalue(bnode));
        a->duplicates++;
        freeAVLNODEandSaveVal(n);
    } else { //it doesnt already exist
        bnode = insertBST(a->tree, n);

        if (debug) {
            printf("_AVL : insertAVL -  NODE[");
            a->display(value, stdout);
            printf("] doesn't exist. inserted and fixing up\n");
        }

        if(sizeBST(a->tree) == 1){
            if(debug) printf("_AVL : insertAVL -  root parent set to 0\n");
            setBSTNODEparent(bnode, 0);
        }

        fixupAVL(bnode);



    }
    if (debug) {
        printf("_AVL : insertAVL -  after insertion display:\n");
        displayAVL(a, stdout);
    }
}


int findAVLcount(AVL *a, void *v) {
    AVLNODE *n = newAVLNODE(v, a->tree, a->display, a->compare, a->free);
    BSTNODE *b = findBST(a->tree, n);
    free(n);
    if (b)return getAVLNODEcount(getBSTNODEvalue(b));
    else return 0;
}


void *findAVL(AVL *a, void *v) {
    AVLNODE *n = newAVLNODE(v, a->tree, a->display, a->compare, a->free);
    BSTNODE *b = findBST(a->tree, n);
    free(n);
    if (b)return getAVLNODEvalue(getBSTNODEvalue(b));
    else return 0;
}


void displayAVL(AVL *a, FILE *fp) {
    if (sizeBST(a->tree) > 0)displayBSTdecorated(a->tree, fp);
    else printf("EMPTY\n");
}


void *deleteAVL(AVL *a, void *v) {

    AVLNODE *n = newAVLNODE(v, a->tree, a->display, a->compare, a->free);
    BSTNODE *b = findBST(a->tree, n);
    if (!b) {
//        printf("Value ");
//        displayAVLNODE(n, stdout);
//        printf(" not found.\n");
        free(n);
        return 0;
    }
    if (debug) {
        printf("_AVL : deleteAVL -  deleting ");
        a->display(v, stdout);
        printf("\n");
    }
    void *temp;
    free(n);
    n = getBSTNODEvalue(b);
    temp = getAVLNODEvalue(n);

    if (n->count > 1) {
        if (debug) printf("_AVL : deleteAVL -  count is >1. decrementing\n");
        decrementAVLNODEcount(n);
        a->duplicates--;
    } else {
        if (debug) {
            printf("_AVL : deleteAVL -  swapping [");
            a->display(v, stdout);
            printf("] to leaf\n");
        }
        b = swapToLeafBST(a->tree, b);
        if (debug) {
            displayAVL(a, stdout);
            printf("_AVL : deleteAVL -  deleting leaf. \n");
        }

        //todo swap this with deleteAVL
        //fixupAVL(b);
        deleteAVLfixup(b);


        pruneLeafBST(a->tree, b);
        setBSTsize(a->tree, sizeBST(a->tree) - 1);


        freeBSTNODE(b, freeAVLNODEandSaveVal);
        if (debug)printf("_AVL : deleteAVL - size is now %d.\n", sizeBST(a->tree));



        if (debug) {
            printf("_AVL : deleteAVL -  after deletion display:\n");
            displayAVL(a, stdout);
        }
        //freeBSTNODE(b, 0);
        //setBSTsize(a->tree, sizeBST(a->tree) - 1);


    }

    return temp;
}


int sizeAVL(AVL *a) {
    return sizeBST(a->tree);
}


void statisticsAVL(AVL *a, FILE *fp) {
    printf("Duplicates: %d\n", a->duplicates);
    statisticsBST(a->tree, fp);
}


void displayAVLdebug(AVL *a, FILE *fp) {
    displayBST(a->tree, fp);
}


void freeAVL(AVL *a) {
    freeBST(a->tree);
    free(a);
}




