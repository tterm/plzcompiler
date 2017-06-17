#ifndef code_h
#define code_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
#define VARI 1
#define CONS 2
#define FUNC 3
#define IFCL 4


#define  NUL    0
#define  ADD    1
#define  SUB    2
#define  MPY    3
#define  DVD    4
#define  NEG    5
#define  ODD    6
#define  EQL    7
#define  NEQ    8
#define  LSS    9
#define  GTR   10
#define  LEQ   11
#define  GEQ   12
#define  ILIT   13
#define  RET   14
#define  CALL   15
#define  LOD   16
#define  STO   17
#define  IINT   18
#define  JPU   19
#define  JPC   20
#define  INP   21
#define  PRN   22
#define  ASC   23
#define  PRS   24
#define  PRL   25
#define  FINT  26
#define  FLIT  27
#define  FSTO  28
*/

typedef struct codeliste *Code;

struct codeliste 
{
	int code;
	int niveau;
	char address[30];
	Code next;
};

typedef struct function_address *Faddress;
typedef struct function_address *Fneeded;

struct function_address
{
	char name[30];
	char functionname[30];
	int addr;
	int type;
	Faddress next;
};

Faddress hole_faddresse();


int zeilennr_ermitteln();
void funktionsanfangsadresse_speichern(char*,int);
void if_statement_anfangsaddresse_speichern(char *,int);
void faddress_hinzufuegen(Faddress);
void while_anfangsaddresse_speichern(char *,int );
void if_clause_anlegen(char *);
void jpu_anlegen(char *);
void jpc_anlegen(char *);
void while_clause_anlegen(char *);
void prl_anlegen();
void add_anlegen();
void sub_anlegen();
void mpy_anlegen();
void div_anlegen();
void neg_anlegen();
void flit_anlegen(double );
void ilit_anlegen(int );
void sto_anlegen(char *,char *,int);
void lod_anlegen(char *,char *,int);
void prn_anlegen();
void eql_anlegen();
void neq_anlegen();
void gt_anlegen();
void lt_anlegen();
void ge_anlegen();
void le_anlegen();
int out_variablen_anlegen(In_variablen,int);
void in_variablen_anlegen(In_variablen);
void ret_anlegen();
void ret_anlegen2(int);
void call_anlegen(char *);
void call_anlegen2(char *,int);
void procedure_anlegen(char *);
void local_stackptr_auf_null_setzen();
void write_code(FILE *);
void liste_verschmelzen();
void destroy_code();

#endif
