/*
 	Autor:			Thomas Termin
	M.-Nr.:			139929
	Freie Mitarbeiter:	Marc Siekmann,	Morris Milekovic`
	M.-Nr:			439956,		539892
*/



#include "plz_glob.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"


#include "code.h"

#define in 1
#define out 2

FILE *codef;
FILE *token;
char tokenfile[30];
char quellfile[30];
char codefile[30];

int namenzaehler2 = 1000;
int namenzaehler = 0;
int zeilenzahl;
int symbol;
char id[20];
float double_zahl;
int fixed_zahl;
int i;
int io;
int global_scope = FALSE;
int boolmain = FALSE;
int var_fuer_main = FALSE;
int in_function = FALSE;
int PRN = FALSE;

char const_name[20];
int const_scope;
int const_type;
char const_value[30];
int zaehler2 = 0;
int var_scope;
int bool_fuer_func = FALSE;
char functionname[20];
char aufzu_functionname[20];
int temp_data_type;
int rueckgabe;

In_variablen invar_start = NULL;
In_variablen invar;

Mainlist mliste;

struct temp_variable {
    char name[30];
    int scope;
};

struct temp_variable temp_variable_liste[20];

/* Funktionsprototypen */
void einlesen(int);

void vergleichen_der_parameter(char *);

int search_var_in_function();

void einlesen_in_temp_liste();

int search_var_in_main(char *, char *);

int add_func_rumpf(char *, char *, int, int, int, char *);

int search_global_var(char *, int);

void add_var(char *, int, int);

int var_search(char *, int);

void pruefe_main();

int search_global_const(char *, int);

int const_search(char *, int);

void add_const(char *, int, int, char *);

int search_var(char *);

int add_func_kopf(char *, char *, int, int, int);

void endeende();

void ende();

int add_func(char *);

void getSymbol();

void program();

void prototype();

void common();

void declarations();

void consts();

void constdecl();

void variables();

void varlist(int);

void type();

void elementarytype();

void procedure();

void parameterlist();

void insection();

void outsection();

void statementseq();

void statement();

void assignment();

void procedurecall();

void ifstatement();

void whilestatement();

void repstatement();

void newstatement();

void dispstatement();

void iostatement();

void actualpars();

void factor();

void term();

void simpleexpression();

void expression();

void identfunc();

void numberfunc();

void textfunc();

/* geaenderte Produktionen da Grammatik vorher nicht LL(1) */
void assignment_procedurecall();

void whilestatement_repstatement();

void io_statement_put();

void io_statement_get();


int sucheVar(char *functionname, char *name) {
    Faddress fl;
    fl = hole_faddresse();


    while (fl != NULL) {
        if ((strcmp(fl->functionname, functionname) == 0) && (strcmp(fl->name, name) == 0))
            return 1;
        fl = fl->next;
    }
    return -1;
}

int sucheScope(char *functionname, char *name) {
    int res = -1;
    res = sucheVar(functionname, name);
    if (res == -1) {
        res = sucheVar("\0", name);
        if (res == 1) return 0;
    }
    return res;
}

int getAnzOut(char *f_name) {
    int cnt = 0;
    Mainlist ml;
    Kopf k_temp;
    ml = search_func(f_name);
    k_temp = ml->f->k;
    while (k_temp != NULL) {
        if (k_temp->flag == out) {
            cnt++;
        }
        k_temp = k_temp->next;
    }
    return cnt;
}

int anzahl_parameter(char *aufrufender_functionname) {
    int zaehler1 = 0;
    Kopf k_temp;
    Mainlist ml;
    ml = search_func(aufrufender_functionname);
    if (ml != NULL) {
        if (ml->f->k != NULL) {
            k_temp = ml->f->k;
            while (k_temp != NULL) {
                zaehler1++;
                k_temp = k_temp->next;
            }
            if (zaehler1 != zaehler2) {
                printf("Anzahl der Parameter stimmen nicht beim aufruf der Funktion %s \n", aufrufender_functionname);
                ende();
            }
        } else {
            if (zaehler1 != zaehler2) {
                printf("Anzahl der Parameter stimmen nicht beim aufruf der Funktion %s \n", aufrufender_functionname);
                ende();
            }

        }
    }
    return zaehler1;
}

void vergleichen_der_parameter(char *functionname) {
    Kopf k_temp;
    In_variablen iter;
    iter = invar_start;
    mliste = search_func(functionname);
    if (mliste != NULL) {
        if (mliste->f->k != NULL) {
            k_temp = mliste->f->k;
            while (k_temp != NULL) {
                if (iter != NULL) {
                    if (strcmp(k_temp->var->name, iter->name) != 0) {
                        printf("Variablennamen im Kopf stimmen nicht ueberein\n");
                        ende();
                    } else {
                        if (k_temp->var->type != iter->data_type) {
                            printf("Datentypen stimmen nicht ueberein\n");
                            ende();
                        }

                    }
                } else {
                    printf("Variablennamen im Kopf stimmen nicht ueberein\n");
                    ende();
                }
                k_temp = k_temp->next;
                iter = iter->next;
            }
            if (iter != NULL) {
                printf("Variablenkoepfe von Funktion %s stimmen nicht ueberein\n", functionname);
                ende();
            }
        } else {
            if (iter != NULL) {
                printf("Variablenkopf von Funktion %s muss leer sein\n", functionname);
                ende();
            }
        }
    } else {
        printf("Schwerwiegender AusnahmeFehler\n");
        ende();

    }
}

void pruefe_main() {
    if (boolmain == TRUE) {
        printf("Multiple Deklaration von MAIN\n");
        ende();
    }
}

