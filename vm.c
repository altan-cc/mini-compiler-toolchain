/*
Assignment :
vm . c - Implement a P - machine virtual machine
Authors : < Lucioano Paredes, Chen-An Chang >
Language : C ( only )
To Compile :
gcc - O2 - Wall - std = c11 -o vm vm . c
To Execute ( on Eustis ) :
./ vm input . txt
where :
input . txt is the name of the file containing PM /0 instructions ;
each line has three integers ( OP L M )
Notes :
- Implements the PM /0 virtual machine described in the homework
instructions .
- No dynamic memory allocation or pointer arithmetic .
- Does not implement any VM instruction using a separate function .
- Runs on Eustis .
Class : COP 3402 - Systems Software - Fall 2025
Instructor : Dr . Jie Lin
Due Date : Friday , September 12 th , 2025
*/

#include <stdio.h>

int pas[500];
int stack_top = 499;

struct {
    int OP;
    int L;
    int M;
} IR;

/* find base L levels down from the current activation record */
int base(int BP, int L) {
    int arb = BP; // activation record base
    while (L > 0) {
        arb = pas[arb]; // follow static link
        L--;
    }
    return arb;
}

void print(int instruction, int l, int m, int pc, int bp, int sp, int level, int *stack_sizes) {
    char *instruction_repr;
    switch (instruction) {
    case 1:
        instruction_repr = "LIT";
        break;
    case 2:
        switch (m) {
        case 0:
            instruction_repr = "RTN";
            break;
        case 1:
            instruction_repr = "ADD";
            break;
        case 2:
            instruction_repr = "SUB";
            break;
        case 3:
            instruction_repr = "MUL";
            break;
        case 4:
            instruction_repr = "DIV";
            break;
        case 5:
            instruction_repr = "EQL";
            break;
        case 6:
            instruction_repr = "NEQ";
            break;
        case 7:
            instruction_repr = "LSS";
            break;
        case 8:
            instruction_repr = "LEQ";
            break;
        case 9:
            instruction_repr = "GTR";
            break;
        case 10:
            instruction_repr = "GEQ";
            break;
        case 11:
            instruction_repr = "EVEN";
            break;
        }
        break;
    case 3:
        instruction_repr = "LOD";
        break;
    case 4:
        instruction_repr = "STO";
        break;
    case 5:
        instruction_repr = "CAL";
        break;
    case 6:
        instruction_repr = "INC";
        break;
    case 7:
        instruction_repr = "JMP";
        break;
    case 8:
        instruction_repr = "JPC";
        break;
    case 9:
        instruction_repr = "SYS";
        break;
    default:
        instruction_repr = "UNK";
        break;
    }

    printf("%s\t%d\t%d\t%d\t%d/%d\t%d\t", instruction_repr, l, m, pc, pas[base(bp, 1)], bp, sp);

    int print_buffer[500];
    int buff_stack_ptr = 499;
    int initial_level = level;

    while (level >= 0) {
        for (int i = stack_sizes[level] - 1, j = bp, k = buff_stack_ptr; i >= 0; i--, j--) {
            print_buffer[k - i] = pas[j];
            buff_stack_ptr--;
        }
        bp = base(bp, 1);
        level--;
    }

    level = 0;
    int counter_until_level = stack_sizes[level];

    for (int i = buff_stack_ptr + 1; i < 500; i++) {
        // printf("counter: %d level: %d\n", counter_until_level, level);
        if (level != initial_level && stack_sizes[level] != 0 && counter_until_level == 0) {
            printf(" | ");
        }
        if (counter_until_level == 0) {
            level++;
            counter_until_level = stack_sizes[level];
        }
        counter_until_level--;
        printf("%d ", print_buffer[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <elf-file>\n", argv[0]);
        return 1;
    }

    char *file_name = argv[1];
    FILE *file_pointer;
    file_pointer = fopen(file_name, "r");

    if (file_pointer == NULL) {
        fprintf(stderr, "File provided does not exist.\n");
        return -1;
    }

    int op;
    int l;
    int m;
    int sp = 499;

    // load instrictions into pas from file (stored backwards starting at top)
    while (fscanf(file_pointer, " %d %d %d", &op, &l, &m) == 3) {
        pas[sp] = op;
        pas[sp - 1] = l;
        pas[sp - 2] = m;
        sp -= 3;
    }
    fclose(file_pointer);

    // fill rest with zeroes
    for (int i = sp; i >= 0; i--) {
        pas[i] = 0;
    }

    sp += 1; // set sp to m of last instruction
    stack_top = sp;
    int pc = 499;
    int bp = sp - 1;

    // printf("last instruction: %d at index %d\n", pas[sp], sp);

    printf("\tL\tM\tPC\tBP\tSP\tstack\n");
    printf("Initial values:\t\t%d\t%d\t%d\n", pc, bp, sp);

    int level = 0;
    int stack_sizes[500];
    for (int i = 0; i < 500; i++)
        stack_sizes[i] = 0;

    while (pc >= 0) {
        // fetch cycle
        IR.OP = pas[pc];
        IR.L = pas[pc - 1];
        IR.M = pas[pc - 2];
        pc -= 3;

        // execute cycle
        // printf("processing pc: %d, op: %d\n", pc, IR.OP);
        switch (IR.OP) {
            case 1: {
                sp = sp - 1;
                pas[sp] = IR.M;
                stack_sizes[level]++;
                break;
            } case 2: {
                if (IR.M == 0) {
                    sp = bp + 1;
                    bp = pas[sp - 2];
                    pc = pas[sp - 3];
                    stack_sizes[level] = 0;
                    level--;
                } else if (IR.M == 1) {
                    pas[sp + 1] = pas[sp + 1] + pas[sp];
                    sp = sp + 1;
                    stack_sizes[level]--;
                } else if (IR.M == 2) {
                    pas[sp + 1] = pas[sp + 1] - pas[sp];
                    sp = sp + 1;
                    stack_sizes[level]--;
                } else if (IR.M == 3) {
                    pas[sp + 1] = pas[sp + 1] * pas[sp];
                    sp = sp + 1;
                    stack_sizes[level]--;
                } else if (IR.M == 4) {
                    pas[sp + 1] = pas[sp + 1] / pas[sp];
                    sp = sp + 1;
                    stack_sizes[level]--;
                } else if (IR.M == 5) {
                    pas[sp + 1] = (pas[sp + 1] == pas[sp]);
                    sp = sp + 1;
                    stack_sizes[level]--;
                } else if (IR.M == 6) {
                    pas[sp + 1] = (pas[sp + 1] != pas[sp]);
                    sp = sp + 1;
                    stack_sizes[level]--;
                } else if (IR.M == 7) {
                    pas[sp + 1] = (pas[sp + 1] < pas[sp]);
                    sp = sp + 1;
                    stack_sizes[level]--;
                } else if (IR.M == 8) {
                    pas[sp + 1] = (pas[sp + 1] <= pas[sp]);
                    sp = sp + 1;
                    stack_sizes[level]--;
                } else if (IR.M == 9) {
                    pas[sp + 1] = (pas[sp + 1] > pas[sp]);
                    sp = sp + 1;
                    stack_sizes[level]--;
                } else if (IR.M == 10) {
                    pas[sp + 1] = (pas[sp + 1] >= pas[sp]);
                    sp = sp + 1;
                    stack_sizes[level]--;
                } else if (IR.M == 11) {
                    pas[sp] = pas[sp] % 2 == 0;
                }
                break;
            } case 3: {
                sp = sp - 1;
                pas[sp] = pas[base(bp, IR.L) - IR.M];
                stack_sizes[level]++;
                break;
            } case 4: {
                pas[base(bp, IR.L) - IR.M] = pas[sp];
                sp = sp + 1;
                stack_sizes[level]--;
                break;
            } case 5: {
                pas[sp - 1] = base(bp, IR.L);
                pas[sp - 2] = bp;
                pas[sp - 3] = pc;
                bp = sp - 1;
                pc = 499 - IR.M;
                level++;
                break;
            } case 6: {
                sp = sp - IR.M;
                stack_sizes[level] += IR.M;
                break;
            } case 7: {
                pc = 499 - IR.M;
                break;
            } case 8: {
                if (pas[sp] == 0) {
                    pc = 499 - IR.M;
                }
                sp = sp + 1;
                stack_sizes[level]--;
                break;
            } case 9: {
                if (IR.M == 1) {
                    printf("Output result is: %d\n", pas[sp]);
                    sp = sp + 1;
                    stack_sizes[level]--;
                } else if (IR.M == 2) {
                    sp = sp - 1;
                    stack_sizes[level]++;
                    printf("Please Enter an Integer: ");
                    scanf("%d", &pas[sp]);
                } else if (IR.M == 3) {
                    print(IR.OP, IR.L, IR.M, pc, bp, sp, level, stack_sizes);
                    return 0;
                }
                break;
            }
        }
        print(IR.OP, IR.L, IR.M, pc, bp, sp, level, stack_sizes);
    }
    return 0;
}
