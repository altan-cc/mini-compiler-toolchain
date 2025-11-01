/*
Assignment :
lex - Lexical Analyzer for PL /0
Author : Chen-An Chang, Luciano Paredes
Language : C ( only )
To Compile :
gcc - O2 - std = c11
-o lex lex . c
To Execute ( on Eustis ):
./ lex < input file >
where :
< input file > is the path to the PL /0 source program
Notes :
- Implement a lexical analyser for the PL /0 language .
- The program must detect errors such as
- numbers longer than five digits
- identifiers longer than eleven characters
- invalid characters .
- The output format must exactly match the specification .
- Tested on Eustis .
Class : COP 3402 - System Software - Fall 2025
Instructor : Dr . Jie Lin
Due Date : Friday , October 3 , 2025 at 11:59 PM ET
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_IDENT_LEN 11
#define MAX_NUM_LEN   5

// tokenType enumeration
typedef enum {
    skipsym = 1,
    identsym,
    numbersym,
    plussym,
    minussym,
    multsym,
    slashsym,
    eqsym,
    neqsym,
    lessym,
    leqsym,
    gtrsym,
    geqsym,
    lparentsym,
    rparentsym,
    commasym,
    semicolonsym,
    periodsym,
    becomessym,
    beginsym,
    endsym,
    ifsym,
    fisym,
    thensym,
    whilesym,
    dosym,
    callsym,
    constsym,
    varsym,
    procsym,
    writesym,
    readsym,
    elsesym,
    evensym
} TokenType;

// reserved words
typedef struct {
    const char *word;
    int token;
} resWord;

static resWord reserved[] = {
    {"begin", beginsym},
    {"end", endsym},
    {"if", ifsym},
    {"fi", fisym},
    {"then", thensym},
    {"while", whilesym},
    {"do", dosym},
    {"call", callsym},
    {"const", constsym},
    {"var", varsym},
    {"procedure", procsym},
    {"write", writesym},
    {"read", readsym},
    {"else", elsesym},
    {"even", evensym},
    {NULL, 0}
};

// check reserved words
int checkReserved(const char* s) {
    int i = 0;
    while (reserved[i].word) {
        if (strcmp(s, reserved[i].word) == 0) {
            return reserved[i].token;
        }
        i++;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    // we close the program if it doesn't have exactly 2 args.
    // the first argument is always the name of the program.
    if (argc != 2) {
        fprintf(stderr, "Usage: %s sourcefile\n", argv[0]);
        return 1;
    }

    // we open the file provided in the first argument in read mode
    FILE* in = fopen(argv[1],"r");
    if (!in) {
        fprintf(stderr,"Cannot open input file %s\n", argv[1]);
        return 1;
    }

    FILE* out = fopen("tokens.txt", "w");
    if (!out) {
        fprintf(stderr, "Cannot create tokens.txt\n");
        fclose(in);
        return 1;
    }

    int c;
    while ((c = fgetc(in)) != EOF) {
        if (isspace(c))
            continue;
        
        if (isalpha(c)) {
            char buff[32];
            int i = 0;
            while (isalnum(c) && i < 12) {
                buff[i++] = c;
                c = fgetc(in);
            }
            buff[i] = '\0';
            if (i > MAX_IDENT_LEN) {
                fprintf(out, "%d\n", skipsym);
                while (isalnum(c)) c = fgetc(in);
            }
            if (c != EOF)
                ungetc(c, in);

            int token = checkReserved(buff);
            if (token)
                fprintf(out, "%d\n", token);
            else
                fprintf(out, "%d %s\n", identsym, buff);
        }
        /* number */
        else if (isdigit(c)) {
            char buff[32];
            int i = 0;
            while (isdigit(c) && i < (MAX_NUM_LEN + 1)) {
                buff[i++] = c;
                c = fgetc(in);
            }
            buff[i] = '\0';
            if (i > MAX_NUM_LEN) {
                fprintf(out, "%d\n", skipsym);
                while (isdigit(c)) {
                    c = fgetc(in);
                }
            }
            if (isalpha(c)) {
                fprintf(out, "%d\n", skipsym);
                while (isalnum(c)) {
                    c = fgetc(in);
                }
            }
            if (c != EOF) {
                ungetc(c, in);
            }
            fprintf(out, "%d %s\n", numbersym, buff);
        }
        /* two-char symbols */
        else if (c == ':') {
            if ((c = fgetc(in)) == '=')
                fprintf(out, "%d\n", becomessym);
            else {
                fprintf(out, "%d\n", skipsym);
                if (c != EOF) ungetc(c, in);
            }
        } else if (c == '<') {
            if ((c = fgetc(in)) == '>')
                fprintf(out, "%d\n", neqsym);
            else if (c == '=')
                fprintf(out, "%d\n", leqsym);
            else {
                fprintf(out, "%d\n", lessym);
                if (c != EOF)
                    ungetc(c, in);
            }
        } else if (c == '>') {
            if ((c = fgetc(in)) == '=')
                fprintf(out, "%d\n", geqsym);
            else {
                fprintf(out, "%d\n", gtrsym);
                if (c != EOF)
                    ungetc(c, in);
            }
        }
        /* single-char symbols */
        else {
            switch (c) {
                case '+': {
                    fprintf(out, "%d\n", plussym);
                    break;
                }
                case '-': {
                    fprintf(out, "%d\n", minussym);
                    break;
                }
                case '*': {
                    fprintf(out, "%d\n", multsym);
                    break;
                }
                case '/': {
                    fprintf(out, "%d\n", slashsym);
                    break;
                }
                case '=': {
                    fprintf(out, "%d\n", eqsym);
                    break;
                }
                case '(': {
                    fprintf(out, "%d\n", lparentsym);
                    break;
                }
                case ')': {
                    fprintf(out, "%d\n", rparentsym);
                    break;
                }
                case ',': {
                    fprintf(out, "%d\n", commasym);
                    break;
                }
                case ';': {
                    fprintf(out, "%d\n", semicolonsym);
                    break;
                }
                case '.': {
                    fprintf(out, "%d\n", periodsym);
                    break;
                }
                default: {
                    fprintf(out, "%d\n", skipsym);
                    break;
                }
            }
        }
    }

    fclose(in);
    fclose(out);
    return 0;
}