int var_search(char *name, int var_scope) {
    if (search_global_var(name, var_scope) == 0)
        return 0;
    else
        return 1;
}

int const_search(char *name, int const_scope) {
    if (search_global_const(name, const_scope) == 0)
        return 0;
    else
        return 1;
}

int search_var(char *varnam) {
    if (invar_start != NULL) {
        In_variablen v;
        v = invar_start;
        while (v != NULL) {
            if (strcmp(v->name, varnam) == 0) {
                return 1;
            }
            v = v->next;
        }
        return 0;
    } else
        return 0;

    return 5;
}

/* einlesen in eine temporaere Liste */
void einlesen(int in_out) {
    /* fuer IN Variablen */
    if (in_out == in) {
        if (invar_start == NULL) {
            invar = (In_variablen) malloc(sizeof(struct in_variablen));
            strcpy(invar->function_name, functionname);
            strcpy(invar->name, id);
            invar->in_out_f = 1;
            invar->next = NULL;
            invar_start = invar;
        } else {
            In_variablen invar_old;
            In_variablen invari;
            invari = (In_variablen) malloc(sizeof(struct in_variablen));
            invar_old = invar_start;
            while (invar_old->next != NULL) {
                invar_old = invar_old->next;
            }
            strcpy(invari->function_name, functionname);
            strcpy(invari->name, id);
            invari->in_out_f = 1;
            invar_old->next = invari;
        }
    }

        /* fuer out Variablen */
    else if (in_out == out) {
        if (invar_start == NULL) {
            invar = (In_variablen) malloc(sizeof(struct in_variablen));
            strcpy(invar->function_name, functionname);
            strcpy(invar->name, id);
            invar->in_out_f = 2;
            invar->next = NULL;
            invar_start = invar;
        } else {
            In_variablen invar_old;
            In_variablen invari;
            invari = (In_variablen) malloc(sizeof(struct in_variablen));
            invar_old = invar_start;
            while (invar_old->next != NULL) {
                invar_old = invar_old->next;
            }
            strcpy(invari->function_name, functionname);
            strcpy(invari->name, id);
            invari->in_out_f = 2;
            invar_old->next = invari;
        }
    }

        /* fuer globale Variablen */
    else if (in_out == 3) {
        if (invar_start == NULL) {
            invar = (In_variablen) malloc(sizeof(struct in_variablen));
            strcpy(invar->name, id);
            strcpy(invar->function_name, functionname);
            invar->next = NULL;
            invar_start = invar;
        } else {
            In_variablen invar_old;
            In_variablen invari;
            invari = (In_variablen) malloc(sizeof(struct in_variablen));
            invar_old = invar_start;
            while (invar_old->next != NULL) {
                invar_old = invar_old->next;
            }
            strcpy(invari->name, id);
            strcpy(invari->function_name, functionname);
            invar_old->next = invari;
        }
    }

}

void textfunc() {
    if (symbol == ident) {
        printf("%s : %d es wird ein String erwartet keine Parameter \n", quellfile, zeilenzahl);
        ende();
    }

    if (symbol == text) {
        getSymbol();
        if (symbol == rparen) {
            getSymbol();
        } else {
            printf("%s : %d nach PUT TEXT ( fehlt \")\"\n", quellfile, zeilenzahl);
            ende();
        }
    } else {
        printf("%s : %d nach PUT TEXT ( muss ein String kommen\n", quellfile, zeilenzahl);
        ende();
    }

}

void actualpars() {
    if (symbol == text) {
        printf("%s : %d normale Parameter erwartet kein String\n", quellfile, zeilenzahl);
        ende();
    }
    expression();
    strcpy(temp_variable_liste[zaehler2].name, id);
    strcpy(temp_variable_liste[zaehler2 + 1].name, "/0");
    if (PRN == TRUE) {
        prn_anlegen();
    }
    if (symbol == comma) {
        zaehler2++;
        /*	strcpy(temp_variable_liste[zaehler2].name,id); */
        getSymbol();
        actualpars();
    }

}

void repstatement() {
    int name;
    char whilename[2];
    namenzaehler++;
    name = namenzaehler;
    sprintf(whilename, "%d", name);
    while_clause_anlegen(whilename);


    statementseq();
    if (symbol == UNTIL) {
        getSymbol();
        expression();
        jpc_anlegen(whilename);
    } else {
        printf("%s : %d nach DO muss hier UNTIL kommen\n", quellfile, zeilenzahl);
        ende();
    }
}

void whilestatement() {
    int name;
    int name2;
    char whilename[2];
    char whilename2[2];
    namenzaehler++;
    name = namenzaehler;
    name2 = namenzaehler;
    name2++;
    sprintf(whilename, "%d", name);
    sprintf(whilename2, "%d", name2);
    while_clause_anlegen(whilename2);
    expression();
    jpc_anlegen(whilename);
    statementseq();
    jpu_anlegen(whilename2);
    while_clause_anlegen(whilename);

    if (symbol == END) {
        getSymbol();
    } else {
        printf("%s : %d nach DO WHILE muss ein END kommen\n", quellfile, zeilenzahl);
        ende();
    }
}

void newstatement() {
    if (symbol == ident) {
        getSymbol();
    } else {
        printf("%s : %d Nach NEW muss ident kommen\n", quellfile, zeilenzahl);
        ende();
    }
}

void dispstatement() {
    if (symbol == ident) {
        getSymbol();
    } else {
        printf("%s : %d Nach DISPOSE muss ident kommen\n", quellfile, zeilenzahl);
        ende();
    }
}

