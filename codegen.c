#include"header.h"
#include"code.h"
#include "cmdcode.h"

#define FLOAT 10
#define FIXED 11
#define NUMBER_INT 55
#define NUMBER_FLOAT 56

Code codelist = NULL;
Faddress variablen_halter = NULL;
Fneeded addressen_finden = NULL;

int global_stackptr = 2;
int local_stackptr = 2;

/*
char *codebefehle[] = {
	
"NUL",
"ADD",   
"SUB",
"MPY",
"DVD",
"NEG",
"ODD",
"EQL",
"NEQ",
"LSS",
"GTR",
"LEQ",
"GEQ",
"ILIT",
"RET",
"CALL",
"LOD",
"STO",
"IINT",
"JPU",
"JPC",
"INP",
"PRN",
"ASC",
"PRS",
"PRL",
"FINT",
"FLIT",
"FSTO"
};
    */

Faddress hole_faddresse() {
    return variablen_halter;
}

void while_clause_anlegen(char *if_name) {
    int address;
    address = zeilennr_ermitteln();

    while_anfangsaddresse_speichern(if_name, address);
}

void while_anfangsaddresse_speichern(char *ifname, int adresse) {
    Faddress fl;
    fl = (Faddress) malloc(sizeof(struct function_address));
    strcpy(fl->name, ifname);
    fl->type = IFCL;
    fl->addr = adresse;
    fl->next = NULL;
    faddress_hinzufuegen(fl);
}

void if_clause_anlegen(char *if_name) {
    int address;
    address = zeilennr_ermitteln();

    if_statement_anfangsaddresse_speichern(if_name, address);
}

void if_clause_anlegen2(char *if_name) {
    int address;
    address = zeilennr_ermitteln();
    address++;

    if_statement_anfangsaddresse_speichern(if_name, address);
}

void procedure_anlegen(char *functionname) {
    int address;
    address = zeilennr_ermitteln();

    funktionsanfangsadresse_speichern(functionname, address);
}

void local_stackptr_auf_null_setzen() {
    local_stackptr = 2;
}

int local_reservieren_am_stack() {
    return local_stackptr++;
}

int reservieren_am_stack() {
    return global_stackptr++;
}

int postfixcode_hinzufuegen(Code codi) {
    int zaehler = 0;
    if (codelist == NULL) {
        codelist = codi;
    } else {
        Code temp;
        temp = codelist;
        while (temp->next != NULL) {
            zaehler++;
            temp = temp->next;
        }
        temp->next = codi;
        zaehler++;
    }
    return zaehler;
}

void faddress_hinzufuegen(Faddress fl) {
    if (variablen_halter == NULL) {
        variablen_halter = fl;
    } else {
        Faddress temp;
        temp = variablen_halter;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = fl;
    }
}

void funktionsanfangsadresse_speichern(char *functionname, int adresse) {
    Faddress fl;
    fl = (Faddress) malloc(sizeof(struct function_address));
    strcpy(fl->name, functionname);
    fl->type = FUNC;
    fl->addr = adresse;
    fl->next = NULL;
    faddress_hinzufuegen(fl);
}

void if_statement_anfangsaddresse_speichern(char *ifname, int adresse) {
    Faddress fl;
    fl = (Faddress) malloc(sizeof(struct function_address));
    strcpy(fl->name, ifname);
    fl->type = IFCL;
    fl->addr = adresse;
    fl->next = NULL;
    faddress_hinzufuegen(fl);
}

void variable_adresse_speichern(char *name, char *functionname, int adresse) {
    Faddress fl;
    fl = (Faddress) malloc(sizeof(struct function_address));
    strcpy(fl->name, name);
    strcpy(fl->functionname, functionname);
    fl->type = VARI;
    fl->addr = adresse;
    fl->next = NULL;
    faddress_hinzufuegen(fl);
}

void constante_adresse_speichern(char *name, char *functionname, int adresse) {
    Faddress fl;
    fl = (Faddress) malloc(sizeof(struct function_address));
    strcpy(fl->name, name);
    strcpy(fl->functionname, functionname);
/*	fl->type=CONS; */
    fl->type = VARI;
    fl->addr = adresse;
    fl->next = NULL;
    faddress_hinzufuegen(fl);

}

int postfixcode_erzeugen(int befehl, int scope, char *anzahl_parameter) {
    Code cl;
    cl = (Code) malloc(sizeof(struct codeliste));
    cl->code = befehl;
    cl->niveau = scope;
    strcpy(cl->address, anzahl_parameter);
    cl->next = NULL;
    return postfixcode_hinzufuegen(cl);

}

