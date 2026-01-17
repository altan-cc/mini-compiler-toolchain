C-based mini compiler toolchain that implements the core stages of language processing for an extended PL/0 programming language. The system consists of a lexer, a recursive-descent parser with code generation, and a virtual machine that executes the generated assembly.

Lexical Analyzer (lex.c):
- Accepts one command-line argument: a PL/0 source file
- Tokenizes the input source code
- Identifies identifiers, numbers, keywords, and symbols
- Outputs tokens for use by the parser

Parser & Code Generator (parsercodegen_complete.c:
- Accepts no command-line arguments
- Input filename is hard-coded in the source
- Implements a recursive-descent parser for an extended PL/0 grammar
- Supports: Procedures, call statements, and if-then-else control structures
- Generates PM/0 assembly code according to the provided instruction set architecture (ISA)

Virtual Machine (vm.c):
- Executes the generated PM/0 assembly code
- Implements a stack-based execution model
- Supports all required instructions, including:
- EVEN instruction (OPR 0 11)
- Produces the final runtime output of the program

This project was developed as a class assignment to demonstrate understanding of compiler construction and language execution pipelines.