void ifstatement() {
    int name;
    int name2;
    char ifname[2];
    char ifname2[4];
    namenzaehler++;
    namenzaehler2++;
    name = namenzaehler;
    name2 = namenzaehler2;
    sprintf(ifname2, "%d", name2);
    sprintf(ifname, "%d", name);
    expression();
    if (symbol == THEN) {
        jpc_anlegen(ifname);
        getSymbol();
        statementseq();
        //if_clause_anlegen(ifname);


        if (symbol == ELSEIF) {
            if_clause_anlegen(ifname);
            getSymbol();
            ifstatement();
        } else if (symbol == ELSE) {
            jpu_anlegen(ifname2);
            if_clause_anlegen(ifname);
            getSymbol();
            statementseq();
            if_clause_anlegen(ifname2);
        } else
            if_clause_anlegen(ifname);

    } else {
        printf("%s : %d nach IF muss THEN kommen\n", quellfile, zeilenzahl);
        ende();
    }

}

void procedurecall() {
    if (symbol == rparen) {
        int anzahl;
        anzahl = anzahl_parameter(aufzu_functionname);
        call_anlegen2(aufzu_functionname, anzahl);
        getSymbol();
    } else {
        int anzahl;
        int i, t_cnt = 0;
        int anz_out;
        actualpars();
        if (symbol == rparen) {
            zaehler2++;
            anzahl = anzahl_parameter(aufzu_functionname);
            call_anlegen2(aufzu_functionname, anzahl);
            anz_out = getAnzOut(aufzu_functionname);

            while (strcmp(temp_variable_liste[t_cnt].name, "\0") != 0) {
                temp_variable_liste[t_cnt].scope = sucheScope(functionname, temp_variable_liste[t_cnt].name);
                t_cnt++;
            }
            t_cnt--;
            t_cnt--;
            anz_out--;
            for (i = t_cnt; i >= anz_out; i--) {
                /*sto_anlegen(temp_variable_liste[i].name,functionname,temp_variable_liste[i].scope ); */
            }
            getSymbol();
        } else {
            printf("%s : %d ) fehlt\n", quellfile, zeilenzahl);
            ende();
        }
        zaehler2 = 0;
    }

}

void io_statement_get() {
    if (symbol == LIST) {
        getSymbol();
        if (symbol == lparen) {
            getSymbol();
            actualpars();
            if (symbol == rparen) {
                getSymbol();
            } else {
                printf("%s : %d ) fehlt nach GET LIST (\n", quellfile, zeilenzahl);
                ende();
            }
        } else {
            printf("%s : %d ( fehlt\n", quellfile, zeilenzahl);
            ende();
        }
    } else {
        printf("%s : %d nach GET muss LIST kommen\n", quellfile, zeilenzahl);
        ende();
    }

}

void io_statement_put() {
    PRN = TRUE;
    if (symbol == LIST) {
        getSymbol();
        if (symbol == lparen) {
            getSymbol();
            actualpars();
            if (symbol == rparen) {
                PRN = FALSE;
                getSymbol();
            } else {
                printf("%s : %d ) fehlt nach PUT LIST (\n", quellfile, zeilenzahl);
                ende();
            }
        } else {
            printf("%s : %d ( fehlt\n", quellfile, zeilenzahl);
            ende();
        }
    } else if (symbol == TEXT) {
        getSymbol();
        if (symbol == lparen) {
            getSymbol();
            textfunc();
        } else {
            printf("%s : %d ( fehlt\n", quellfile, zeilenzahl);
            ende();
        }
    } else if (symbol == SKIP) {
        prl_anlegen();
        getSymbol();
    } else {
        printf("%s : %d nach PUT muss LIST oder TEXT oder SKIP kommen\n", quellfile, zeilenzahl);
        ende();
    }
}

void whilestatement_repstatement() {
    if (symbol == WHILE) {
        getSymbol();
        whilestatement();
    } else {
        repstatement();
    }
}

void factor() {
    if (symbol == hoch) {
        getSymbol();
    } else if (symbol == NUMBER_FLOAT || symbol == NUMBER_INT) {
        getSymbol();
    } else if (symbol == lparen) {
        getSymbol();
        expression();
        if (symbol == rparen) {
            getSymbol();
        } else {
            printf(") fehlt in expression\n");
            ende();
        }
    }
}

void term() {

    if (symbol == ident) {
        char name[20];
        char funct[20];
        getSymbol();
        strcpy(name, id);
        strcpy(funct, functionname);
        /* Ueberpruefung ob Variable deklariert ist */
        if (search_var_in_main(functionname, id) == 0) {
            if (search_global_const(id, 0) == 0) {
                if (search_global_var(id, 0) == 0) {
                    printf("Variable %s nicht deklariert\n", id);
                    ende();
                } else {
                    lod_anlegen(id, functionname, 0);
                }
            } else {
                lod_anlegen(id, functionname, 0);
            }
        } else {
            lod_anlegen(id, functionname, 1);
        }
        factor();
        if (symbol == times) {
            getSymbol();
            term();
            mpy_anlegen();
        } else if (symbol == slash) {
            getSymbol();
            term();
            div_anlegen();
        } else if (symbol == DIV) {
            getSymbol();
            term();
        } else if (symbol == MOD) {
            getSymbol();
            term();
        } else if (symbol == AND) {
            getSymbol();
            term();
        }

    } else if (symbol == NUMBER_FLOAT || symbol == NUMBER_INT) {
        if (symbol == NUMBER_FLOAT) {
            flit_anlegen(double_zahl);
        } else if (symbol == NUMBER_INT) {
            ilit_anlegen(fixed_zahl);
        }
        factor();
        if (symbol == times) {
            getSymbol();
            term();
            mpy_anlegen();
        } else if (symbol == slash) {
            getSymbol();
            term();
            div_anlegen();
        } else if (symbol == DIV) {
            getSymbol();
            term();
        } else if (symbol == MOD) {
            getSymbol();
            term();
        } else if (symbol == AND) {
            getSymbol();
            term();
        }
    } else if (symbol == lparen) {
        factor();
        if (symbol == times) {
            getSymbol();
            term();
            mpy_anlegen();
        } else if (symbol == slash) {
            getSymbol();
            term();
            div_anlegen();
        } else if (symbol == DIV) {
            getSymbol();
            term();
        } else if (symbol == MOD) {
            getSymbol();
            term();
        } else if (symbol == AND) {
            getSymbol();
            term();
        }
    } else {
        printf("%s : %d fehlender rvalue\n", quellfile, zeilenzahl);
        ende();
    }

}

