// Basic type definitions:
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>
/*setup for a red and black tree, taken from https://en.wikipedia.org/wiki/Red%E2%80%93black_tree
and modified to suit my purposes*/

enum color_t { BLACK, RED };
int count = 0;

struct RBnode {     // node of red–black tree
  struct RBnode* parent;   // == NULL if root of the tree
  int postingLength;
  char* key;
  struct RBnode* child[2]; // == NIL if child is empty
    // Index is:
    //   LEFT  := 0, if (key < parent->key)
    //   RIGHT := 1, if (key > parent->key)
  enum color_t color;
  int* postings;
  int postingMaxLength;
  float* frequencies;
};

#define NIL   NULL // null pointer  or  pointer to sentinel node
#define LEFT  0
#define RIGHT 1
#define left  child[LEFT]
#define right child[RIGHT]

struct RBtree { // red–black tree
  struct RBnode* root; // == NIL if tree is empty
};

// Get the child direction (∈ { LEFT, RIGHT })
//   of the non-root non-NIL  RBnode* N:
#define childDir(N) ( N == (N->parent)->right ? RIGHT : LEFT )

// Helper functions:

struct RBnode* GetParent(struct RBnode* N) {
  // The parent of the root node is set to NULL.
  return N == NULL ? NULL : N->parent;
}

struct RBnode* GetGrandParent(struct RBnode* N) {
  // Will return NULL if N is root or child of root
  return GetParent(GetParent(N));
}

struct RBnode* GetSibling(struct RBnode* N) {
  struct RBnode* P = GetParent(N);
  // No parent means no sibling:
  assert(P != NULL);
  return P->child[1-childDir(N)];
}
// If parent P and child direction dir is available, same as:
//   P->child[1-dir]

struct RBnode* GetUncle(struct RBnode* N) {
  struct RBnode* P = GetParent(N);
  // No parent means no uncle:
  assert(P != NULL);
  return GetSibling(P);
}

struct RBnode* GetCloseNephew(struct RBnode* N) {
  struct RBnode* P = GetParent(N);
  int dir;
  struct RBnode* S;
  assert(P != NULL);
  dir = childDir(N);
  S = P->child[1-dir]; // sibling of N
  assert(S != NIL);
  return S->child[dir]; // the nephew close to N
}

struct RBnode* GetDistantNephew(struct RBnode* N) {
  struct RBnode* P = GetParent(N);
  int dir;
  struct RBnode* S;
  assert(P != NULL);
  dir = childDir(N);
  S = P->child[1-dir]; // sibling of N
  assert(S != NIL);
  return S->child[1-dir]; // the nephew distant from N
}

struct RBnode* RotateDirRoot(
    struct RBtree* T,   // red–black tree
    struct RBnode* P,   // root of subtree (may be the root of T)
    int dir) {   // dir ∈ { LEFT, RIGHT }
  struct RBnode* G = P->parent;
  struct RBnode* S = P->child[1-dir];
  struct RBnode* C;
  assert(S != NIL); // pointer to true node required
  C = S->child[dir];
  P->child[1-dir] = C; if (C != NIL) C->parent = P;
  S->child[  dir] = P; P->parent = S;
  S->parent = G;
  if (G != NULL)
    G->child[ P == G->right ? RIGHT : LEFT ] = S;
  else
    T->root = S;
  return S; // new root of subtree
}

#define RotateDir(N,dir) RotateDirRoot(T,N,dir)
#define RotateLeft(N)  RotateDirRoot(T,N,LEFT)
#define RotateRight(N) RotateDirRoot(T,N,RIGHT)

void RBinsert1(
  struct RBtree* T,         // -> red–black tree
  struct RBnode* P,  // -> parent node of N (may be NULL)
  struct RBnode* N,  // -> node to be inserted
  int dir)          // side of P on which to insert N (∈ { LEFT, RIGHT })
{
  struct RBnode* G;  // -> parent node of P
  struct RBnode* U;  // -> uncle of N

  N->color = RED;
  N->left  = NIL;
  N->right = NIL;
  N->parent = P;
  if (P == NULL) {   // There is no parent
    T->root = N;     // N is the new root of the tree T.
    return; // insertion complete
  }
  P->child[dir] = N; // insert N as dir-child of P
  // fall through to the loop
  // start of the (do while)-loop:
  do {
    if (P->color == BLACK) {
      // I_case_3 (P black):
      return; // insertion complete
    }
    // From now on P is red.
    if ((G = GetParent(P)) == NULL) 
      goto I_case_6; // P red and root
    // else: P red and G!=NULL.
    dir = childDir(P); // the side of parent G on which node P is located
    U = G->child[1-dir]; // uncle
    if (U == NIL || U->color == BLACK) // considered black
      goto I_case_45; // P red && U black
    // I_case_1 (P+U red):
    P->color = BLACK;
    U->color = BLACK;
    G->color = RED;
    N = G; // new current node
    // iterate 1 black level (= 2 tree levels) higher
  } while ((P = N->parent) != NULL); // end of (do while)-loop
  // fall through to I_case_2
  // I_case_2 (P == NULL):
  return; // insertion complete
  I_case_45: // P red && U black:
  if (N == P->child[1-dir]) {
    // I_case_4 (P red && U black && N inner grandchild of G):
    RotateDir(P,dir); // P is never the root
    N = P; // new current node
    P = G->child[dir]; // new parent of N
    // fall through to I_case_5
  }
  // I_case_5 (P red && U black && N outer grandchild of G):
  RotateDirRoot(T,G,1-dir); // G may be the root
  P->color = BLACK;
  G->color = RED;
  return; // insertion complete
  I_case_6: // P is the root and red:
    P->color = BLACK;
    return; // insertion complete
} // end of RBinsert1

