#define TRUE 1
#define FALSE 0

/* temp liste */
typedef struct in_variablen *In_variablen;

struct in_variablen {
    char function_name[20];
    char name[20];
    int in_out_f;
    int data_type;
    In_variablen next;
};


typedef struct constante *Constante;

struct constante {
    char name[20];
    int scope;
    int type;
    char value[30];
};

typedef struct variable *Variable;

struct variable {
    char name[20];
    int scope;
    int type;
    char value[30];
};

typedef struct kopf *Kopf;

struct kopf {
    Variable var;
    int flag;    /* in oder out flag */
    Kopf next;
};

typedef struct rumpf *Rumpf;

struct rumpf {
    int flag;    /* fuer const oder var */
    Variable var;
    Constante con;
    Rumpf next;
};

typedef struct function *Function;

struct function {
    char name[20];
    Kopf k;
    Rumpf r;
};


typedef struct mainlist *Mainlist;

struct mainlist {

    int flag;    /* 0 1 oder 2 * fuer const var oder func */
    Variable v;
    Constante c;
    Function f;
    Mainlist next;
};

Mainlist search_func(char *);

void variable_anlegen(In_variablen, int, int);

void const_anlegen(char *, char *, int, int, char *);