void variable_anlegen(In_variablen iv, int scope, int type) {
    int result;
    int parama_zaehler = 0;
    In_variablen temp_iv;
    temp_iv = iv;
    while (temp_iv != NULL) {
        parama_zaehler++;
        temp_iv = temp_iv->next;
    }
    temp_iv = iv;
    if (type == FIXED) {
        char zahl[30];
        sprintf(zahl, "%d", parama_zaehler);
        postfixcode_erzeugen(IINT, scope, zahl);
    } else if (type == FLOAT) {
        char zahl[30];
        sprintf(zahl, "%d", parama_zaehler);
        postfixcode_erzeugen(FINT, scope, zahl);
    }

        /* nur falls keine der beiden Typen kommt was aber nicht passieren kann*/
    else
        return;
    while (temp_iv != NULL) {
        if (scope == 0) {
            result = reservieren_am_stack();
        } else if (scope == 1) {
            result = local_reservieren_am_stack();
        }
        variable_adresse_speichern(temp_iv->name, temp_iv->function_name, result);
        temp_iv = temp_iv->next;
    }
}

void const_anlegen(char *const_name, char *functionname, int scope, int type, char *value) {
    int result;
    char res[20];
    if (scope == 0) {
        result = reservieren_am_stack();
    } else {
        result = local_reservieren_am_stack();
    }
    sprintf(res, "%d", result);
    if (type == NUMBER_INT) {
        /* 1 weil nur eine Variable angelegt wird */
        postfixcode_erzeugen(IINT, scope, "1");
        postfixcode_erzeugen(ILIT, 0, value);
        postfixcode_erzeugen(STO, scope, res);
    } else if (type == NUMBER_FLOAT) {
        postfixcode_erzeugen(FINT, scope, "1");
        postfixcode_erzeugen(FLIT, 0, value);
        postfixcode_erzeugen(STO, scope, res);
    }
        /* nur falls keine der beiden Typen kommt was aber nicht passieren kann*/
    else {
        /*	stackptr--;*/
        return;
    }
    constante_adresse_speichern(const_name, functionname, result);
}

void fneeded_hinzufuegen(Fneeded fn) {
    if (addressen_finden == NULL) {
        addressen_finden = fn;
    } else {
        Fneeded temp;
        temp = addressen_finden;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = fn;
    }
}

void zu_fneeded_liste_hinzufuegen(char *name, char *functionname, int listennr, int flag) {
    Fneeded fn;
    fn = (Faddress) malloc(sizeof(struct function_address));
    strcpy(fn->name, name);
    strcpy(fn->functionname, functionname);
    fn->type = flag;
    fn->addr = listennr;
    fn->next = NULL;
    fneeded_hinzufuegen(fn);
}

void call_anlegen2(char *functionname, int anzahl) {
    int zaehler;
    zaehler = postfixcode_erzeugen(CALL, anzahl, "\0");
    /* in dieser Liste steht der Name und die Stelle in der Hauptliste wo addresse noch hin muss */
    zu_fneeded_liste_hinzufuegen(functionname, "\0", zaehler, FUNC);
}

void call_anlegen(char *functionname) {
    int zaehler;
    zaehler = postfixcode_erzeugen(CALL, 0, "\0");
    /* in dieser Liste steht der Name und die Stelle in der Hauptliste wo addresse noch hin muss */
    zu_fneeded_liste_hinzufuegen(functionname, "\0", zaehler, FUNC);
}

void jpc_anlegen(char *name) {
    int zaehler;
    zaehler = postfixcode_erzeugen(JPC, 0, "\0");
    zu_fneeded_liste_hinzufuegen(name, "\0", zaehler, IFCL);
}

void jpu_anlegen(char *name) {
    int zaehler;
    zaehler = postfixcode_erzeugen(JPU, 0, "\0");
    zu_fneeded_liste_hinzufuegen(name, "\0", zaehler, IFCL);
}

void ret_anlegen() {
    postfixcode_erzeugen(RET, 0, "0");
}

void ret_anlegen2(int anzahl) {
    postfixcode_erzeugen(RET, anzahl, "0");
}

void add_anlegen() {
    postfixcode_erzeugen(ADD, 0, "0");
}

void sub_anlegen() {
    postfixcode_erzeugen(SUB, 0, "0");
}

void mpy_anlegen() {
    postfixcode_erzeugen(MPY, 0, "0");
}

void div_anlegen() {
    postfixcode_erzeugen(DVD, 0, "0");
}

void neg_anlegen() {
    postfixcode_erzeugen(NEG, 0, "0");
}

void flit_anlegen(double zahl) {
    char temp[30];
    sprintf(temp, "%f", zahl);
    postfixcode_erzeugen(FLIT, 0, temp);
}

void ilit_anlegen(int zahl) {
    char temp[30];
    sprintf(temp, "%d", zahl);
    postfixcode_erzeugen(ILIT, 0, temp);
}

void sto_anlegen(char *var_name, char *functionname, int scope) {
    int zaehler;
    zaehler = postfixcode_erzeugen(STO, scope, "\0");
    zu_fneeded_liste_hinzufuegen(var_name, functionname, zaehler, VARI);
}

void lod_anlegen(char *var_name, char *functionname, int scope) {
    int zaehler;
    zaehler = postfixcode_erzeugen(LOD, scope, "\0");
    zu_fneeded_liste_hinzufuegen(var_name, functionname, zaehler, VARI);
}

