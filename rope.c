/**
 * NUME #1: Cornea Alexandra
 * NUME #2: Dumitrescu Alexandra
 * GRUPA: 313CA
**/

#include "./rope.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "utils.h"

#define EMPTY ""


/**
 * Functii de CREATE:
 *
 *      -- Returneaza structura de RopeNode* - respectiv RopeTree* -, careia
 *         i-au fost alocate si initializate campurile. Se verifica succesul
 *         operatiilor prin DIE()
 *
 *      -- Observatie:   
 *         Nodul are atribuita o greutate. In functia de create, ii dam
 *         greutatea lungimea stringului  (1). In cazul in care nodul
 *         nou NU este frunza, greutatea ii va fi actualizata in
 *         momentul adaugarii sale in rope.   
 *         Vezi si:     (2)
**/


RopeNode* makeRopeNode(const char* str) {
    RopeNode *r_node = malloc(sizeof(RopeNode));
    DIE(r_node == NULL, "malloc failed");

    r_node->left = NULL;
    r_node->right = NULL;

    r_node->str = str;
    r_node->weight = strlen(str);   /* (1) */

    return r_node;
}

RopeTree* makeRopeTree(RopeNode* root) {
    RopeTree* r_tree = malloc(sizeof(RopeTree));
    DIE(r_tree == NULL, "malloc failed");

    r_tree->root = root;

    return r_tree;
}


/**
 * Functii folosite pentru afisarea elementelor dintr-un rope.
 *
 * Se parcurge arborele pornind de la o radacina, parcurgem
 * subarborele stang si cel drept pana gasim o frunza, careia ii afisam
 * stringul.
 *
**/

void printRopeNode(RopeNode* rn) {
    if (!rn)
        return;

    if (!(rn->left) && !(rn->right)) {
        printf("%s", rn->str);
        return;
    }

    printRopeNode(rn->left);
    printRopeNode(rn->right);
}

void printRopeTree(RopeTree* rt) {
    if (rt && rt->root) {
        printRopeNode(rt->root);
        printf("%s", "\n");
    }
}

void debugRopeNode(RopeNode* rn, int indent) {
    if (!rn)
        return;

    for (int i = 0; i < indent; ++i)
        printf("%s", " ");

    if (!strcmp(rn->str, EMPTY))
        printf("# %d\n", rn->weight);
    else
        printf("%s %d\n", rn->str, rn->weight);

    debugRopeNode(rn->left, indent+2);
    debugRopeNode(rn->right, indent+2);
}


/**
 * Functie care actualizeaza greutatea unui nod.    (2)
 * 
 * Fiecare nod are retinuta lungimea stringului din subarborele stang, deci
 * greutatea parintelui sau va fi de fapt suma dintre greutatea fiului stang
 * si greutatile tuturor descendentilor din dreapta ai fiului stang. (C)
 *
 * Ex:
 *        X         | Greutatea lui X va fi suma dintre greutatea fiului stang                   
 *       /          | (2 - lungimea stringului ab) si greutatile tuturor
 *      2           | descendentilor din dreapta (1 pentru stirngul c)
 *     / \          | (si 2 pentru lungimea stringului de).  Nu mai este nevoie
 *    2   1         | vizitarea fiilor din stanga.
 *   ab  / \
 *      1   2
 *      c   de
 *
**/

int getTotalWeight(RopeNode* rt) {      /* (2) */
    if (!rt)
        return 0;

    return rt->weight + getTotalWeight(rt->right);
}


/**
 * Functii de CONCAT
 *
 * Pentru 2 rope-uri trimise ca parametru, se returneaza
 * un nou rope care le concateneaza astfel:
 * Radacina noului rope va avea ca fiu stang radacina
 * primului, iar ca fiu drept radacina celui de-al 2lea.
 *
 *     new_root        
 *      /    \         
 *  root#1  root#2  
 * 
 *
 * Am implementat si o functie de concatenare a 2 radacini trimise
 * ca parametrii ce returneaza doar o radacina pentru un nou rope
 * pentru a evita alocarea memoriei pentru un nou arbore.
 *
**/


