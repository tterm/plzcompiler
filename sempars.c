#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"
#include"code.h"

#define CONSTANTE 0
#define VARIABLE 1
#define FUNCTION 2


Mainlist start_ptr = NULL;
Mainlist last_ptr = NULL;

/*Mainlist search_func(char *); */

void init_mainlist() {

}

void add_struct(Mainlist ml) {
    if (start_ptr == NULL) {
        start_ptr = ml;
    } else {
        last_ptr->next = ml;
    }

    last_ptr = ml;
}

void add_var(char *name, int scope, int type) {
    Variable vari;
    Mainlist mlist;
    vari = (Variable) malloc(sizeof(struct variable));
    mlist = (Mainlist) malloc(sizeof(struct mainlist));
    mlist->v = vari;
    strcpy(mlist->v->name, name);
    mlist->v->scope = scope;
    mlist->v->type = type;
    mlist->v->value[0] = '\0';
    mlist->c = NULL;
    mlist->f = NULL;
    mlist->flag = VARIABLE;
    mlist->next = NULL;

    add_struct(mlist);
}

void add_const(char *name, int sco, int type, char *value) {
    Constante cons;
    Mainlist mlist;
    cons = (Constante) malloc(sizeof(struct constante));
    mlist = (Mainlist) malloc(sizeof(struct mainlist));
    mlist->c = cons;
    mlist->v = NULL;
    mlist->f = NULL;
    mlist->flag = CONSTANTE;
    strcpy(mlist->c->name, name);
    mlist->c->scope = sco;
    mlist->c->type = type;
    strcpy(mlist->c->value, value);
    mlist->next = NULL;

    add_struct(mlist);

}

Mainlist search_func(char *name) {
    Mainlist ptr;
    ptr = start_ptr;
    while (ptr != NULL) {
        if (ptr->flag == FUNCTION) {
            Function temp;
            temp = ptr->f;
            if (temp != NULL) {
                if (strcmp(temp->name, name) == 0)
                    return ptr;
            }
        }

        ptr = ptr->next;
    }
    return NULL;
}

/* wenn return 0 dann muss kopf hinzugefuegt werden */
/* wenn return 1 dann ist schon vorhanden */
/* wenn return 2 dann werden variablen aus rumpf hinzugefuegt */
int add_func(char *id) {
    Mainlist ml = search_func(id);
    if (ml != NULL) {
        if (ml->f->k != NULL && ml->f->r != NULL)
            return 1;
        else if (ml->f->k != NULL)
            return 2;
        else if (ml->f->r != NULL)
            return 3;
        else
            return 4;
    } else {
        Function funci;
        Mainlist mlist;
        funci = (Function) malloc(sizeof(struct function));
        funci->k = NULL;
        funci->r = NULL;
        strcpy(funci->name, id);
        mlist = (Mainlist) malloc(sizeof(struct mainlist));

        mlist->f = funci;
        mlist->c = NULL;
        mlist->v = NULL;
        mlist->flag = FUNCTION;
        mlist->next = NULL;
        add_struct(mlist);
        return 0;
    }
    return -1;
}

int search_global_const(char *name, int const_scope) {
    Mainlist ptr;
    ptr = start_ptr;
    while (ptr != NULL) {
        if (ptr->c != NULL && ptr->c->scope == const_scope) {
            if (strcmp(ptr->c->name, name) == 0) {
                return 1;
            }

        }
        ptr = ptr->next;
    }
    return 0;
}

int search_global_var(char *name, int var_scope) {
    Mainlist ptr;
    ptr = start_ptr;
    while (ptr != NULL) {
        if (ptr->v != NULL && ptr->v->scope == var_scope) {
            if (strcmp(ptr->v->name, name) == 0) {
                return 1;
            }
        } else if (ptr->c != NULL && ptr->c->scope == var_scope) {
            if (strcmp(ptr->c->name, name) == 0) {
                return 1;
            }
        }
        ptr = ptr->next;
    }
    return 0;
}

int search_var_in_main(char *functionname, char *var_name) {
    Kopf k_temp;
    Rumpf temp;
    Mainlist ml = search_func(functionname);

    if (ml != NULL) {
        k_temp = ml->f->k;
        while (k_temp != NULL) {
            if (k_temp->var != NULL) {
                if (strcmp(k_temp->var->name, var_name) == 0) {
                    return 1;
                }

            }
            k_temp = k_temp->next;
        }
        temp = ml->f->r;
        while (temp != NULL) {
            if (temp->con != NULL) {
                if (strcmp(temp->con->name, var_name) == 0) {
                    return 1;
                }
            } else if (temp->var != NULL) {
                if (strcmp(temp->var->name, var_name) == 0) {
                    return 1;
                }
            }
            temp = temp->next;
        }
        return 0;
    }
    return 1;

}

