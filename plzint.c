#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/* #include "code.h" */
#include "cmdcode.h"

#define maxcode 1000
#define maxstack 10000

#define charItemLength 20
#define maxattrlng 30
#define maxparam 20

int debug = 1;

int pc;                    /* program counter */
int base;                  /* base pointer */
int sptr;                  /* stack pointer */
int jumpswitch;            /* indicates jumps */

struct stackItem {
    int h;
    int l;
};

struct stackItem stack[maxstack];       /* runtime stack */

struct stackItem parameter[maxparam];

char tstring[maxattrlng];

/* --- */
/*char *codebefehle[] =
{
"NUL",	* 00
"ADD",	* 01
"SUB",	* 02
"MPY",	* 03
"DVD",	* 04
"NEG",	* 05
"ODD",	* 06
"EQL",	* 07
"NEQ",	* 08
"LSS",	* 09
"GTR",	* 10
"LEQ",* 11
"GEQ",	* 12
"ILIT",	* 13
"RET",	* 14
"CALL",	* 15
"ILOD",	* 16
"STO",	* 17
"IINT",	* 18
"JPU",	* 19
"JPC",	* 20
"INP",	* 21
"PRN",	* 22
"ASC",	* 23
"PRS",	* 24
"PRL",	* 25
"FINT",	* 26
"FLIT",	* 27
"ISTO",	* 28
"LOD",	* 29
"IPRN",	* 30
"FPRN",	* 31
"SOUT",	* 32
"SPC",	* 33
"PDBG",	* 34
"STDB"	* 35
};*/
/* --- */

FILE *intcode;
struct instr {
    int o;         /* opcode */
    int l;         /* scope flag */
    char a[maxattrlng]; /* opattr */
};

struct instr code[maxcode];

/* Math und Stack routinen */

void pushFloat(float dat);

void pushInt(int dat);

int popInt();

float popFloat();

int truncToInt(float val) {
    return val;
}

char *int2char(int dat) {
    sprintf(tstring, "%d", dat);
    return tstring;
}

char *float2char(float dat) {
    sprintf(tstring, "%f", dat);
    return tstring;
}

int char2int(char *dat) {
    int ret;
    ret = atoi(dat);
    return ret;
}

float char2float(char *dat) {
    float ret;
    ret = atof(dat);
    /*ret=truncToInt(ret*1000)/1000;*/
    return ret;
}

int previewInt() {
    int ret = popInt();
    pushInt(ret);
    return ret;
}

float previewFloat() {
    float ret = popFloat();
    pushFloat(ret);
    return ret;
}

int popInt() {
    sptr--;
    return stack[sptr].h;
}

float popFloat() {
    float ret;
    float tmp;
    sptr--;
    ret = stack[sptr].h;
    tmp = stack[sptr].l;
    if (tmp < 0)tmp = 0;
    if (tmp != 0) {
        while (truncToInt(tmp) != 0) {
            tmp = tmp / 10;
        }
    }
    if (ret < 0)
        ret = ret - tmp;
    else
        ret = ret + tmp;
    return ret;
}


void pushInt(int dat) {
    stack[sptr].h = dat;
    stack[sptr].l = -1;
    sptr++;
}

void pushIntF(float dat) {
    pushInt(truncToInt(dat));
}

void pushFloat(float dat) {
    float tmp;
    /*fprintf(stdout, "dat %3f", dat);*/
    stack[sptr].h = truncToInt(dat);
    tmp = dat - truncToInt(dat);
    /*fprintf(stdout, "dat %3f, tmp %3f", dat, tmp);*/
    if (tmp != 0) {
        while (tmp != truncToInt(tmp)) {
            tmp = tmp * 10;
            /*fprintf(stdout, "-tmp %3f", tmp);*/
        }
    }
    if (tmp < 0)tmp = -tmp;
    stack[sptr].l = tmp;
    sptr++;
}

void push(struct stackItem *dat) {
    stack[sptr].h = dat->h;
    stack[sptr].l = dat->l;
    sptr++;
}

struct stackItem *pop() {
    sptr--;
    return &stack[sptr];
}

struct stackItem *preview() {
    return &stack[sptr - 1];
}

void setOnStack(int addr, struct stackItem *dat) {
    stack[addr].h = dat->h;
    stack[addr].l = dat->l;
}

struct stackItem *getFromStack(int addr) {
    return &stack[addr];
}

int getStackPointer() {
    return sptr;
}

void setStackPointer(int newValue) {
    sptr = newValue;
}

void stacktrace() {
    int i;
    if (!debug) return;
    fprintf(stdout, "---- stack trace ---------------------->\n");
    for (i = getStackPointer() - 1; i > 0; i--) {
        printf(" [%03d]=[%3d.%3d]\n", i, getFromStack(i)->h, getFromStack(i)->l);
    }
    fprintf(stdout, "<--- stack trace -----------------------\n");
}