RopeTree* concat(RopeTree* rt1, RopeTree* rt2)
{
    if (rt1 == NULL || rt2 == NULL)
    	return NULL;

    /* o alternativa pentru strdup */
    char *aux = malloc(strlen(EMPTY) + 1);
    strcpy(aux, EMPTY);

    RopeNode* new_root = makeRopeNode(aux);

    RopeTree* new_rt = makeRopeTree(new_root);
    new_root->left = rt1->root;
    new_root->right = rt2->root;

    new_root->weight = getTotalWeight(rt1->root);
    return new_rt;
}


RopeNode* concat_nodes(RopeNode* root1, RopeNode* root2) {
    if (root1 == NULL || root2 == NULL)
        return NULL;

    /* o alternativa pentru strdup */
    char *aux = malloc(strlen(EMPTY) + 1);
    strcpy(aux, EMPTY);

    RopeNode* new_root = makeRopeNode(aux);

    new_root->left = root1;
    new_root->right = root2;

    new_root->weight = getTotalWeight(root1);

    return new_root;
}


/**
 * Operatia de SEARCH in rope
 *
 * Ne folosim de (C) => se eficientizeaza cautarea intr-un rope.
 *
 * Daca vrem sa cautam un index in stringul din rope, verificam la fiecare
 * pas daca indexul este mai mic decat greutatea nodului, caz in care ne
 * deplasam la stanga, sau daca este mai mare, caz in care
 * ce cautam se afla in subarborele drept si trebuie scazut din index valoarea
 * greutatii.   Vezi: (4)
 *
 * Observatii:
 *      -- Functia findLeaf() returneaza un pointer la frunza de unde putem
 *         extrage informatia din stringul pe care il retine.
 *
 *      -- Indexarea intr-un string incepe de la 0, iar in rope de la 1, astfel
 *         trebuie adaugata o unitate in indexul primit ca parametru
 *         Dupa ce am efectuat cautarea, in stringul din frunza gasita, indexul
 *         trebuie scazut din nou cu o unitate.    Vezi:  (3)
 *
 *              Ex:        1      | index(0) -> idx = 0
 *                        / \     |             idx = 1 -> findLeaf returneaza
 *                       1   1    |                        frunza ce retine 'a'
 *                       a   b    |             idx = 0 -> din stringul retinut
 *                                |                        de frunza ne trebuie
 *                                |                        caracterul de pe
 *                                |                        pozitia 0.
**/                 


RopeNode* findLeaf(RopeNode* root, int *idx)    /* (4) */
{
    if (root->left == NULL && root->right == NULL)
        return root;

    if (*idx <= root->weight) {
        return findLeaf(root->left, idx);
    } else {
        *idx = *idx - root->weight;
        return findLeaf(root->right, idx);
    }
    return root;
}

char indexRope(RopeTree* rt, int idx) {
    if (rt == NULL)
    	return 0;

    idx = idx + 1;  /* (3) */
    RopeNode* aux = findLeaf(rt->root, &idx);
    idx = idx - 1;  /* (3) */

    return aux->str[idx];
}


char* search(RopeTree* rt, int start, int end) {
    if (rt == NULL)
        return NULL;

    char *aux = malloc(end - start + 1);
    DIE(aux == NULL, "Malloc failed");

    int i, j;

    for (i = start, j = 0; i < end; i++, j++)
        aux[j] = indexRope(rt, i);

    aux[j] = '\0';
    return aux;
}



/**
 * Operatia de SPLIT
 * 
 *  Impartim operatia in 3 sub-probleme: 
 *
 *
 *  A.  Operatia NU urmareste distrugerea rope-ului initial.
 *
 *      Ne folosim de o clona a arborelui pentru a pastra intact
 *      arborele initial. (O data taiate legaturile in ropeul inital
 *      acesta nu mai poate fi refolosit)
 *     
 *      Clonarea arborelui se face prin functia clone_subtree().
 *      Se parcurge de la radacina fiecare nod si se copiaza continutul
 *      nodului intr-un nou nod. La final se returneaza radacina.
 *      Vezi: (6), (5)
**/


RopeNode *clone_node(RopeNode *root)  /* (5) */
{
    /* Se copiaza stringul */
    char *new_string = malloc(strlen(root->str) + 1);
    strcpy(new_string, root->str);

    RopeNode *new_node = makeRopeNode(new_string);

    /* Se copiaza greutatea*/
    new_node->weight = root->weight;

    return new_node;
}