void prl_anlegen() {
    postfixcode_erzeugen(PRL, 0, "0");
}

void prn_anlegen() {
    postfixcode_erzeugen(PRN, 0, "0");
}

void eql_anlegen() {
    postfixcode_erzeugen(EQL, 0, "0");
}

void neq_anlegen() {
    postfixcode_erzeugen(NEQ, 0, "0");
}

void gt_anlegen() {
    postfixcode_erzeugen(GTR, 0, "0");
}

void lt_anlegen() {
    postfixcode_erzeugen(LSS, 0, "0");
}

void ge_anlegen() {
    postfixcode_erzeugen(GEQ, 0, "0");
}

void le_anlegen() {
    postfixcode_erzeugen(LEQ, 0, "0");
}

void in_variablen_anlegen(In_variablen iv) {
    In_variablen temp_iv;
    int result;
    temp_iv = iv;
    while (temp_iv != NULL) {
        if (temp_iv->data_type == FIXED && temp_iv->in_out_f == 1) {
            /*postfixcode_erzeugen(IINT,1,"1");*/
            result = local_reservieren_am_stack();
            variable_adresse_speichern(temp_iv->name, temp_iv->function_name, result);
        } else if (temp_iv->data_type == FLOAT && temp_iv->in_out_f == 1) {
            /*postfixcode_erzeugen(FINT,1,"1");*/
            result = local_reservieren_am_stack();
            variable_adresse_speichern(temp_iv->name, temp_iv->function_name, result);
        }

        temp_iv = temp_iv->next;
    }
}

int out_variablen_anlegen(In_variablen iv, int type) {
    In_variablen temp_iv;
    int result;
    int ret = 0;
    temp_iv = iv;
    while (temp_iv != NULL) {
        if (type == FIXED && temp_iv->in_out_f == 2) {
            /*	postfixcode_erzeugen(IINT,1,"1"); */
            result = local_reservieren_am_stack();
            variable_adresse_speichern(temp_iv->name, temp_iv->function_name, result);
            ret++;
        } else if (type == FLOAT && temp_iv->in_out_f == 2) {
            /*	postfixcode_erzeugen(FINT,1,"1");  */
            result = local_reservieren_am_stack();
            variable_adresse_speichern(temp_iv->name, temp_iv->function_name, result);
            ret++;
        }

        temp_iv = temp_iv->next;
    }
    return ret;
}

int zeilennr_ermitteln() {
    Code cl = codelist;
    int line = 0;
    while (cl != NULL) {
        line++;
        cl = cl->next;
    }
    return line;
}

int in_variablen_halter_suchen(char *name, int type, char *functionname) {
    int addresse = -1;
    Faddress fa_temp;
    fa_temp = variablen_halter;
    while (fa_temp != NULL) {
        if ((fa_temp->type == type) && (strcmp(fa_temp->name, name) == 0) &&
            (strcmp(fa_temp->functionname, functionname) == 0)) {
            addresse = fa_temp->addr;
            break;
        }
        fa_temp = fa_temp->next;
    }
    return addresse;
}


void in_codeliste_eintragen(int zaehler, int addresse) {
    Code cl_temp;
    int i;
    cl_temp = codelist;
    for (i = 0; i < zaehler; i++) {
        cl_temp = cl_temp->next;
    }
    sprintf(cl_temp->address, "%d", addresse);
}

void liste_verschmelzen() {

    Fneeded fn_temp;
    Code cl_temp;
    int result;
    fn_temp = addressen_finden;
    cl_temp = codelist;


    while (fn_temp != NULL) {
        result = in_variablen_halter_suchen(fn_temp->name, fn_temp->type, fn_temp->functionname);
        if (result == -1) {
            result = in_variablen_halter_suchen(fn_temp->name, fn_temp->type, "\0");
        }
        in_codeliste_eintragen(fn_temp->addr, result);
        fn_temp = fn_temp->next;
    }


}

void write_code(FILE *codefile) {
    Code cl = codelist;
    int line = 0;
    while (cl != NULL) {
        fprintf(codefile, "%03d %s  %2d  %s\n", line++, codebefehle[cl->code], cl->niveau, cl->address);
        cl = cl->next;
    }

}


void destroy_codelist() {
    Code temp;
    while (codelist != NULL) {
        temp = codelist;
        codelist = codelist->next;
        free(temp);
    }
}

void destroy_faddress() {
    Faddress temp;
    while (variablen_halter != NULL) {
        temp = variablen_halter;
        variablen_halter = variablen_halter->next;
        free(temp);
    }
}

void destroy_fneeded() {
    Fneeded temp;
    while (addressen_finden != NULL) {
        temp = addressen_finden;
        addressen_finden = addressen_finden->next;
        free(temp);
    }
}

void destroy_code() {
    destroy_codelist();
    destroy_faddress();
    destroy_fneeded();
}