void simpleexpression() {
    term();
    if (symbol == plus) {
        getSymbol();
        simpleexpression();
        add_anlegen();
    } else if (symbol == minus) {
        getSymbol();
        simpleexpression();
        sub_anlegen();
    } else if (symbol == OR) {
        getSymbol();
        simpleexpression();
    }
}

void expression() {

    if (symbol == minus) {
        getSymbol();
        simpleexpression();
        neg_anlegen();

        if (symbol == EQ) {
            getSymbol();
            expression();
            eql_anlegen();
        } else if (symbol == NE) {
            getSymbol();
            expression();
            neq_anlegen();
        } else if (symbol == GT) {
            getSymbol();
            expression();
            gt_anlegen();
        } else if (symbol == LT) {
            getSymbol();
            expression();
            lt_anlegen();
        } else if (symbol == GE) {
            getSymbol();
            expression();
            ge_anlegen();
        } else if (symbol == LE) {
            getSymbol();
            expression();
            le_anlegen();
        }
    } else if (symbol == plus) {
        getSymbol();
        simpleexpression();
        if (symbol == EQ) {
            getSymbol();
            expression();
            eql_anlegen();
        } else if (symbol == NE) {
            getSymbol();
            expression();
            neq_anlegen();
        } else if (symbol == GT) {
            getSymbol();
            expression();
            gt_anlegen();
        } else if (symbol == LT) {
            getSymbol();
            expression();
            lt_anlegen();
        } else if (symbol == GE) {
            getSymbol();
            expression();
            ge_anlegen();
        } else if (symbol == LE) {
            getSymbol();
            expression();
            le_anlegen();
        }
    } else {
        simpleexpression();
        if (symbol == EQ) {
            getSymbol();
            expression();
            eql_anlegen();
        } else if (symbol == NE) {
            getSymbol();
            expression();
            neq_anlegen();
        } else if (symbol == GT) {
            getSymbol();
            expression();
            gt_anlegen();
        } else if (symbol == LT) {
            getSymbol();
            expression();
            lt_anlegen();
        } else if (symbol == GE) {
            getSymbol();
            expression();
            ge_anlegen();
        } else if (symbol == LE) {
            getSymbol();
            expression();
            le_anlegen();
        }
    }
}

void assignment() {
    expression();
}

void assignment_procedurecall() {
    if (symbol == hoch) {
        getSymbol();
        if (symbol == gleich) {
            getSymbol();
            assignment();
        } else {
            printf("%s : %d nach ident^ muss = folgen\n", quellfile, zeilenzahl);
            ende();
        }
    } else if (symbol == gleich) {
        char name[20];
        char funct[20];
        int scope;
        /* Ueberpruefung ob Variable deklariert ist */
        if (search_var_in_main(functionname, id) == 0) {
            if (search_global_const(id, 0) == 0) {
                if (search_global_var(id, 0) == 0) {
                    printf("Variable %s nicht deklariert\n", id);
                    ende();
                } else {
                    scope = 0;
                }
            } else {
                scope = 0;
            }
        } else {
            scope = 1;
        }
        strcpy(name, id);
        strcpy(funct, functionname);


        getSymbol();
        assignment();
        sto_anlegen(name, funct, scope);
    } else if (symbol == lparen) {
        strcpy(aufzu_functionname, id);
        if (search_func(id) == NULL) {
            printf("Funktion %s noch nicht bekannt\n", id);
            ende();
        }
        getSymbol();
        /*	strcpy(aufzu_functionname,id); */
        procedurecall();
    } else {
        printf("%s : %d in einem Statement muss nach dem ident ein ^ oder = kommen bzw ( bei einem proc aufruf )\n",
               quellfile, zeilenzahl);
        ende();
    }
}

void statementseq() {
    if (symbol == ident) {
        getSymbol();
        assignment_procedurecall();
        statementseq();
    } else if (symbol == IF) {
        getSymbol();
        ifstatement();

        if (symbol == END) {
            getSymbol();
        } else {
            printf("%s : %d IF muss mit END abgeschlossen werden\n", quellfile, zeilenzahl);
            ende();
        }
        statementseq();
    } else if (symbol == DO) {
        getSymbol();
        whilestatement_repstatement();
        statementseq();
    } else if (symbol == NEW) {
        getSymbol();
        newstatement();
        statementseq();
    } else if (symbol == DISPOSE) {
        getSymbol();
        dispstatement();
        statementseq();
    } else if (symbol == PUT) {
        getSymbol();
        io_statement_put();
        statementseq();
    } else if (symbol == GET) {
        getSymbol();
        io_statement_get();
        statementseq();
    }

    if (symbol == semicolon) {
        getSymbol();
        statementseq();
    }

}

