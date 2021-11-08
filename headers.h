#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

//global variables

#define MAX 2024
#define _CRT_SECURE_NO_WARNINGS

//structs and types

struct symbols {
        int      lineCount;
		short    format;
        long     Address;
		char	 opCode[3];
        char     symName[7];//6 chars per symbol + null   
		char     opName[8]; //7chars per opCode + null
}; 
typedef struct symbols SYMBOL;

struct opcodes {
		int     lineCount;
		short	 format;
		long     Address;
        char     opCode[3];
        char     Name[8];//7 chars per opcode
};
typedef struct opcodes OPCODES;


//function prototypes
void insertSym(SYMBOL* structPointerArray[], char* newSymbol, char* opName, unsigned int lineCount, unsigned int programCounter, unsigned long* symCount, short format, char* opCode);



void insertOp(SYMBOL* structPointerArray[], char* opName, unsigned int lineCount, unsigned int programCounter, unsigned long* opCount, short format, char* opCode);


char* makeHRecord(SYMBOL* structPointerArray[], unsigned long length, long pc);
char* makeTRecord(SYMBOL* structPointerArray[]);
char* getFormatOp(char* nextoken);
char* ltoa(long value, char* buffer, int base);
unsigned short getFormat(char* token2, char* token3);
char* getOp(char* opCode);
void flushScanf(void);
int IsAValidSymbol( char *TestSymbol );
int IsADirective( char *Test );
int IsAnInstruction( char *Test );