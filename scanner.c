#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "plz_glob.h"

/*Zeiger zum Lesen/Schreiben auf bzw. in die dateien*/

FILE *quell_File, *listing, *token;

int linenr = 1;
int symbol, nummer;
int zaehl = 0;
int x, b, d;
int lauf = 0, y;
int anzahl_chars;
int ist_kommentar = 0;
int ist_kommentarA = 0;
int hilf = 0;
int flag = 0;
int flagE = 0;
int hilfE = 0;
int flag_plus = 0;
int flag_minus = 0;
int v;
int cnt_kommentar = 0;
int hilf_punkt = 0;
int lauf_punkt;
int gefunden = 0;
int cnt_float = 0;

/*Puffer fuer die Stringeingabe*/
char strings[string_max];
char c = ' ';
char a = ' ';

/*Liest ein Zeichen aus der Quelldatei und schreibt es in die Datei listing */

int lese_Zeichen() {
    char ch;
    ch = fgetc(quell_File);

    if (ch != EOF) {
        fputc(ch, listing);
    }
    if (ch == '\n') {
        fprintf(listing, "%d", linenr++);

        fprintf(token, "%d  %d\n", zeile, linenr);
    }

    return ch;
}

/*Diese Funktion untersucht die gelesenen Zeichen und unterscheidet
 welcher Art das gelesene Symbol ist.
*/
int pruefe_float(int c) {

    hilf = zahl[0];

    if ((cnt_float > 1) && (hilf > 3)) {
        printf("Zu grosse Floatzahl in Zeile %d gewaehlt!\n", linenr);
        return (0);
    }

    if (ist_kommentar == 0) {
        symbol = NUMBER_FLOAT;
    }
    return symbol;
}

