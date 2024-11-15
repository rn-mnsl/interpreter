/* Sebesta simple Lexical Analyzer example */

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "token.h"

/* Global declarations */

/* Variables */
int charClass;
char lexeme[100];
char c;
int lexLen;
int token;
int nextToken;
FILE *in_fp;

/* Function declarations */
void addChar();
char getChar();
char getNonBlank();
void lex();
void add_token(TokenType token);
void number();
void identifier();
bool match(char expected);
void string();

/******************************************************/
/* main driver */
int main() {
    /* Open the input data file and process its contents */
    const char* fname = "file.txt";
    in_fp = fopen(fname, "rb");

    if (in_fp == NULL) {
        printf("ERROR - cannot open file \n");
        return 1;
    }

    do {
        c = getChar();
        lex();
        printf("Next token is: %d, Next lexeme is %s\n", nextToken, lexeme);
    } while (nextToken != EOF);

    return 0;
}

/******************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = c;
        lexeme[lexLen] = '\0';
    } else {
        printf("Error - lexeme is too long \n");
    }
}

/******************************************************/
/* getChar - a function to get the next character of input */
char getChar() {
    return getc(in_fp);
}

/******************************************************/
/* getNonBlank - a function to call getChar until it returns a non-whitespace character */
char getNonBlank() {
    while (isspace(c)) {
        c = getChar();
    }
    return c;
}

/******************************************************/
/* lex - a simple lexical analyzer for arithmetic expressions */
void lex()
{
    lexLen = 0;
    c = getNonBlank();
    // Parse identifiers
    if (isdigit(c)) return number();

    // Parse number literals
    if (isalpha(c)) return identifier();

    // Parse one- or two-character tokens
    switch (c) {
        // Single-character operators
        case '(': return add_token(LEFT_PARENTHESIS);
        case ')': return add_token(RIGHT_PARENTHESIS);
        case '[': return add_token(LEFT_BRACKET);
        case ']': return add_token(RIGHT_BRACKET);
        case '{': return add_token(LEFT_BRACE);
        case '}': return add_token(RIGHT_BRACE);
        case ',': return add_token(COMMA);
        case ';': return add_token(SEMICOLON);
        case '*': return add_token(MULTIPLY);
        case '^': return add_token(EXPONENT);

        // Multi-character operators
        case '+': return add_token(match('+') ?  INCREMENT : PLUS);
        case '-': return add_token(match('-') ?  DECREMENT : MINUS);
        case '=': return add_token(match('=') ? EQUAL : ASSIGN);
        case '/': return add_token(match('/') ? COMMENT : DIVIDE);
        case '>': return add_token(match('=') ? GREATER_EQUAL : EQUAL);
        case '<': return add_token(match('=') ? LESS_EQUAL : EQUAL);
        case '!': return add_token(match('=') ? NOT_EQUAL : EQUAL);
        case '|': return add_token(match('|') ? OR : OR);
        case '&': return add_token(match('&') ? OR : OR);

        default:
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = '\0';
            nextToken = EOF;

    }
}

/******************************************************/
/* add_token - updates next_token and lexeme for printing */
void add_token(TokenType token)
{
    addChar();
    nextToken = token;
}

/******************************************************/
/* match - helper function for multi-character operators */
bool match(char expected)
{
    // Peek at next character
    char nextChar = getChar();

    // Push the character back to stream for later reading if not what we expected
    if (nextChar != expected) {
        ungetc(nextChar, in_fp);
        return false;
    }

    // The next character matches expected and forms the token
    addChar(); // This adds the first character of the token (the one that matched the case), the second char is done by addChar
    c = nextChar;
    return true;
}

/******************************************************/
/* number - reads the rest of the number literal */
void number()
{
    addChar();  // Add the first digit we already found

    char nextChar = getChar();
    if (isspace(nextChar)) return; // Return fast when number ends

    while (isdigit(nextChar)) {
        c = nextChar;
        addChar();
        nextChar = getChar();
    }

    // Check if the non-digit following the integer part of the number is a period
    char char_after_non_digit;
    if (isdigit(char_after_non_digit = getChar()) && nextChar == '.' )
    {
        // Put back the digit we peeked at
        ungetc(char_after_non_digit, in_fp);

        // Add the decimal point
        c = nextChar;
        addChar();

        // Read the fractional part
        nextChar = getChar();
        while (isdigit(nextChar)) {
            c = nextChar;
            addChar();
            nextChar = getChar();
        }
    }

    // Put back the last non-digit character we found
    ungetc(nextChar, in_fp);
    nextToken = NUMBER;
}

/******************************************************/
/* identifier - reads the rest of the identifier */
void identifier()
{
    // Add the first character we found
    addChar();

    char nextChar = getChar();
    while (isalnum(nextChar) || nextChar == '_') {  // isalnum checks for letters or digits
        c = nextChar;
        addChar();
        nextChar = getChar();
    }

    // Put back the last non-alphanumeric character we found
    ungetc(nextChar, in_fp);
    nextToken = IDENTIFIER;
}