RopeNode *clone_subtree(RopeNode *root)  /* (6) */
{
    if (root == NULL)
        return NULL;

    RopeNode *new_node = clone_node(root);

    new_node->left = clone_subtree(root->left);
    new_node->right = clone_subtree(root->right);

    return new_node;
}


/**
 *
 * B.  Se trateaza cazul in care operatia trebuie sa imparta frunza in alte 2
 *     frunze.        Vezi: (7)
 *          Ex:
 *              3       | Split(2)   ->   idx = 2
 *             / \      | Se apeleaza functia de find_leaf() pentru a returna
 *            3   1     | frunza de unde incepem splitul.
 *           abc  d     | Daca idx < strlen(abc) inseamna ca trebuie sa
 *                      | impartim frunza astfel:
 *                      |   prima frunza contine idx caractere
 *                      |   a doua frunza contine strlen(abc) - idx caractere
 *
 *              3       | Pentru a putea cauta string-urile in care se imparte
 *             / \      | frunza ne folosim de functia search
 *            2   1
 *           / \  d
 *          ab  c
 * 
 *  start = indexul unde vrem sa facem taierea - indexul actualizat dupa
 *          operatia find_leaf (vrem sa ajungem la indexul de inceput al frunzei)
 *        = 2 - 2 = 0
 *  idx = 2
 *  end = start + strlen(string al frunzei)
 *      = 0 + strlen(abc) = 3
 *
 * Deci, prima frunza contine stringul returnat de search(rt, start, idx);
 * iar a doua search(idx, end).
 *
 *
**/


void split_leaf(RopeNode *leaf, RopeNode* root, int start, int idx, int end)
{       /* (7) */
    RopeNode *left_son;
    RopeNode *right_son;

    RopeTree *rt = makeRopeTree(root);

    char *left_string = search(rt, start, idx);
    char *right_string = search(rt, idx, end);

    free(rt);

    left_son = makeRopeNode(left_string);
    right_son = makeRopeNode(right_string);

    leaf->left = left_son;
    leaf->right = right_son;

    free((void *)((leaf)->str));

    /* o alternativa pentru strdup */
    char *aux = malloc(strlen(EMPTY) + 1);
    strcpy(aux, EMPTY);

    leaf->str = aux;

    leaf->weight = getTotalWeight(leaf->left);
}


/**
 * C. Construirea subarborelui stang si drept
 *
 * Se imparte arborele (clona) in 2 subarbori:
 *      subarborele stang - contine doar nodurile aflate la stanga
 *                          lantului parcurs de la radacina pana la frunza
 *                          impreuna cu nodurile din lant
 *
 *      subarborele drept - contine toti descendentii aflati la dreapta
 *                          nodurilor din lant, care sunt concatenati
 *                          pentru a construi tot un rope.
 *
 * In apelul recursiv ne deplasam simultan cu nodul si cu parintele sau.
 *
 * Daca nodul ajunge sa fie frunza, verificam daca este fiu drept al
 * parintelui, caz in care nu se intampla nimic, sau fiu stang, caz in care
 * se concateneaza la pair.right fiul drept si se rupe legatura parintelui
 * cu acesta.   (Avem grija sa nu se concateneze la pair.right un nod NULL)
 *
 * Ne deplasam prin arbore ca la search(). De fiecare data cand ne intoarcem
 * din apelul recursiv verificam daca putem rupe legatura parintelui cu fiul
 * drept si actualizam greutatea parintelui.
 *
 * Vezi exemplu (8)
**/


void check_right_son(RopeNode *parent, RopeNode *son, SplitPair *pair)
{
    if (parent->right != son  && parent->right != NULL) {
        if (pair->right == NULL)
            pair->right = parent->right;
        else
            pair->right = concat_nodes(pair->right, parent->right);
        parent->right = NULL;
    }
}

void __split(RopeNode *node, RopeNode *parent, int *idx, SplitPair *pair)
{
    if (node == NULL)
        return;

    if (node->left == NULL && node->right == NULL) {
        check_right_son(parent, node, pair);
        return;
    }
    if (*idx <= node->weight) {
        __split(node->left, node, idx, pair);

        parent->weight = getTotalWeight(parent->left);
        check_right_son(parent, node, pair);
    } else {
        *idx = *idx - node->weight;
        __split(node->right, node, idx, pair);

        parent->weight = getTotalWeight(parent->left);
        check_right_son(parent, node, pair);
    }
}