void constdecl() {
    if (symbol == ident) {
        strcpy(const_name, id);
        getSymbol();
        if (symbol == gleich) {
            getSymbol();
            if (symbol == NUMBER_FLOAT || symbol == NUMBER_INT) {
                const_type = symbol;
                /* hier wird der Value eines const in ein char umgewandelt */
                /* damit man in der Tabelle keine 2 verschiedenen value typen hat */
                if (symbol == NUMBER_FLOAT) {
                    sprintf(const_value, "%f", double_zahl);
                } else if (symbol == NUMBER_INT) {
                    sprintf(const_value, "%d", fixed_zahl);
                }

                getSymbol();
                if (symbol == rparen) {
                    getSymbol();
                } else {
                    printf("%s : %d Nach CONST declaration muss ) kommen\n", quellfile, zeilenzahl);
                    ende();
                }
            } else {
                printf("%s : %d nach ident= muss eine Zahl kommen\n", quellfile, zeilenzahl);
                ende();
            }

        } else {
            printf("%s : %d nach ident muss = kommen\n", quellfile, zeilenzahl);
            ende();
        }
    } else {
        printf("%s : %d nach CONST ( muss ident kommen\n", quellfile, zeilenzahl);
        ende();
    }
}

void consts() {
    constdecl();
    if (symbol == semicolon) {
        /* suchen ob der Name schon vorhanden ist in dem scope */
        if (const_search(const_name, const_scope) == 0) {
            /* einlesen von globalen consts */
            if (global_scope == TRUE) {
                const_anlegen(const_name, functionname, 0, const_type, const_value);
                add_const(const_name, 0, const_type, const_value);
            }
        } else {
            printf("CONST Name %s ist schon vergeben\n", const_name);
            ende();
        }

        if (var_fuer_main == TRUE) {
            if (search_var_in_main(functionname, const_name) == 0) {
                const_anlegen(const_name, functionname, 1, const_type, const_value);
                add_func_rumpf(functionname, const_name, const_type, 0, 1, const_value);
            } else {
                printf("CONST Name %s ist schon vergeben\n", const_name);
                ende();
            }

        }
        if (var_fuer_main == FALSE && global_scope == FALSE) {
            if (search_var_in_main(functionname, const_name) == 0) {
                const_anlegen(const_name, functionname, 1, const_type, const_value);
                add_func_rumpf(functionname, const_name, const_type, 0, 1, const_value);
            } else {
                printf("CONST Name %s in function %s ist schon vergeben\n", const_name, functionname);
                ende();
            }
        }
        getSymbol();
        if (symbol == lparen) {
            getSymbol();
            consts();
        }

    } else {
        printf("%s : %d nach CONST () muss ; kommen\n", quellfile, zeilenzahl);
        ende();
    }


}

void variables() {
    In_variablen iter;
    In_variablen iter_old;
    varlist(io);
    if (symbol == rparen) {
        getSymbol();
        type();
        iter = invar_start;
        if (global_scope == TRUE) {
            variable_anlegen(iter, 0, temp_data_type);
        }
        if (var_fuer_main == TRUE) {
            variable_anlegen(iter, 1, temp_data_type);
        }
        if (var_fuer_main == FALSE && global_scope == FALSE && in_function == TRUE) {
            variable_anlegen(iter, 1, temp_data_type);
        }
        while (iter != NULL) {
            /* variable in der Mainliste suchen und schauen ob sie schon
				   vorhanden ist */
            if (var_search(iter->name, var_scope) == 0) {
                /* nur globale Variablen weg schreiben */
                if (global_scope == TRUE) {

                    add_var(iter->name, 0, temp_data_type);
                }
                if (var_fuer_main == TRUE) {
                    if (search_var_in_main(functionname, iter->name) == 0) {
                        add_func_rumpf(functionname, iter->name, temp_data_type, 1, 2, '\0');
                    } else {
                        printf("Variable %s schon deklariert\n", iter->name);
                        ende();
                    }
                }
                if (var_fuer_main == FALSE && global_scope == FALSE && in_function == TRUE) {
                    if (search_var_in_main(functionname, iter->name) == 0) {
                        add_func_rumpf(functionname, iter->name, temp_data_type, 1, 2, '\0');
                    } else {
                        printf("Variable %s in %s ist schon deklariert\n", iter->name, functionname);
                        ende();
                    }
                }
                iter_old = iter;
                iter = iter->next;
                iter_old->next = NULL;
                free(iter_old);
            } else {
                printf("Variable %s ist schon deklariert\n", iter->name);
                ende();
            }
        }
        iter = NULL;
        invar_start = NULL;
        if (symbol == semicolon) {
            getSymbol();
            if (symbol == lparen) {
                getSymbol();
                variables();
            }
        } else {
            printf("%s : %d nach Datentyp muss ; folgen\n", quellfile, zeilenzahl);
            ende();
        }
    } else {
        printf("%s : %d nach den Variablen muss ) folgen\n", quellfile, zeilenzahl);
        ende();
    }

}

void declarations() {
    if (symbol == CONST) {
        getSymbol();
        if (symbol == lparen) {
            getSymbol();
            consts();
        } else {
            printf("%s : %d nach CONST muss ( folgen\n", quellfile, zeilenzahl);
            ende();
        }
        if (symbol == DECLARE) {
            getSymbol();
            if (symbol == lparen) {
                getSymbol();
                variables();
            } else {
                printf("%s : %d nach DECLARE muss ( folgen\n", quellfile, zeilenzahl);
                ende();
            }

        }
    } else if (symbol == DECLARE) {
        getSymbol();
        if (symbol == lparen) {
            getSymbol();
            variables();
        } else {
            printf("%s : %d nach DECLARE muss ( folgen\n", quellfile, zeilenzahl);
            ende();
        }
    }

}

