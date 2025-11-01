#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 500

// opcode enumeration
typedef enum {
    LIT = 1,
    OPR,
    LOD,
    STO,
    CAL,
    INC,
    JMP,
    JPC,
    SYS,
} OpCode;

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

typedef struct {
    int kind; // const = 1, var = 2, proc = 3
    char name[12]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
} symbol;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

int str_to_int(char* str) {
    char *end;

    int num = strtol(str, &end, 10);

    return num;
}

int symbol_table_check(char* name) {
    
}

int add_to_symbol_table(int kind, char* name, int value, int level, int address) {

}

void emit(FILE* outputFile, OpCode opCode, int l, int m) {

}

int peek(char** tokenArr, int* current, TokenType expectedToken) {

}

int expect_token(char** tokenArr, int* current, TokenType expectedToken) {
    if (str_to_int(tokenArr[*current]) != expectedToken) {
        return 0;
    }

    (*current)++;

    return 1;
}

void error(char* errorMessage) {
    printf("Error: %s", errorMessage);

    exit(1);
}

int expect_identifier(char** tokenArr, char* expectedIdentifier, int* current) {
    if (!expect_token(tokenArr, current, identsym)) {
        return 0;
    }

    if (strcmp(tokenArr[*current], expectedIdentifier) != 0) {
        return 0;
    }

    (*current)++;

    return 1;
}

int statement(char** tokenArr, int* current, int length, FILE* outputFile) {
    if (peek(tokenArr, current, identsym)) {
        (*current)++;

        char* identifier = tokenArr[*current];
        (*current)++;

        int symbolIndex = symbol_table_check(identifier);

        if (symbolIndex == -1) {
            error("undeclared identifier");
        }

        if (symbol_table[symbolIndex].kind != 2) {
            error("only variable values may be altered");
        }

        if (!expect_token(tokenArr, current, becomessym)) {
            error("assignment statements must use :=");
        }

        expression(tokenArr, current, length, outputFile);

        emit(outputFile, STO, 0, symbol_table[symbolIndex].addr);
    }

}

int var_declaration(char** tokenArr, int* current, int length, FILE* outputFile) {
    int numVars = 0;
    do {
        numVars++;
        expect_token(tokenArr, current, varsym);

        if (!expect_token(tokenArr, current, identsym)) {
            error("const, var, and read keywords must be followed by identifier");
        }

        char* identifier = tokenArr[*current];
        (*current)++;

        add_to_symbol_table(2, identifier, 0, 0, numVars + 2);
    } while(peek(tokenArr, current, commasym));

    if (!expect_token(tokenArr, current, semicolonsym)) {
        error("constant and variable declarations must be followed by a semicolon");
    };

    return numVars;
}

int const_declaration(char** tokenArr, int* current, int length, FILE* outputFile) {
    do {
        expect_token(tokenArr, current, constsym);

        if (!expect_token(tokenArr, current, identsym)) {
            error("const, var, and read keywords must be followed by identifier");
        }

        char* identifier = tokenArr[*current];
        (*current)++;

        if (symbol_table_check(identifier) != -1) {
            error("symbol name has already been declared");
        }

        if (!expect_token(tokenArr, current, eqsym)) {
            error("constants must be assigned with =");
        }

        if (!expect_token(tokenArr, current, numbersym)) {
            error("constants must be assigned an integer value");
        };

        int number = str_to_int(tokenArr[*current]);
        (*current)++;

        add_to_symbol_table(1, identifier, number, 0, 0);
    } while(peek(tokenArr, current, commasym));

    if (!expect_token(tokenArr, current, semicolonsym)) {
        error("constant and variable declarations must be followed by a semicolon");
    };
}

void block(char** tokenArr, int* current, int length, FILE* outputFile) {
    if (peek(tokenArr, current, constsym)) {
        const_declaration(tokenArr, current, length, outputFile);
    }
    if (peek(tokenArr, current, varsym)) {
        var_declaration(tokenArr, current, length, outputFile);
    }
    statement(tokenArr, current, length, outputFile);
}

void program(char** tokenArr, int* current, int length, FILE* outputFile) {
    block(tokenArr, current, length, outputFile);

    if (!expect_token(tokenArr, current, periodsym)) {
        error("program must end with period");
    }

    // emit halt
    emit(outputFile, SYS, 0, 3);
}

int main() {
    // read tokens file
    FILE* tokensFile = fopen("tokens.txt","r");
    if (!tokensFile) {
        perror("fopen");
        return 1;
    }

    int capacity = 500;
    char** tokenArr = malloc(capacity * sizeof(char*));

    char* buffer = malloc(500);

    int count = 0;
    while (fscanf(tokensFile, "%s", buffer) == 1) {
        if (count >= capacity - 1) {
            capacity *= 2;
            tokenArr = realloc(tokenArr, capacity * sizeof(char*));
        }

        tokenArr[count] = strdup(buffer);  
        count++;
    }

    for (int i = 0; i < count; ++i) {
        printf("%s ", tokenArr[i]);
    };
    printf("\n");

    for (int i = 0; i < count; i++) {
        free(tokenArr[i]);
    }
    free(tokenArr);

    return 0;
}