//searches for a value and if it doesnt exist returns a node with the parent set to the correct spot
//if no node found return a node with null key
struct RBnode* search(char* key, struct RBtree* tree) {
    struct RBnode* current_node = tree->root;
    //creating a node to go into root of tree if tree is empty
    if(current_node == NULL) {
        struct RBnode* new_node = (struct RBnode *) malloc(sizeof(struct RBnode));
        new_node->parent = current_node;
        new_node->postingLength = 0;
        new_node->key = NULL;
        return new_node;
    }
    while(1) {
        //if equal then it exists in tree already so just return it
        if(strcmp(key, current_node->key) == 0) {
            return current_node;
        }
        if(strcmp(key, current_node->key) < 0) {
            if(current_node->left == NULL) {
                //if node is meant to be on left, postinglength = 0 to represent left direction
                struct RBnode* new_node = (struct RBnode *) malloc(sizeof(struct RBnode));
                new_node->parent = current_node;
                new_node->postingLength = 0;
                new_node->key = NULL;
                return new_node;
            }
            current_node = current_node->left;
        }
        else {
            if(current_node->right == NULL) {
                //if node is meant to be on right, postinglength = 1 to represent right direction
                struct RBnode* new_node = (struct RBnode *) malloc(sizeof(struct RBnode));
                new_node->parent = current_node;
                new_node->postingLength = 1;
                new_node->key = NULL;
                return new_node;
            }
            current_node = current_node->right;
        }
    }
}

//used to insert into docDictionary, it shifts positions over after doing it. 
//docDictionary is a sorted array storing only words added in a particular document
void insert(char** dictionary, char* word, int index, int size) {
    int j;
    //moving elements over one to make room
    for(j=size;j>index;j--) {
        dictionary[j] = dictionary[j-1];
    }
    //adding to dictionary
    dictionary[index] = (char *) malloc(sizeof(char) * (strlen(word)+1));
    strcpy(dictionary[index], word);
}

//creating the main index file via recursion into the red and black tree
void printout(struct RBnode* node, FILE* index) {
    int i;
    if(node == NULL) {
        return;
    }
    //goes to left part of tree
    printout(node->child[0], index);

    //prints out string and posting length
    fprintf(index, "%s:%d:", node->key, node->postingLength);
    //prints out postings
    for(i=0;i<node->postingLength;i++) {
        fprintf(index, "%d,%.2f ", node->postings[i], (node->frequencies[i]*1000));
    }
    fputs("\n", index);
    count++;
    
    //goes to right part of tree
    printout(node->child[1], index);
}

//freeing up allocated memory in the red and black tree
void clearout(struct RBnode* node) {
    if(node == NULL) {
        return;
    }
    free(node->postings);
    free(node->frequencies);
    free(node->key);
    clearout(node->child[0]);
    clearout(node->child[1]);
    free(node);
}

//Searching the docDictionary
//If in the dictionary return the index of the word
//if not in the dictionary insert the negative of the index of insertion -1
//(-1 off of the index so that inserting at 0 is repreented by a negative number)
int searchDictionary(char** dictionary, char* word, int size) {
    int i, result, min, max;
    //checking it isnt empty
    if(size == 0) {
        return -1;
    }
    i = size/2;
    min = 0;
    max = size-1;
    while((result = strcmp(dictionary[i], word)) != 0) {
        if(result<0) {
            min = i+1;
        } else {
            max = i-1;
        }
        //hence it is not in the dictionary
        if(max<min) {
            //changes i to the value that represents where we insert new word
            if(min > i) {
                i++;
            }
            break;
        }
        i = (max+min)/2;
    }
    //adjusting index to ne negative and -1
    if(max<min) {
        return (i * -1) - 1;
    }
    return i;
}