void procedure() {
    local_stackptr_auf_null_setzen();

    if (symbol == colon) {
        getSymbol();
        if (symbol == PROC) {
            getSymbol();
            if (symbol == lparen) {
                getSymbol();
                if (symbol == MAIN) {
                    /* abpruefen ob main schon vorhanden */
                    pruefe_main();
                    boolmain = TRUE;
                    var_fuer_main = TRUE;
                    strcpy(functionname, "MAIN");
                    /* muss noch ausgewertet werden */
                    rueckgabe = add_func(functionname);
                    procedure_anlegen(functionname);
                    if (rueckgabe != 0) {
                        if (rueckgabe == 1) {
                            printf("Funktion mit dem Namen %s ist schon vorhanden\n", functionname);
                            ende();
                        } else if (rueckgabe == 2) {
                            printf("Funktionskopf mit dem Namen %s ist schon vorhanden\n", functionname);
                            ende();
                        } else if (rueckgabe == 3) {
                            printf("Funktionsrumpf mit dem Namen %s ist schon vorhanden\n", functionname);
                            ende();
                        } else if (rueckgabe == 4) {
                            printf("Funktionsprototyp mit dem Namen %s ist schon vorhanden\n", functionname);
                            ende();
                        }
                    }
                    getSymbol();
                    if (symbol == rparen) {
                        getSymbol();
                        if (symbol == semicolon) {
                            getSymbol();
                            /* hier die Deklarationen */
                            declarations();
                            if (symbol == DO) {
                                getSymbol();
                                statementseq();
                            }

                            if (symbol == END) {
                                ret_anlegen();
                                var_fuer_main = FALSE;
                                getSymbol();
                                if (symbol == ident) {
                                    getSymbol();
                                    if (symbol == semicolon) {
                                        getSymbol();
                                        if (symbol == ident) {
                                            getSymbol();
                                            strcpy(functionname, id);
                                            procedure();
                                        } else if (symbol != ident && symbol != zeile) {
                                            printf("%s : %d nach einer Procedure duerfen nur weitere Procedure kommen\n",
                                                   quellfile, zeilenzahl);
                                            ende();
                                        }
                                    } else {
                                        printf("%s : %d nach END ident muss ; kommen\n", quellfile, zeilenzahl);
                                        ende();
                                    }
                                } else {
                                    printf("%s : %d nach END muss ident kommen\n", quellfile, zeilenzahl);
                                    ende();
                                }

                            } else {
                                printf("%s : %d Programm muss mit END ident; abschliessen\n", quellfile, zeilenzahl);
                                ende();
                            }
                        } else {
                            printf("%s : %d Nach Funktionsende muss ; kommen\n", quellfile, zeilenzahl);
                            ende();
                        }

                    } else {
                        printf("%s : %d nach MAIN muss ) kommen\n", quellfile, zeilenzahl);
                        ende();
                    }
                } else if (symbol == IN || symbol == semicolon) {
                    int anzahl_der_out_variablen;
                    parameterlist();


                    if (symbol == rparen) {
                        procedure_anlegen(functionname);
                        rueckgabe = add_func(functionname);
                        if (rueckgabe == 0) {
                            /* Liste muss gespeichert werden da noch kein Proto da ist */
                            In_variablen iter_out;
                            iter_out = invar_start;
                            while (iter_out != NULL) {
                                /* out Variablen hinzufuegen und temp liste loeschen */
                                if (iter_out->in_out_f == out) {
                                    In_variablen out_old;
                                    add_func_kopf(iter_out->function_name, iter_out->name, iter_out->data_type, 2, 1);
                                    out_old = iter_out;
                                    iter_out = iter_out->next;
                                    out_old->next = NULL;
                                    free(out_old);

                                }
                                    /* hier wird die Elemente in der Templiste mit flag in geloescht */
                                else if (iter_out->in_out_f == in) {
                                    In_variablen in_old;
                                    add_func_kopf(iter_out->function_name, iter_out->name, iter_out->data_type, 1, 1);
                                    in_old = iter_out;
                                    iter_out = iter_out->next;
                                    in_old->next = NULL;
                                    free(in_old);
                                }
                            }
                            if (invar_start != NULL) {
                                invar_start = NULL;
                            }
                        } else if (rueckgabe == 1) {
                            printf("Funktion %s gibt es schon\n", functionname);
                            ende();
                        } else if (rueckgabe == 2) {
                            In_variablen iter;
                            iter = invar_start;
                            printf("%d\n", temp_data_type);
                            in_variablen_anlegen(iter);
                            anzahl_der_out_variablen = out_variablen_anlegen(iter, temp_data_type);
                            /* kopf schon da liste muss geprueft werden */
                            vergleichen_der_parameter(functionname);
                            while (iter != NULL) {
                                In_variablen in_old;
                                in_old = iter;
                                iter = iter->next;
                                free(in_old);
                            }
                            if (invar_start != NULL) {
                                invar_start = NULL;
                            }

                        } else if (rueckgabe == 3) {
                            printf("Funktion %s ist schon implementiert\n", functionname);
                            ende();
                        } else if (rueckgabe == 4) {
                            vergleichen_der_parameter(functionname);
                        }
                        getSymbol();
                        if (symbol == semicolon) {
                            getSymbol();
                            /* hier Deklaration von CONST UND DECLARE */
                            in_function = TRUE;
                            declarations();
                            in_function = FALSE;
                            if (symbol == DO) {
                                getSymbol();
                                statementseq();
                            }

                            if (symbol == END) {
                                /* test */
                                Kopf temp;
                                temp = search_func(functionname)->f->k;
                                while (temp != NULL) {
                                    if (temp->flag == out) {
                                        lod_anlegen(temp->var->name, functionname, 1);

                                    }
                                    temp = temp->next;
                                }
                                ret_anlegen2(anzahl_der_out_variablen);
                                getSymbol();
                                if (symbol == ident) {
                                    getSymbol();
                                    if (symbol == semicolon) {
                                        getSymbol();
                                        if (symbol == ident) {
                                            getSymbol();
                                            strcpy(functionname, id);
                                            procedure();
                                        } else if (symbol != ident && symbol != zeile) {
                                            printf("%s : %d nach einer Procedure duerfen nur weitere Procedure kommen\n",
                                                   quellfile, zeilenzahl);
                                            ende();
                                        }
                                    } else {
                                        printf("%s : %d nach END ident muss ; kommen\n", quellfile, zeilenzahl);
                                        ende();
                                    }
                                } else {
                                    printf("%s : %d nach END muss ident kommen\n", quellfile, zeilenzahl);
                                    ende();
                                }

                            } else {
                                printf("%s : %d Programm muss mit END ident; abschliessen\n", quellfile, zeilenzahl);
                                ende();
                            }
                        } else {
                            printf("%s : %d Funktionskopf muss mit ; abgeschlossen werden\n", quellfile, zeilenzahl);
                            ende();
                        }
                    } else {
                        printf("%s : %d ) fehlt nach Funktinskopfende\n", quellfile, zeilenzahl);
                        ende();
                    }
                } else {
                    printf("%s : %d Nach PROC ( muss Parameterlist oder MAIN kommen\n", quellfile, zeilenzahl);
                    ende();
                }
            } else {
                printf("%s : %d Nach PROC muss ( kommen\n", quellfile, zeilenzahl);
                ende();
            }
        } else {
            printf("%s : %d nach ident : muss PROC kommen\n", quellfile, zeilenzahl);
            ende();
        }

    } else {
        printf("%s : %d nach ident von Funktion muss : kommen\n", quellfile, zeilenzahl);
        ende();
    }

}

