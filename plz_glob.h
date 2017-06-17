/* Rekursiever Compiler fuer PL/Z
Modul: Globale Deffinitionen und Deklarationen

Morris Milekovic

*/

#define zeile -1
#define COMMON  0
#define CONST   1
#define DECLARE 2
#define DISPOSE 3
#define DIV     4
#define DO      5
#define ELSE    6
#define ELSEIF   7
#define END     8
#define EQ      9
#define FLOAT   10
#define FIXED   11 
#define GE      12 
#define GET     13
#define GT      14
#define IF      15 
#define IN      16
#define LIST    17
#define MAIN    18 
#define MOD     19 
#define NE      20 
#define NEW     21
#define OUT     22
#define POINTER 23
#define PROC    24
#define PROTO   25
#define PUT     26 
#define SKIP    27 /* New Line*/
#define TEXT    28 
#define TO      29 
#define UNTIL   30
#define WHILE   31
#define LT      32
#define AND     33
#define OR      34
#define THEN	35
#define expo   36
#define LE    37
#define colon  38
#define semicolon 39
#define comma 40
#define lparen 41
#define rparen 42
#define hoch 43
#define plus 44
#define minus 45
#define kommentar_ende 46 
#define times 47
#define dot 48
#define kommentar_anfang 47
#define slash 50
#define ident 51
#define text 52
#define schluesselwort 53
#define gleich 54
#define NUMBER_INT 55
#define NUMBER_FLOAT 56

/*Konstantendefinition*/
#define float_max 3.4 E38  /*max. Groesse fuer Float*/
#define i_max 32767        /*max. integer Wert*/
#define char_max 15        /*max. Laenge fuer idents*/
#define string_max 256     /*max-Laenge fuer Strings*/
#define key_max 15


int zahl[i_max];
/*Array fuer die gefunden idents*/
char idents[char_max];
char key_words[key_max];
char strings[string_max];


char *res_woerter[]={"COMMON",
"CONST",
"DECLARE",
"DISPOSE",
"DIV",
"DO",
"ELSE",
"ELSEIF",
"END",
"EQ",
"FLOAT",
"FIXED",
"GE",
"GET",
"GT",
"IF",
"IN",
"LIST",
"MAIN",
"MOD",
"NE", 
"NEW",
"OUT",
"POINTER",
"PROC",
"PROTO",
"PUT",
"SKIP",
"TEXT",
"TO",
"UNTIL",
"WHILE",
"LT",
"AND",
"OR",
"THEN",
"E",
"LE" 
};