void printdebug() {
    fprintf(stdout, "\n\n");
    stacktrace();
    fprintf(stdout, "---- register trace ------------------->\n");
    fprintf(stdout, "- pc=%3d; cmd: %4s %d %10s | stack=%3d | base=%3d\n", pc, codebefehle[code[pc].o], code[pc].l,
            code[pc].a, sptr, base);
    fprintf(stdout, "<--- register trace --------------------\n\n");
}

int isFloatOnTop() {
    return preview()->l >= 0;
}

int isIntOnTop() {
    return preview()->l < 0;
}

/* --- Suchen CommandoCode --- */
int searchOpcode(char *oc) {
    int i;
    int len = (sizeof(codebefehle) / sizeof((codebefehle)[0]));
    /*fprintf(stderr, "len= %d\n,%3d %3d",len,sizeof(codebefehle),sizeof((codebefehle)[0]));*/
    for (i = 0; i < len; i++) /* lenge der struct codebefehle verwenden */
    {
        /*fprintf(stderr, "i= %d\n",i);*/
        if (strcmp(codebefehle[i], oc) == 0) {
            return i;
        }
    }
    return -1;
}

void readcode(int *count)
/* read intermediate code => in instr*/
{
    int no, o, k;
    char oc[5];

    *count = 0;
    while (!feof(intcode) && *count < maxcode) {
        k = fscanf(intcode, "%3d  %4s  %2d %s",
                   &no, oc, &code[*count].l,
                   &code[*count].a);
        if (debug)
            fprintf(stdout, "<- %03d  %-4s %02d %s\n", no, oc, code[*count].l, &code[*count].a);
        if (k < 4) break;

        /*fprintf(stderr, "-oc-> >%s<\n",oc);*/
        o = searchOpcode(oc);
        /*fprintf(stderr, "--o-> >%d<\n",o);*/
        if (o == -1) {
            fprintf(stderr, "Unknown opcode symbol %s\n", oc);
            exit(1);
        }
        code[(*count)++].o = o;
    }
    if (*count == maxcode) {
        fprintf(stderr, "Program too long.");
        exit(1);
    }
    (*count)--;
}