void type() {
    if (symbol == POINTER) {
        getSymbol();
        if (symbol == TO) {
            getSymbol();
            elementarytype();
        } else {
            printf("%s : %d nach POINTER fehlt TO\n", quellfile, zeilenzahl);
            ende();
        }

    } else {

        elementarytype();
    }


}

void outsection() {
    if (symbol == lparen) {
        getSymbol();
        /* 2 hier fuer in out flag in liste */
        varlist(2);
        if (symbol == rparen) {
            In_variablen iter;
            getSymbol();
            type();
            iter = invar_start;
            /* alle Variablen aus der temporaeren liste mit in flag wegschreiben in Mainliste */
            while (iter != NULL && iter->in_out_f == out) {
                iter->data_type = temp_data_type;
                iter = iter->next;

            }
        } else {
            printf("%s : %d ) erwartet nach OUT deklaration\n", quellfile, zeilenzahl);
            ende();
        }
    } else {
        printf("%s : %d ( erwartet nach OUT\n", quellfile, zeilenzahl);
        ende();
    }
}

void insection() {
    if (symbol == lparen) {
        getSymbol();
        /* 1 hier fuer in out flag in liste */
        varlist(1);
        if (symbol == rparen) {
            getSymbol();
            type();
        } else {
            printf("%s : %d ) erwartet\n", quellfile, zeilenzahl);
            ende();
        }
    } else {
        printf("%s : %d ( erwartet\n", quellfile, zeilenzahl);
        ende();
    }
}

void parameterlist() {
    if (symbol == IN) {
        getSymbol();
        insection();
        if (symbol == semicolon) {
            In_variablen iter;
            iter = invar_start;
            /* alle Variablen aus der temporaeren liste mit in flag wegschreiben in Mainliste */
            while (iter != NULL && iter->in_out_f == in) {
                iter->data_type = temp_data_type;
                iter = iter->next;

            }
            getSymbol();
            if (symbol == OUT) {
                getSymbol();
                outsection();
            }
        } else {
            printf("%s : %d ;  erwartet nach IN\n", quellfile, zeilenzahl);
            ende();
        }
    } else if (symbol == semicolon) {
        getSymbol();
        if (symbol == OUT) {
            getSymbol();
            outsection();
        }

    } else {
        printf("%s : %d in der Parameterliste muss IN kommen bzw wenn kein IN kommt muss ; kommen", quellfile,
               zeilenzahl);
        ende();
    }


}

void elementarytype() {
    if (symbol == FIXED || symbol == FLOAT) {
        temp_data_type = symbol;
        getSymbol();
    } else {
        printf("%s : %d Datentyp fehlt\n", quellfile, zeilenzahl);
        ende();
    }
}

void varlist(int t) {
    io = t;
    if (symbol == ident) {
        if (search_var(id) == 0) {
            einlesen(t);
        } else {
            printf("Parameter %s ist schon deklariert\n", id);
            ende();
        }
        getSymbol();
        if (symbol == comma) {
            getSymbol();
            varlist(t);
        }
    } else {
        printf("%s : %d fehlender ident\n", quellfile, zeilenzahl);
        ende();
    }
}

void common() {
    declarations();
    if (symbol == END) {
        var_scope = 1;
        const_scope = 1;
        global_scope = FALSE;
        getSymbol();
    } else {
        printf("%s : %d nach COMMON und der Declaration muss END kommen\n", quellfile, zeilenzahl);
        ende();
    }

}