int getsym() {

    c = lese_Zeichen();

    if ((c == EOF) && (cnt_kommentar > 0)) {
        printf("Fehlenders Ende-Kommentar-Zeichen in Zeile: %d!\n", linenr - 1);
        return (0);
    }

    anfang:
/*Beliebig viele Leerzeichen werden gelesen*/
    while (isspace(c)) {
        c = lese_Zeichen();
    }

    if (ist_kommentar == 0) {
        ist_kommentarA = 0;
    }


    /*Ident gefunden muss mit einem Buchstaben beginnen und darf nur kleinbuchstaben enthalten*/

    if ((isalpha(c) && islower(c)) || (c == '_')) {
        if (c == '_') {


        }
        for (x = 0; x <= char_max; x++) {
            idents[x] = 0;
        }
        anzahl_chars = 0;

        if (ist_kommentar == 0) {
            symbol = ident;
        }


        /*max char_max zeichen lesen fuer ident*/
        do {
            if (anzahl_chars <= (char_max - 1)) {
                idents[anzahl_chars] = c;
                c = lese_Zeichen();
            } else {
                if (ist_kommentar == 0) {
                    fprintf(token, "%d %s\n  ", ident, idents);
                    printf("Identifer in Zeile %d zu lang!\n", linenr - 1);
                }
                do {
                    c = lese_Zeichen();
                } while (c == ';' || c == '=');
                return (0);

            }
            anzahl_chars++;

            if (isupper(c)) {
                printf("Fehlerhafter Identifer in Zeile %d !\n", linenr - 1);
                return (0);
            }
        } while (isalpha(c) || isdigit(c) || c == '_');

        if (ist_kommentar == 0) {
            fprintf(token, "%d  %s\n", ident, idents);
        }
        goto anfang;

    }


        /*Zahl wird gefunden und so lange eingelesen, wie Zahlen kommen*/
    else if (isdigit(c)) {
        if (ist_kommentar == 0) {
            symbol = NUMBER_INT;
            flagE = 0;
        }

        lauf = 0;
        for (v = 0; v <= 45; v++) {
            zahl[v] = 0;
        }

        do {
            switch (c) {
                case '0':
                    nummer = 0;
                    break;
                case '1':
                    nummer = 1;
                    break;
                case '2':
                    nummer = 2;
                    break;
                case '3':
                    nummer = 3;
                    break;
                case '4':
                    nummer = 4;
                    break;
                case '5':
                    nummer = 5;
                    break;
                case '6':
                    nummer = 6;
                    break;
                case '7':
                    nummer = 7;
                    break;
                case '8':
                    nummer = 8;
                    break;
                case '9':
                    nummer = 9;
                    break;
            }

            zahl[lauf] = nummer;
            c = lese_Zeichen();

            lauf++;
            /*Fehlerueberpruefung fuer float*/
            if (lauf > 45) {
                printf("Zu grosse Floatzahl in Zeile %d gewaehlt!\n", linenr);
                return (0);
            }

            if (c == '.') {
                hilf_punkt = lauf - 1;
                flag = 1;
                pruefe_float(c);
            } else if (c == 'E') {
                hilfE = lauf - 1;
                flagE = 1;
            } else if (c == '-') {
                flag_minus = 1;

            } else if (c == '+') {
                flag_plus = 1;

            }
            if (flagE == 1) {
                if (isdigit(c)) {
                    cnt_float++;
                }
            }
        } while (isdigit(c) || (c == '.') || (c == 'E') || (c == '-') || (c == '+'));


/*******************************  Integer Zahlen  *****************************************************/
        /*Fehlermeldungen nur, wenn keine float zahl */
        if (flag == 0) {

            if (lauf > 5) {
                printf("Ausserhalb des zulaessigen Zahlenbereiches 1 in Zeile %d\n", linenr);
                return (0);
            }
            if (lauf == 5) {
                if (zahl[0] > 3) {
                    printf("Ausserhalb des zulaessigen Zahlenbereiches2 in Zeile %d\n", linenr - 1);
                    return (0);
                } else if ((zahl[0] > 3) || (zahl[1] > 2)) {
                    printf("Ausserhalb des zulaessigen Zahlenbereiches 3in Zeile %d\n", linenr);
                    return (0);
                } else if ((zahl[0] > 3) || (zahl[1] > 2) || (zahl[2] > 7)) {
                    printf("Ausserhalb des zulaessigen Zahlenbereiches in Zeile %d\n", linenr);
                    return (0);
                } else if ((zahl[0] > 3) || (zahl[1] > 2) || (zahl[2] > 7) || (zahl[3] > 6)) {
                    printf("Ausserhalb des zulaessigen Zahlenbereiches4 in Zeile %d\n", linenr);
                    return (0);
                } else if ((zahl[0] > 3) || (zahl[1] > 2) || (zahl[2] > 7) || (zahl[3] > 6) || (zahl[4] > 7)) {
                    printf("Ausserhalb des zulaessigen Zahlenbereiches 5 in Zeile %d\n", linenr);
                    return (0);
                }
            }
        }
        if (ist_kommentar == 0) {
            fprintf(token, "%d  ", symbol);
        }

        if (flag == 0 && (ist_kommentar == 0)) {
            for (d = 0; d <= lauf - 1; d++) {
                fprintf(token, "%d", zahl[d]);
            }
        }

/********************************  Float zahlen ****************************************/
        if (flag == 1) {

            /************  Fehlerbehandlungen  *****************/
            if (cnt_float > 2) {
                printf("Es duerfen nur 2 Exponentenstellen in Zeile : %d gewaehlt werden!\n", linenr);
                return (0);
            } else if ((cnt_float > 1) && (zahl[0] > 3)) {
                printf("Zu grosse Float-Zahl in Zeile: %d gewaehlt\n", linenr);
                return (0);
            } else if ((cnt_float > 1) && (hilf_punkt > 0)) {

                printf("Zu grosse Float-Zahl in Zeile: %d gewaehlt\n", linenr);
                return (0);

            }



            /***********  Ausgabe der Zahl  ************/
            for (lauf_punkt = 0; lauf_punkt <= hilf_punkt; lauf_punkt++) {

                fprintf(token, "%d", zahl[lauf_punkt]);
            }
            fprintf(token, ".");
            if (flagE == 1) {
                lauf = hilfE;
            } else {
                lauf = lauf - 1;

            }
            if (hilf_punkt > 45) {
                printf("Zu viele Stellen bei Float-Zahl in Zeile %d!\n", linenr);
            }
            for (d = hilf_punkt + 2; d <= lauf; d++) {

                fprintf(token, "%d", zahl[d]);

            }
            if (flagE == 1) {
                fprintf(token, "E");
            }
            flagE = 0;


            if (flag_plus == 1) {

                fprintf(token, "+");
                if ((cnt_float == 2) && (zahl[lauf + 3] > 3)) {
                    printf("Zu grossen Exponenten in Zeile %d gewaehlt!\n", linenr);
                    return (0);
                }
                if ((cnt_float == 2) && (zahl[lauf + 3] > 3) && (zahl[lauf + 4] > 8)) {
                    printf("Zu grossen Exponenten in Zeile %d gewaehlt!\n", linenr);
                    return (0);
                }
                /* Zwei zahlen als Exponent */

                if (cnt_float == 2) {

                    fprintf(token, "%d", zahl[lauf + 3]);
                    fprintf(token, "%d", zahl[lauf + 4]);
                    cnt_float = 0;
                }
                    /*nur eine Azhl als Exponent*/
                else {

                    fprintf(token, "%d", zahl[lauf + 3]);
                    cnt_float = 0;
                }
                flag = 0;
                flag_plus = 0;
            }
            if (flag_minus == 1) {
                fprintf(token, "-");
                if ((cnt_float == 2) && (zahl[lauf + 3] > 3)) {
                    printf("Zu grossen Exponenten in Zeile %d gewaehlt!\n", linenr);
                    return (0);
                }
                if ((cnt_float == 2) && (zahl[lauf + 3] > 3) && (zahl[lauf + 4] > 8)) {
                    printf("Zu grossen Exponenten in Zeile %d gewaehlt!\n", linenr);
                    return (0);
                }
                /* Zwei zahlen als Exponent */

                if (cnt_float == 2) {

                    fprintf(token, "%d", zahl[lauf + 3]);
                    fprintf(token, "%d", zahl[lauf + 4]);
                    cnt_float = 0;
                }
                    /*nur eine Azhl als Exponent*/
                else {

                    fprintf(token, "%d", zahl[lauf + 3]);
                    cnt_float = 0;
                }

                flag = 0;
                flag_minus = 0;
            }


            flag = 0;
        }
        if (ist_kommentar == 0) {
            fprintf(token, "\n");
        }
        goto anfang;
    } else if (c == ':') {
        symbol = colon;

    } else if (c == ';') {
        symbol = semicolon;

    } else if (c == ',') {
        symbol = comma;

    } else if (c == '=') {
        symbol = gleich;

    } else if (c == '(') {
        symbol = lparen;

    } else if (c == ')') {
        symbol = rparen;

    } else if (c == '^') {
        symbol = hoch;

    } else if (c == '+') {
        symbol = plus;

    } else if (c == '-') {
        symbol = minus;

    } else if (c == '*') {
        c = lese_Zeichen();

        if (c == '/') {
            cnt_kommentar--;
            if (cnt_kommentar == 0) {
                ist_kommentarA = 1;
                ist_kommentar = 0;
                symbol = kommentar_ende;
            }
        } else symbol = times;

    } else if (c == '.') {

        symbol = dot;

    } else if (c == '/') {

        c = lese_Zeichen();
        if (c == '*') {
            cnt_kommentar++;
            ist_kommentar = 1;
            symbol = kommentar_anfang;
        } else
            symbol = slash;

    } else if (c == EOF) {

        return 0;
    } else if (isalpha(c) && isupper(c) && (ist_kommentar == 0)) {

        zaehl = 0;

        for (x = 0; x <= key_max; x++) {
            key_words[x] = 0;
        }
        do {

            key_words[zaehl] = c;
            zaehl++;
            c = lese_Zeichen();


        } while (isalpha(c) && isupper(c) && (ist_kommentar == 0));

        for (b = 0; b <= 37; b++) {

            if (strcmp(key_words, res_woerter[b]) == 0) {
                symbol = b;
                gefunden = 1;
                break;
            }
        }
        if (gefunden == 0) {
            printf("Unzulaessiges Schluesselwort/Ident in Zeile: %d \n", linenr);
            gefunden = 0;
            return (0);
        }

        fprintf(token, "%d\n", symbol);
        /*       fprintf(token, "%d  %s\n", symbol,key_words);*/

        /* fuer die naechste Runde */
        gefunden = 0;
        goto anfang;

    } else if (c == '"') {
        symbol = text;
        zaehl = 0;
        do {
            c = lese_Zeichen();
            if (isalpha(c) || isdigit(c) || isupper(c)) {
                for (x = 0; x <= string_max; x++) {
                    strings[x] = 0;
                }

                do {
                    strings[zaehl] = c;
                    zaehl++;
                    c = lese_Zeichen();

                    if (zaehl >= string_max) {
                        fprintf(token, "%s ", strings);
                        printf("Maximale Stringlaenge in Zeile %d ueberschritten!\n", linenr);
                        do {
                            c = lese_Zeichen();

                        } while (c == ';' || c == '.' || c == '"');
                        return (0);
                    }
                } while (isalpha(c) || isspace(c) || isdigit(c) || isupper(c));
            }
        } while (c == '"');
    }

    return 1;
}/* ende getSym*/