/* --- Hier wird was gemacht --- */
int main(int argc, char *argv[])
/*------------------------------------------------------------------
  The main function of the interpreter reads the intermediate
  code file and interpretes itc content.
  ------------------------------------------------------------------*/
{
    int i, tmp, count;
    char cname[20];
    char *p;

    /* check arguments */
    if (argc == 1) {
        fprintf(stderr, "Usage: %s codefile [-d|-d2]\n", argv[0]);
        exit(1);
    }

    if (argc > 2) {
        if (strcmp("-d", argv[2]) == 0)
            debug = 1;
        if (strcmp("-d2", argv[2]) == 0)
            debug = 2;
    } else
        debug = 0;

    /* set up filenames */
    strcpy(cname, argv[1]);
    p = strchr(cname, '.');
    if (p) *p = 0;
    strcat(cname, ".cod");

    /* open file */
    if ((intcode = fopen(cname, "r")) == NULL) {
        fprintf(stderr, "Cannot open intermed. code file %s\n",
                cname);
        exit(1);
    }

    if (debug)
        fprintf(stdout, "\n<------------- load\n");

    /* read code into array */
    readcode(&count);

    if (debug)
        fprintf(stdout, "<------------- done\n");

    /* initialization of stack */
    pushInt(0);
    pushInt(0);

    /* initialization of registers */
    base = 0;
    pc = 0;

    jumpswitch = 1;

    if (debug)
        fprintf(stdout, "\n-------------> run");

    /* main loop */
    do {

        if (!jumpswitch) pc++;
        if (pc >= maxcode || pc < 0) {
            fprintf(stderr, "\nProgramcounter runned out of range! \nPlease check your postfix-code! \n");
            exit(1);
        }
        jumpswitch = 0;

        if (debug == 1)
            fprintf(stdout, "\n-> pc: %03d cmd: %-4s %01d %10s;  | stack [%3d]=[%3d.%3d] | base=%3d    ", pc,
                    codebefehle[code[pc].o], code[pc].l, code[pc].a, sptr, preview()->h, preview()->l, base);
        if (debug == 2)
            printdebug();

        switch (code[pc].o) {
            int itmp;
            float ftmp;

            case NUL :
                break;

            case ADD :
                ftmp = popFloat();
                pushFloat(popFloat() + ftmp);
                break;

            case SUB :
                ftmp = popFloat();
                pushFloat(popFloat() - ftmp);
                break;

            case MPY :
                ftmp = popFloat();
                pushFloat(popFloat() * ftmp);
                break;

            case DVD :
                ftmp = popFloat();
                pushFloat(popFloat() / ftmp);
                break;

            case NEG :
                pushFloat(-popFloat());
                break;

            case ODD :
                pushInt(popInt() % 2);
                break;

            case EQL :
                ftmp = popFloat();
                pushFloat(popFloat() == ftmp ? 1 : 0);
                break;

            case NEQ :
                ftmp = popFloat();
                pushFloat(popFloat() != ftmp ? 1 : 0);
                break;

            case LSS :
                ftmp = popFloat();
                pushFloat(popFloat() < ftmp ? 1 : 0);
                break;

            case GTR :
                ftmp = popFloat();
                pushFloat(popFloat() > ftmp ? 1 : 0);
                break;

            case LEQ :
                ftmp = popFloat();
                pushFloat(popFloat() <= ftmp ? 1 : 0);
                break;

            case GEQ :
                ftmp = popFloat();
                pushFloat(popFloat() >= ftmp ? 1 : 0);
                break;

            case ILIT :
                /*fprintf(stderr, "\n*ilit=%d\n",char2int(code[pc].a));*/
                pushInt(char2int(code[pc].a));
                break;

            case FLIT :
                /*fprintf(stdout, "*flit- %f", char2float(code[pc].a));
                fprintf(stdout, "*flit* %s", code[pc].a);*/
                pushFloat(char2float(code[pc].a));
                break;

            case RET :
                tmp = code[pc].l;
                for (i = tmp; i > 0; i--)    /*Uebergabe Parameter vom Stack*/
                {                    /*nehmen und zwischenspeichern */
                    parameter[i] = *pop();
                    /*fprintf(stdout,">i=%d",i);*/
                }

                setStackPointer(base + 2);
                pc = popInt();
                base = popInt();
                /*fprintf(stdout," pc=%d, base=%d ",pc,base);*/
                for (i = 1; i <= tmp; i++)    /*Uebergabe Parameter auf den */
                {                    /*aktuellen Stack legen*/
                    push(&parameter[i]);
                    /*fprintf(stdout,"<i=%d",i);*/
                }
                break;

            case CALL :
                tmp = code[pc].l;
                for (i = tmp; i > 0; i--)    /*Uebergabe Parameter vom Stack*/
                {                    /*nehmen und zwischenspeichern */
                    parameter[i] = *pop();
                    /*fprintf(stdout,">i=%d",i);*/
                }

                itmp = getStackPointer();
                pushInt(base);
                pushInt(pc);
                base = itmp;
                pc = char2int(code[pc].a);
                /*fprintf(stderr, "# CALL pc= %3d \n", pc);*/
                jumpswitch = 1;

                for (i = 1; i <= tmp; i++)    /*Uebergabe Parameter auf den */
                {                    /*aktuellen Stack legen*/
                    push(&parameter[i]);
                    /*fprintf(stdout,"<i=%d",i);*/
                }
                break;

            case ILOD :
                itmp = base;
                if (code[pc].l == 0) itmp = 0;
                push(getFromStack(itmp + char2int(code[pc].a)));
                preview()->l = -1; /* sicherstellen: int */
                break;

            case ISTO :
                itmp = base;
                if (code[pc].l == 0) itmp = 0;
                preview()->l = -1; /* sicherstellen: int */
                setOnStack(itmp + char2int(code[pc].a), pop());
                break;

            case LOD :
                itmp = base;
                if (code[pc].l == 0) itmp = 0;
                push(getFromStack(itmp + char2int(code[pc].a)));
                break;

            case STO :
                itmp = base;
                if (code[pc].l == 0) itmp = 0;
                setOnStack(itmp + char2int(code[pc].a), pop());
                break;

            case IINT :
            case FINT :
                sptr += char2int(code[pc].a);
                break;

            case JPU :
                jumpswitch = 1;
                pc = char2int(code[pc].a);
                break;

            case JPC :
                itmp = popInt();
                if (itmp == 0) {
                    jumpswitch = 1;
                    pc = char2int(code[pc].a);
                }
                break;

            case INP :
                /*scanf("%d", &stack[++sptr]);*/
                scanf("%d", &itmp);
                pushInt(itmp);
                break;

            case PRN :
                if (isIntOnTop())
                    printf("  %6d", popInt());
                else
                    printf("  %6f", popFloat());
                break;

            case IPRN :
                printf("  %6d", popInt());
                break;

            case FPRN :
                printf("  %6f", popFloat());
                break;

            case PRS :
                for (i = 1; i <= char2int(code[pc].a); i++)
                    printf("%s", code[pc + i].a);
                pc += char2int(code[pc].a);
                break;

            case PRL :
                printf("\n");
                break;

            case SPC :
                printf(" ");
                break;

            case PDBG :
                itmp = debug;
                debug = 1;
                printdebug();
                debug = itmp;
                break;

            case STDB :
                debug = code[pc].l;
                break;

            case SOUT :
                printf("    %s", code[pc].a);
                break;

            default  :
                fprintf(stderr, "\nUnknown command %4s pc= %3d opcode %d\n", codebefehle[code[pc].o], pc, code[pc].o);
                exit(1);
        }

    } while (pc != 0);

    /* close files */
    fclose(intcode);
    if (debug)
        fprintf(stdout, "\n-------------> done\n");
    if (debug)
        fprintf(stdout,
                "\nRegister-Endzustand:\n   pc: %3d cmd: %4s %d %10s;  | stack [%3d]=[%3d.%3d] | base=%3d            \n",
                pc, codebefehle[code[pc].o], code[pc].l, code[pc].a, sptr, preview()->h, preview()->l, base);
    stacktrace();
    return 0;
}