void prototype() {
    if (search_func(functionname) != NULL) {
        printf("Funktion %s ist schon deklariert\n", functionname);
        ende();
    } else {
        rueckgabe = add_func(functionname);
        if (symbol == colon) {
            getSymbol();
            if (symbol == PROC) {
                getSymbol();
                if (symbol == lparen) {
                    getSymbol();
                    parameterlist();
                    if (symbol == rparen) {
                        /* Funktionskopf hinzufuegen */
                        In_variablen iter_out;
                        iter_out = invar_start;
                        while (iter_out != NULL) {
                            /* out Variablen hinzufuegen und temp liste loeschen */
                            if (iter_out->in_out_f == out) {
                                In_variablen out_old;
                                add_func_kopf(iter_out->function_name, iter_out->name, iter_out->data_type, 2, 1);
                                out_old = iter_out;
                                iter_out = iter_out->next;
                                out_old->next = NULL;
                                free(out_old);

                            }
                                /* hier wird die Elemente in der Templiste mit flag in geloescht */
                            else if (iter_out->in_out_f == in) {
                                In_variablen in_old;
                                add_func_kopf(iter_out->function_name, iter_out->name, iter_out->data_type, 1, 1);
                                in_old = iter_out;
                                iter_out = iter_out->next;
                                in_old->next = NULL;
                                free(in_old);
                            }
                        }
                        if (invar_start != NULL) {
                            invar_start = NULL;
                        }
                        getSymbol();
                        if (symbol == semicolon) {
                            getSymbol();
                            if (symbol == ident) {
                                getSymbol();
                                strcpy(functionname, id);
                                prototype();
                            }
                        } else {
                            printf("%s : %d  ; fehlt \n", quellfile, zeilenzahl);
                            ende();
                        }
                    } else {
                        printf("%s : %d ) fehlt \n", quellfile, zeilenzahl);
                        ende();
                    }
                } else {
                    printf("%s : %d ( erwartet\n", quellfile, zeilenzahl);
                    ende();
                }

            } else {
                printf("%s : %d PROC erwartet\n", quellfile, zeilenzahl);
                ende();
            }
        } else {
            printf("%s : %d nach %s  : erwartet\n", quellfile, zeilenzahl, id);
            ende();
        }
    }
}

void protos() {
    if (symbol == ident) {
        getSymbol();
        strcpy(functionname, id);
        prototype();
    } else {
        printf("%s: %d Error ident fehlt nach PROTO\n", quellfile, zeilenzahl);
        ende();
    }
}

void program() {
    if (symbol == PROTO) {
        getSymbol();
        protos();
        if (symbol == END) {
            getSymbol();
            if (symbol == COMMON) {
                io = 3;
                global_scope = TRUE;
                const_scope = 0;
                var_scope = 0;
                functionname[0] = '\0';
                getSymbol();
                common();
                if (symbol == ident) {
                    getSymbol();
                    strcpy(functionname, id);
                    printf("%s\n", functionname);
                    procedure();
                }
            } else if (symbol == ident) {
                getSymbol();
                strcpy(functionname, id);
                procedure();
            } else {
                printf("%s : %d error nach END von PROTO \n", quellfile, zeilenzahl);
                ende();
            }
        } else {
            printf("%s : %d END fehlt nach PROTO \n", quellfile, zeilenzahl);
            ende();
        }
    } else if (symbol == COMMON) {
        io = 3;
        global_scope = TRUE;
        const_scope = 0;
        var_scope = 0;
        functionname[0] = '\0';
        getSymbol();
        common();

        if (symbol == ident) {
            io = 3;
            getSymbol();
            strcpy(functionname, id);
            procedure();
        } else if (symbol != ident && symbol != zeile) {
            printf("%s : %d Nach COMMON duerfen nur noch Funktionsimplementationen kommen keine PROTO mehr\n",
                   quellfile, zeilenzahl);
            ende();
        }
    } else if (symbol == ident) {
        io = 3;
        getSymbol();
        strcpy(functionname, id);
        procedure();
    } else {
        printf("%s : %d Programmkopf fehlerhaft\n", quellfile, zeilenzahl);
        ende();
    }


}

void getSymbol() {

    i = fscanf(token, "%d", &symbol);

    if (i == 0) {
        return;
    } else if (i < 0) {
        return;
    }

    if (symbol == ident) {
        fscanf(token, "%s", id);
    } else if (symbol == NUMBER_FLOAT) {
        fscanf(token, "%f", &double_zahl);
    } else if (symbol == NUMBER_INT) {
        fscanf(token, "%d", &fixed_zahl);
    } else if (symbol == zeile) {
        fscanf(token, "%d", &zeilenzahl);
        getSymbol();
    } else if (symbol == text) {
        fscanf(token, "%s", id);
    }


}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("parser <file>\n");
        ende(1);
    }

    strcpy(quellfile, argv[1]);
    strcat(quellfile, ".plz");
    strcpy(tokenfile, argv[1]);
    strcat(tokenfile, ".tok");
    strcpy(codefile, argv[1]);
    strcat(codefile, ".cod");

    if ((token = fopen(tokenfile, "r")) == NULL) {
        printf("Fehler beim oeffnen der Tokendatei! \n");
        ende(1);
    }

    getSymbol();
    call_anlegen("MAIN");
    program();

    endeende();
    liste_verschmelzen();
    if ((codef = fopen(codefile, "w")) == NULL) {
        printf("Fehler beim oeffnen der Codedatei! \n");
        ende(1);
    }
    write_code(codef);
    fclose(codef);

    printf("%d Zeilen verarbeitet\n", zeilenzahl);
    printf("Programm korrekt\n");
    return 0;


}