int main(int argc, char *argv[]) {
    char sname[20], lname[20], tname[20];
    char *p;

    if (argc == 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        return (0);
    }

    /*Endungen der Dateien ergaenzen*/
    strcpy(sname, argv[1]);
    p = strchr(sname, '.');
    if (p) *p = 0;


    strcpy(lname, sname);
    strcpy(tname, sname);
    strcat(sname, ".plz");
    strcat(lname, ".lst");
    strcat(tname, ".tok");


    if ((quell_File = fopen(sname, "r")) == 0) {
        printf("Quelldatei kann nicht geoeffnet werden %s! \n", sname);
        return (0);
    }

    if ((listing = fopen(lname, "w")) == 0) {
        printf("Listing Datei kann nicht geoeffnet werden %s! \n", lname);
        return (0);
    }
    if ((token = fopen(tname, "w")) == 0) {
        printf("Token Datei kann nicht geoefnet werden %s! \n", tname);
        return (0);
    }

    while (getsym()) {

        if ((symbol != text) && (symbol != FIXED) && (ist_kommentar == 0) && (ist_kommentarA == 0)) {
            fprintf(token, "%d  ", symbol);
        }

        if ((symbol == ident) && (ist_kommentar == 0)) {
            fprintf(token, "%s\n", idents);

        } else if ((symbol == text) || (symbol == schluesselwort)) {
            fprintf(token, "%d ", symbol);
            fprintf(token, "%2s\n", strings);

        } else {
            if (ist_kommentar == 0) {
                fprintf(token, "\n");
            }

        }

    }
    fclose(quell_File);
    fclose(listing);
    fclose(token);

    return (0);

}/*ende Main*/