/**
 * In final, subarborele drept va fi cel construit prin concatenari in __split
 * iar arborele stang ramane radacina clonei folosite.
 *
 * Inainte de apelul __split cautam parintele si fiul cu care incepem apelul
 * recursiv si tratam cazul in care taietura se face fix la radacina.
 *
**/

SplitPair split(RopeTree* rt, int idx)
{
    RopeNode *new_root = clone_subtree(rt->root);   /* (A) */

    SplitPair pair;
    pair.left = pair.right = NULL;

    /* (B) */
    int idx_copy = idx;
    RopeNode *leaf = findLeaf(new_root, &idx_copy);

    if (leaf->weight > idx_copy) {
        int start =  idx - idx_copy;
        int end = idx - idx_copy + leaf->weight;

        split_leaf(leaf, new_root, start, idx, end);
    }

    /* (C) */
    RopeNode* son;
    if (idx == new_root->weight) {
        pair.right = new_root->right;
        new_root->right = NULL;
        pair.left = new_root;
        return pair;
    }
    if (idx <= new_root->weight) {
        son = new_root->left;
    } else {
        son = new_root->right;
        idx = idx - new_root->weight;
    }

    __split(son, new_root, &idx, &pair);
    pair.left = new_root;

    /* Cazul in care una din perechi este goala */
    if (pair.left == NULL) {
        char *aux = malloc(strlen(EMPTY) + 1);
        strcpy(aux, EMPTY);
        pair.left = makeRopeNode(aux);
    }
    if (pair.right == NULL) {
        char *aux = malloc(strlen(EMPTY) + 1);
        strcpy(aux, EMPTY);
        pair.right = makeRopeNode(aux);
    }

    return pair;
}


/**
 * Operatia de INSERT       (8)
 * 
 * Se reduce la aplicarea unui split si a doua concatenari.
 * Ex: insert(2, "str")
 *                                  pair.left   pair_right
 *     3                3               2           1
 *    / \      =>      / \     =>      /           / \
 *   3   1            2   1           2           1   1
 *  abc  d           / \  d          /            c   d
 *                  2   1           2
 *                 ab   c          ab
 * 
 *  Se concateneaza la pair.left un nou nod ce contine new_string, iar apoi
 *  se concateneaza la radacina rezultata pair.right.
 *
 *  concat(pair.left, new_node)         concat(new_root, pair.right)
 *       2                                       2
 *      / \                                    /   \
 *     2   3                                  2     1
 *    /    str                  =>           / \   / \
 *   2                                      2   3 1   1
 *  /                                      /  str c   d
 * 2                                      2
 * ab                                    /
 *                                      2
 *                                      ab
**/
RopeTree* insert(RopeTree* rt, int idx, const char* str) {
    if (rt == NULL)
        return NULL;

    SplitPair pair = split(rt, idx);

    RopeNode *new_node = makeRopeNode(str);
    RopeNode *new_root = concat_nodes(pair.left, new_node);

    RopeNode *new = concat_nodes(new_root, pair.right);
    RopeTree *new_tree = makeRopeTree(new);
    return new_tree;
}

/**
 * Operatia de DELETE
 * 
 * Se procedeaza asemanator ca la insert.
 * Se aplica 2 operatii de split asupra ropeului initial si se concateneaza
 * pair1.left cu pair2.right. Memoria alocata de pair1.right si pair2.left este
 * eliberata.
 *
**/

void destroy(RopeNode *rn)
{
    if (rn == NULL)
        return;

    free((void*)rn->str);
    destroy(rn->left);
    destroy(rn->right);
    free(rn);
}


RopeTree* delete(RopeTree* rt, int start, int len) {
    if (rt == NULL)
        return NULL;

    SplitPair pair1 = split(rt, start);
    SplitPair pair2 = split(rt, start + len);

    RopeNode *new_node = concat_nodes(pair1.left, pair2.right);

    destroy(pair1.right);
    destroy(pair2.left);

    return makeRopeTree(new_node);
}