/*int serch_var_in_function(char *functionname,char var_name)
{
	Kopf k_temp;
	Rumpf temp;
	Mainlist
}
*/
/* return 1 wenn function noch nicht da ist fehler */
int add_func_kopf(char *id, char *varname, int typ, int in_out, int sco) {
    Mainlist ml = search_func(id);

    if (ml != NULL) {
        if (ml->f->k == NULL) {
            Kopf ko;
            Variable _var;
            _var = (Variable) malloc(sizeof(struct variable));
            ko = (Kopf) malloc(sizeof(struct kopf));
            ml->f->k = ko;
            ko->next = NULL;
            ko->var = _var;
            strcpy(ko->var->name, varname);
            ko->var->type = typ;
            ko->var->scope = sco;
            ko->flag = in_out;
            ko->var->value[0] = '\0';

        } else {
            Kopf alter_kopf;
            Kopf ko;
            Variable _var;
            _var = (Variable) malloc(sizeof(struct variable));
            ko = (Kopf) malloc(sizeof(struct kopf));
            alter_kopf = ml->f->k;
            while (alter_kopf->next != NULL) {
                alter_kopf = alter_kopf->next;
            }
            ko->next = NULL;
            ko->var = _var;
            strcpy(ko->var->name, varname);
            ko->var->type = typ;
            ko->var->scope = sco;
            ko->flag = in_out;
            ko->var->value[0] = '\0';
            alter_kopf->next = ko;

        }
        return 0;
    }

    return 1;
}

/* return 1 wenn function noch nicht da ist fehler */
int add_func_rumpf(char *id, char *varname, int typ, int flag, int scope, char *value) {
    Mainlist ml = search_func(id);
    if (ml != NULL) {
        if (ml->f->r == NULL) {
            if (flag == VARIABLE) {
                Variable _var;
                Rumpf ru;
                _var = (Variable) malloc(sizeof(struct variable));
                ru = (Rumpf) malloc(sizeof(struct rumpf));
                ml->f->r = ru;
                ru->next = NULL;
                ru->var = _var;
                ru->con = NULL;
                ru->flag = VARIABLE;
                strcpy(ru->var->name, varname);
                ru->var->type = typ;
                ru->var->scope = scope;
                ru->var->value[0] = '\0';
            } else if (flag == CONSTANTE) {
                Constante _con;
                Rumpf ru;
                _con = (Constante) malloc(sizeof(struct constante));
                ru = (Rumpf) malloc(sizeof(struct rumpf));
                ml->f->r = ru;
                ru->next = NULL;
                ru->con = _con;
                ru->var = NULL;
                ru->flag = CONSTANTE;
                strcpy(ru->con->name, varname);
                ru->con->type = typ;
                ru->con->scope = scope;
                strcpy(ru->con->value, value);
            }

        } else {
            if (flag == VARIABLE) {
                Rumpf alter_rumpf;
                Rumpf ru;
                Variable _var;
                _var = (Variable) malloc(sizeof(struct variable));
                ru = (Rumpf) malloc(sizeof(struct rumpf));
                alter_rumpf = ml->f->r;
                while (alter_rumpf->next != NULL) {
                    alter_rumpf = alter_rumpf->next;
                }
                ru->next = NULL;
                ru->var = _var;
                strcpy(ru->var->name, varname);
                ru->flag = VARIABLE;
                ru->var->type = typ;
                ru->var->scope = scope;
                ru->var->value[0] = '\0';
                alter_rumpf->next = ru;
            } else if (flag == CONSTANTE) {
                Rumpf alter_rumpf;
                Rumpf ru;
                Constante _con;
                _con = (Constante) malloc(sizeof(struct constante));
                ru = (Rumpf) malloc(sizeof(struct rumpf));
                alter_rumpf = ml->f->r;
                while (alter_rumpf->next != NULL) {
                    alter_rumpf = alter_rumpf->next;
                }
                ru->next = NULL;
                ru->con = _con;
                strcpy(ru->con->name, varname);
                ru->flag = CONSTANTE;
                ru->con->type = typ;
                ru->con->scope = scope;
                strcpy(ru->con->value, value);
                alter_rumpf->next = ru;
            }

        }
        return 0;
    }
    return 1;

}


/* rekursives loeschen der Symboltabelle */
void destroy_var(Variable ptr) {
    free(ptr);
}

void destroy_const(Constante ptr) {
    free(ptr);
}

void destroy_kopf(Kopf ptr) {
    if (ptr->next != NULL) {
        destroy_kopf(ptr->next);
    }
    if (ptr->var != NULL) {
        destroy_var(ptr->var);
    }
    free(ptr);
}

void destroy_rumpf(Rumpf ptr) {
    /*fertig machen */
    free(ptr);
}

void destroy_function(Function ptr) {
    if (ptr->r != NULL) {
        destroy_rumpf(ptr->r);
    }
    if (ptr->k != NULL) {
        destroy_kopf(ptr->k);
    }
    free(ptr);
}

void destroy_main(Mainlist ptr) {
    if (ptr != NULL) {
        if (ptr->next != NULL) {
            destroy_main(ptr->next);
        }
        if (ptr->c != NULL) {
            destroy_const(ptr->c);
        }
        if (ptr->f != NULL) {
            destroy_function(ptr->f);
        }
        if (ptr->v != NULL) {
            destroy_var(ptr->v);
        }
        free(ptr);
    }

}

void destroy() {
    destroy_main(start_ptr);
}

void ende() {
    destroy();
    destroy_code();
    exit(1);
}

void endeende() {
    destroy();
}