int main() {
    struct RBnode* temp;
    char currentWord[100];
    int maximumAllocated = 0;
    int c, i, k;
    int letterIndex = 0;
    int length=0;
    int docID=0;
    int initialSize = 100;
    char** docDictionary = (char**) malloc(sizeof(char **) * initialSize);
    int docDictionarySize = 0;
    int* counts = (int*) malloc(sizeof(int) * initialSize);
    int maxDocDictionarySize = initialSize;
    char** docIDList = (char **) malloc(sizeof(char **) * initialSize);
    int docIDCount = initialSize;
    struct RBtree tree = {NULL};
    //going through the output of the parser
    for(;(c=getchar()) != EOF; letterIndex++) {
        //signals end of a word
        if(c == ' ') {
            currentWord[letterIndex] = '\0';
            //Sorting out storing docID's separately
            if(length==0) {
                currentWord[letterIndex] = '-';
                length++;
                continue;
            } else if(length == 1) {
                //reallocating space if its too small
                if(docIDCount == docID) {
                    docIDCount *=2;
                    docIDList = (char **) realloc(docIDList, sizeof(char **) * docIDCount);
                }
                //adding docID to docIDList
                docIDList[docID] = (char *) malloc(sizeof(char) * (strlen(currentWord)+1));
                strcpy(docIDList[docID], currentWord);
                letterIndex = -1;
                length++;
                continue;
            }
            //binary search to find word in docDictionary
            i = searchDictionary(docDictionary, currentWord, docDictionarySize);
            letterIndex = -1;
            //i is negative means that it is not in the docDictionary
            if(i < 0) {
                i = (i+1) * -1;
                //adding to the docDictionary
                //reallocating size if the docDictionary is too small
                if(docDictionarySize == maxDocDictionarySize) {
                    maxDocDictionarySize *= 2;
                    docDictionary = (char **) realloc(docDictionary, sizeof(char *) * maxDocDictionarySize);
                    counts = (int *) realloc(counts, sizeof(int) * maxDocDictionarySize);
                }
                //checking if space needs to be deallocatged first before inserting
                if(docDictionarySize < maximumAllocated) {
                    free(docDictionary[docDictionarySize]);
                }
                insert(docDictionary, currentWord, i, docDictionarySize);
                //adjust counts
                for(k=docDictionarySize;k>i;k--) {
                    counts[k] = counts[k-1];
                }
                docDictionarySize++;
                if(docDictionarySize>maximumAllocated) {
                    maximumAllocated = docDictionarySize;
                }
                //adding to counts and indexes
                counts[i] = 1;
            //in the case the word is already in docDictionary
            } else {
                counts[i]++;
            }
            letterIndex = -1;
            length++;
        //new line means new document
        } else if(c == '\n') {
            //updating frequencies and postings
            for(i=0;i<docDictionarySize;i++) {
                struct RBnode* node;
                //finding the node in the red and black tree
                node = search(docDictionary[i], &tree);
                //need to insert into tree
                if(node->key == NULL) {
                    node->key = (char *) malloc((strlen(docDictionary[i])+1)*sizeof(char));
                    strcpy(node->key, docDictionary[i]);
                    //postingLength temporarily represented whether node is left or right child
                    int dir = node->postingLength;
                    RBinsert1(&tree, node->parent, node, dir);
                    node->postings = (int *) malloc(sizeof(int));
                    node->postings[0] = docID;
                    node->frequencies = (float *) malloc(sizeof(float));
                    node->frequencies[0] = ((float) counts[i]) / ((float) length);
                    node->postingMaxLength = 1;
                    node->postingLength = 1;
                } else {
                    //node already in tree so adding to postings
                    //checking is there is enough space allocated
                    if(node->postingLength == node->postingMaxLength) {
                        node->postingMaxLength *= 2;
                        node->postings = (int *) realloc(node->postings, sizeof(int) * node->postingMaxLength);
                        node->frequencies = (float *) realloc(node->frequencies, sizeof(float) * node->postingMaxLength);
                    }
                    //adding to postings and frequencies
                    node->postings[node->postingLength] = docID;
                    node->frequencies[node->postingLength] = ((float) counts[i]) / ((float) length);
                    node->postingLength++;
                }
            }
            //resetting variables
            docDictionarySize = 0;
            docID++;
            letterIndex = -1;
            length = 0;
        } else {
            //append letter to currentWord
            currentWord[letterIndex] = (char) c;
        }
    }
    //printing
    FILE *fp = fopen("./index", "w");
    printout(tree.root, fp);
    fclose(fp);
    fp = fopen("./index_size", "w");
    fprintf(fp, "%d\n", count);
    fprintf(fp, "%d", docID);
    fclose(fp);
    fp = fopen("./IDlist", "w");
    int j;
    for(i=0;i<docID;i++) {
        //capitailising WSJ which was made lowercase by the parser
        for(j=0;j<3;j++) {
            docIDList[i][j] = toupper(docIDList[i][j]);
        }
        fprintf(fp, "%s\n", docIDList[i]);
    }
    //freeing all allocatted memory
    clearout(tree.root);
    
    for(i=0;i<docID;i++) {
        free(docIDList[i]);
    }
    free(docIDList);
    for(i=0;i<maximumAllocated;i++) {
        free(docDictionary[i]);
    }
    free(docDictionary);
    free(counts);
    return 0;
}