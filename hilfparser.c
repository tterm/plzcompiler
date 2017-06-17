#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct type *Type;

struct type {
    int op;
};

typedef struct symbol *Symbol;

struct symbol {
    char name[20];
    int scope;
    Type type;
    Symbol next;
};

Symbol start_ptr = NULL;
Liste sta_ptr = NULL;

typedef struct liste *Liste;

struct liste {
    char name[20];
    Symbol liste;
    int scope;
    Liste next;
};
Liste ptr:

        void liste_hinzu(int sco, char *name, int t)
{

    Symbol pointer;
    ptr = (Liste) malloc(sizeof(struct liste));

    strcpy(ptr->name, "global");
    ptr->next = sta_ptr;
    sta_ptr = ptr;

}

void hinzu(char *ide, int sco) {
    Symbol pointer;
    pointer = (Symbol) malloc(sizeof(struct symbol));
    strcpy(pointer->name, ide);
    pointer->scope = sco;
    pointer->next = start_ptr;
    start_ptr = pointer;
    ptr->liste = start_ptr;
}


void ausgeben() {
    Liste ptr_neu = sta_ptr;
    Symbol pointer = start_ptr;
    while (ptr_neu != NULL) {
        while (pointer != NULL) {
            printf("%s , %d \n", pointer->name, pointer->scope);
            pointer = pointer->next;
        }
    }
}

