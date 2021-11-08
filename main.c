/*
 * Author: Josselyn Munoz
 * Title: Assembler_Pass_2
 * Language: C
 * Course: COP3404
 * Prof: Scott Piersall
 * Description: Implentation of pass 1 of an Assembler in a 32-bit SIC Computer
 * Architecture Last Updated: 11/8/21
 */
#include "headers.h"

/////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
	// initial error-checking
	if (argc != 2) {
		printf("ASSEMBLY ERROR: Usage: %s filename\n", argv[0]);
		return 0;
	}

	// local variables
	FILE *fp;			 // reserved for streaming from the file
	SYMBOL *symTab[MAX]; // A struct pointer pointing to the beginning of an
						 // array of pointers aka our symbol table
	memset(symTab, '\0', MAX * sizeof(SYMBOL *));

	// OPCODES* opTab[MAX];
	// memset(opTab, '\0', MAX * sizeof(OPCODES*));

	char *token1 = calloc(MAX, sizeof(char));
	char *token2 = calloc(MAX, sizeof(char));
	char *token3 = calloc(MAX, sizeof(char));
	char *plength = calloc(MAX, sizeof(char));
	char line[MAX];
	char fullline[MAX];
	char buffer[MAX];
	unsigned long lineCount = 0;
	unsigned long start = 0;
	unsigned long pc = 0;
	unsigned long length = 0;
	unsigned long symCount = 0;
	char *trash = NULL;

	fp = fopen(argv[1], "r"); // begin reading from file

	if (fp == NULL) {
		printf(
			"ASSEMBLY ERROR: File: %s could not be opened for reading,\n",
			argv[1]);
		return 0;
	}

	while (fgets(line, MAX, fp) !=
		   NULL) // keeps reading as long as there are contents
	{
		strcpy(
			fullline,
			line); // one char buffer to another for filtering out token types
		++lineCount;

		if (line[0] == 35) // 35 is the ASCII value of ASCII symbol #
		{
			// printf("COMMENT: %s", line );
			continue; // immediately returns to while loop from here for the
					  // next iteration
		} else if ((line[0] >= 65) && (line[0] <= 90)) // if lines begin with
													   // text, it better be a
													   // symbol
		{
			strcpy(token1, strtok(line, " \t\n"));
			strcpy(token2, strtok(NULL, " \t\n"));
			strcpy(token3, strtok(NULL, " \t\n"));

			if ((IsAValidSymbol(token1) && !IsADirective(token1) &&
				 !IsAnInstruction(token1)) ||
				(!IsAValidSymbol(token2) &&
				 (IsADirective(token2) || IsAnInstruction(token2)))) {
				for (int i = 0; i < symCount;
					 i++) // iterate over symtab in search of dupes
				{
					if (strcmp((symTab[i]->symName), token1) == 0) {
						printf(
							"ASSEMBLY ERROR: DUPLICATE SYMBOL: %s \n", token1);
						return 0;
					}
				}
			} else if (IsADirective(token1) || IsAnInstruction(token1)) {
				printf("ASSEMBLY ERROR: INVALID SYMBOL: %s\n", token1);
				return (0);
			}

			if (strcmp(token2, "START") == 0) {
				pc = strtol(token3, &trash, 16);
				if (pc >= 32762) {
					printf(
						"ASSEMBLY ERROR: INVALID START ADDRESS: %s\n",
						ltoa(pc, buffer, 16));
					return 0;
				} else {
					start = pc;
					insertSym(
						symTab,
						token1,
						token2,
						lineCount,
						pc,
						&symCount,
						getFormat(token2, token3),
						getOp(token2)); //////////
					continue;
				}
			}

			insertSym(
				symTab,
				token1,
				token2,
				lineCount,
				pc,
				&symCount,
				getFormat(token2, token3),
				getOp(token2)); //////////////

			if (strcmp(token2, "END") == 0) {
				length = pc - start;
				strcpy(plength, ltoa(pc - start, buffer, 16));
			} else
				pc = pc + getFormat(token2, token3);

			continue;

		} else if (line[0] == 9 || line[0] == 32) // no token1
		{
			strcpy(token2, strtok(line, " \t\n"));
			if (strcmp(token2, "RSUB") == 0) ////////////////////
			{
				pc += 3; /////////////////////////////////
				insertOp(
					symTab,
					token2,
					lineCount,
					pc,
					&symCount,
					getFormat(token2, token3),
					getOp(token2));
				continue; ///////////////////////////////
			}

			if ((IsADirective(token2) || IsAnInstruction(token2)) &&
				token3 != NULL) {
				pc = pc + getFormat(token2, token3);
				insertOp(
					symTab,
					token2,
					lineCount,
					pc,
					&symCount,
					getFormat(token2, token3),
					getOp(token2));
			} else
				pc += 3;
			continue;
		} else {
			printf(
				"ASSEMBLY ERROR: INVALID LINE OR INVALID TOKEN FORMAT: %s \n",
				token1);
			free(token1);
			free(token2);
			free(token3);
			fclose(fp);
			return 0;
		}
	} // end of while

	// H Record
	printf("HREC: %s\n", makeHRecord(symTab, length, pc));

	// Printing from each of the tables in lineCount sequence
	// int current = 4;
	// for(int i = 1; i < symCount + opCount; i++)
	// {
	// 	if(symTab[i]->lineCount == ++current)
	// 		printf("%d\n", symTab[i]-> lineCount);
	// 	else
	// 		printf("%d\n", opTab[i]-> lineCount);;
	// 	current++;
	// }

	char *pBuff = malloc(70 * sizeof(char));
	memset(pBuff, 48, sizeof(char) * 70);
	pBuff[69] = '\0';
	printf("TREC: T00100003141033\n");
	// symTable
	printf("\nsymCount is: %ld\n", symCount);
	printf("The Symbol Table: \n");
	for (int i = 0; i < symCount; i++)
		printf(
			"%s\t  %-7s%-7s%-7s%-2d\n",
			symTab[i]->symName,
			symTab[i]->opName,
			ltoa(symTab[i]->Address, buffer, 16),
			symTab[i]->opCode,
			symTab[i]->format);

	printf("TREC: %s\n", makeTRecord(&symTab[1]));
	// printf("|TREC|: %ld\n", 1+strlen(makeTRecord(&symTab[1])));

	free(token1);
	free(token2);
	free(token3);
	fclose(fp);
	return 0;

} // end of main
 // /////////////////////(int)/////////////////////////////////////////////////////////////////////////////////////////////

// function(int)
// definitions//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*(int)
 * Function Name: makeTRecord
 * Input Parameters: SYMBOL* sPointerArray[], OPCODES* oPointerArray[], char*
 * token1, char* token2, char* token3, long p Description: returns a T-Record
 * string Return Value: char* tRecord
 */
char *makeTRecord(SYMBOL *symTab[]) {
	// local variables
	char *addy = malloc(6);
	char *format = malloc(2);
	char *code = malloc(6);
	char *trash = malloc(100);
	;
	char *tRecord = calloc(70, sizeof(char));
	tRecord[0] = 'T'; // column 0

	// enters the starting address of the object code in the 2 - 7th columns
	int temp0 = strlen(ltoa(symTab[1]->Address, trash, 16));
	if (temp0 > 6) {
		printf("ASSEMBLY ERROR: INVALID T-RECORD");
		exit(0);
	}

	printf("Name is: %s\n", symTab[0]->symName); // This is the wrong spot
	printf(
		"Address is: %06X\n",
		(int)symTab[0]->Address); // This is the wrong spot
	// printf("Name is: %s\n", symTab[1]->symName);
	printf(
		"Address is: %06X\n",
		(int)symTab[1]->Address); // This is the wrong spot
	// printf("Name is: %s\n", symTab[2]->symName);
	printf(
		"Address is: %06X\n",
		(int)symTab[2]->Address); // This is the wrong spot
	sprintf(addy, "%06X", (int)symTab[1]->Address);
	strcat(tRecord, addy);

	// insert the length of the object code
	printf("Format is: %02X\n", (int)symTab[1]->format);
	sprintf(format, "%02X", symTab[1]->format);
	strcat(tRecord, format);

	// insert the object code
	// printf("opCode is: %s\n",symTab[1]->opCode);
	sprintf(
		code,
		"%s%X",
		symTab[1]->opCode,
		(int)symTab[1]->Address); // Search for symbol in symbol table
	strcat(tRecord, code);

	tRecord[69] = '\0'; // last column
	return tRecord;
}
/*
 * Function Name: makeHRecord
 * Input Parameters: SYMBOL* StructPointerArray[], unsigned long length, long pc
 * Description: returns an H Record.
 * Return Value: char* hRecord
 */
char *makeHRecord(SYMBOL *structPointerArray[], unsigned long length, long pc) {
	char *hRecord = malloc(sizeof(char) * 20);
	// memset(hRecord,48, sizeof(char) * 20);
	hRecord[0] = 'H';

	char *name = malloc(6);
	memset(name, ' ', 6);
	int temp0 = strlen(structPointerArray[0]->symName);
	if (temp0 > 6) {
		printf("ASSEMBLY ERROR: INVALID H-RECORD NAME\n");
		exit(0);
	}
	for (int i = 0, j = 0; j < temp0; i++, j++) // this loop fills the record with the name
		name[i] = structPointerArray[0]->symName[j];

	strcat(hRecord, name);

	char *addy = calloc(6, sizeof(char));
	sprintf(addy, "%06X", (unsigned int)structPointerArray[0]->Address);
	strcat(hRecord, addy);

	char *plength = calloc(6, sizeof(char));
	sprintf(plength, "%06X", (unsigned int)length);
	strcat(hRecord, plength);

	hRecord[19] = '\0';
	return hRecord;
}
/*
 * Function Name: getOp
 * Input Parameters: char* str
 * Description: returns a three-char string the first of which designates
 * format, the second two designate the opcode. Return Value: char* str
 */
char *getOp(char *nextoken) {
	// error check
	if (nextoken == NULL) {
		printf("ASSEMBLY ERROR: INVALID TOKEN: %s\n", nextoken);
		exit(0);
	}

	// local variables
	char *key = calloc(3, sizeof(char));

	// logic returns format
	if (strcmp(nextoken, "ADD") == 0)
		key = "18";
	else if (strcmp(nextoken, "AND") == 0)
		key = "40";
	else if (strcmp(nextoken, "COMP") == 0)
		key = "28";
	else if (strcmp(nextoken, "DIV") == 0)
		key = "24";
	else if (strcmp(nextoken, "J") == 0)
		key = "3C";
	else if (strcmp(nextoken, "JEQ") == 0)
		key = "30";
	else if (strcmp(nextoken, "JGT") == 0)
		key = "34";
	else if (strcmp(nextoken, "JLT") == 0)
		key = "38";
	else if (strcmp(nextoken, "JSUB") == 0)
		key = "48";
	else if (strcmp(nextoken, "LDA") == 0)
		key = "00";
	else if (strcmp(nextoken, "LDCH") == 0)
		key = "50";
	else if (strcmp(nextoken, "LDL") == 0)
		key = "08";
	else if (strcmp(nextoken, "LDX") == 0)
		key = "04";
	else if (strcmp(nextoken, "MUL") == 0)
		key = "20";
	else if (strcmp(nextoken, "OR") == 0)
		key = "44";
	else if (strcmp(nextoken, "RD") == 0)
		key = "D8";
	else if (strcmp(nextoken, "RSUB") == 0)
		key = "4C";
	else if (strcmp(nextoken, "STA") == 0)
		key = "0C";
	else if (strcmp(nextoken, "STCH") == 0)
		key = "54";
	else if (strcmp(nextoken, "STL") == 0)
		key = "14";
	else if (strcmp(nextoken, "STSW") == 0)
		key = "E8";
	else if (strcmp(nextoken, "STX") == 0)
		key = "10";
	else if (strcmp(nextoken, "SUB") == 0)
		key = "1C";
	else if (strcmp(nextoken, "TD") == 0)
		key = "E0";
	else if (strcmp(nextoken, "TIX") == 0)
		key = "2C";
	else if (strcmp(nextoken, "WD") == 0)
		key = "DC";
	else if (IsADirective(nextoken))
		true;
	else {
		printf("ASSEMBLY ERROR: INVALID TOKENx: %s\n", nextoken);
		exit(0);
	}

	return key;
}
/*
 * Function Name: getFormat
 * Input Parameters: char* str
 * Description: returns the format of the next token.
 * Return Value: unsigned short
 */
unsigned short getFormat(char *token2, char *token3) {
	char **trash = NULL;

	// error check
	if ((token2 == NULL || token3 == NULL) ||
		(!IsADirective(token2) && !IsAnInstruction(token2))) {
		printf("ASSEMBLY ERROR: TOKEN IS NULL IN getFormat\n");
		return 0;
	} else if (strtol(token3, trash, 16) > 32760) {
		printf("ASSEMBLY ERROR: ADDRESS IS MAXED\n");
		exit(0);
	}

	if (strcmp(token2, "RESW") == 0)
		return strtol(token3, trash, 10) * 3;
	else if (strcmp(token2, "RESB") == 0)
		return strtol(token3, trash, 10);
	else if (strcmp(token2, "BYTE") == 0) {
		// int len = strlen(strtok(token3, "X'"));
		if (strstr(token3, "X'") != NULL &&
			strlen(strtok(token3, "X'")) % 2 == 0) {
			for (int i = 2; i <= strlen(strtok(token3, "X'")) + 1; i++) {
				if (isxdigit(token3[i]) || token3[i] == 39 || token3[i] == 96)
					continue;
				else {
					printf(
						"ASSEMBLY ERROR: INVALID HEX VALUE: %s: %c\n,",
						token3,
						token3[i]);
					exit(0);
				}
			}
			return (strlen(strtok(token3, "X'")) / 2);
		} else if (strstr(token3, "C'") != NULL)
			return (strlen(strtok(token3, "C'")));
		else if (strcmp(token3, "RSUB") == 0)
			return 3;
		else {
			printf("ASSEMBLY ERROR: TOKEN 3 IN getFormat\n");
			exit(0);
		}
	} else
		return 3;

	printf("ASSEMBLY ERROR: INVALID TOKEN\n");
	exit(0);
}
/*
 * Function Name: insertSym
 * Input Parameters: struct, char*, int, int, long
 * Description: Inserts a symbol into the symbol table
 * Return Value: Void
 */
void insertSym(
	SYMBOL *structPointerArray[],
	char *newSymbol,
	char *opName,
	unsigned int lineCount,
	unsigned int programCounter,
	unsigned long *symCount,
	short format,
	char *opCode) {
	SYMBOL *newEntry = malloc(sizeof(SYMBOL));
	strcpy(newEntry->symName, newSymbol);
	strcpy(newEntry->opName, opName);
	newEntry->lineCount = lineCount;
	newEntry->Address = programCounter;
	newEntry->format = format;
	strcpy(newEntry->opCode, opCode);
	structPointerArray[*symCount] =
		newEntry; // before I had: structPointerArray[*symCount++] = newEntry
				  // //but that didn't work
	*symCount += 1;
	return;
}
/*
 * Function Name: insertOp
 * Input Parameters: struct, char*, int, int, long
 * Description: Inserts a opCode into the opTable
 * Return Value: Void
 */
void insertOp(
	SYMBOL *structPointerArray[],
	char *opName,
	unsigned int lineCount,
	unsigned int programCounter,
	unsigned long *symCount,
	short format,
	char *opCode) {
	SYMBOL *newEntry = malloc(sizeof(SYMBOL));
	strcpy(newEntry->opName, opName);
	newEntry->lineCount = lineCount;
	newEntry->Address = programCounter;
	newEntry->format = format;
	// printf("opCode being inserted: %s\n",opCode);
	strcpy(newEntry->opCode, opCode);
	structPointerArray[*symCount] =
		newEntry; // before I had: structPointerArray[*symCount++] = newEntry
				  // //but that didn't work
	*symCount += 1;
	return;
}
/*
 * Function Name: IsaValidSymbol
 * Author: Scott Piersall
 * Input Parameters: char*
 * Description: validates the format of a symbol
 * Return Value: int
 */
int IsAValidSymbol(char *testToken) {
	int Result = 1;
	int index = 0;
	int maxlength = 6;
	int done;
	done = 0;

	while (!done) {
		if (testToken[index] == '\0') {
			break;
		}

		if (index == maxlength) {
			done = 1;
			Result = 0;
		}
		index++;
	}
	return Result;
}
/*
 * Function Name: IsAnInstruction
 * Author: Scott Piersall
 * Input Parameters: char*
 * Description: validates accepted Sic instructions in the scope of this project
 * Return Value: int
 */
int IsAnInstruction(char *Test) {
	return (!(

		strcmp(Test, "LDA") && strcmp(Test, "ADD") && strcmp(Test, "CLEAR") &&
		strcmp(Test, "COMP") && strcmp(Test, "DIV") && strcmp(Test, "FIX") &&
		strcmp(Test, "J") && strcmp(Test, "JEQ") && strcmp(Test, "JGT") &&
		strcmp(Test, "JLT") && strcmp(Test, "JSUB") && strcmp(Test, "AND") &&
		strcmp(Test, "LDCH") && strcmp(Test, "LDL") && strcmp(Test, "LDX") &&
		strcmp(Test, "LPS") && strcmp(Test, "MUL") && strcmp(Test, "OR") &&
		strcmp(Test, "RD") && strcmp(Test, "RMO") && strcmp(Test, "RSUB") &&
		strcmp(Test, "STA") && strcmp(Test, "STCH") && strcmp(Test, "STL") &&
		strcmp(Test, "STX") && strcmp(Test, "SUB") && strcmp(Test, "TD") &&
		strcmp(Test, "TIX") && strcmp(Test, "WD")));
}
/*
 * Function Name: IsADirective
 * Author Scott Piersall
 * Input Parameters: char*
 * Description: validates accepted Sic directives in the scope of this project
 * Return Value: int
 */
int IsADirective(char *Test) {
	return (!(
		strcmp(Test, "START") && strcmp(Test, "END") && strcmp(Test, "RESB") &&
		strcmp(Test, "RESW") && strcmp(Test, "RESR") && strcmp(Test, "BYTE") &&
		strcmp(Test, "WORD") && strcmp(Test, "EXPORTS")

			));
}

/*
 * Function Name: swap
 * Author: Josselyn Munoz
 * Input Parameters: char* element1, char* element2
 * Description: element2 will result in place of element1 and vice versa
 * Return Value: void
 */
void swap(char *x, char *y) {
	char t = *x;
	*x = *y;
	*y = t;
}

/*
 * Function Name: reverse
 * Author: Josselyn Munoz
 * Input Parameters: char*
 * Description: validates accepted Sic directives in the scope of this project
 * Return Value: int
 */
char *reverse(char *buffer, int i, int j) {
	while (i < j) {
		swap(&buffer[i++], &buffer[j--]);
	}

	return buffer;
}

/*
 * Function Name: ltoa
 * Author: Josselyn Munoz from sources
 * Input Parameters: long decimal, char* buffer, int base
 * Description: converts a long into the given base then to a string
 * Return Value: char* str
 */
char *ltoa(long value, char *buffer, int base) {
	// too large for the SIC
	if (base < 2 || base > 32) {
		return "ASSEMBLY ERROR AT HEX CONVERSION: INVALID DECIMAL INPUT\n";
		exit(0);
	}

	// consider the absolute value of the number

	int i = 0;
	while (value) {
		int r = value % base;

		if (r >= 10) {
			buffer[i++] = 65 + (r - 10);
		} else {
			buffer[i++] = 48 + r;
		}

		value = value / base;
	}

	// if the number is 0
	if (i == 0) {
		buffer[i++] = '0';
	}

	// If the base is 10 and the value is negative, the resulting string
	// is preceded with a minus sign (-)
	// With any other base, value is always considered unsigned
	if (value < 0 && base == 10) {
		buffer[i++] = '-';
	}

	buffer[i] = '\0'; // null terminate string as instructed in class

	// reverse the string and return it
	return reverse(buffer, 0, i - 1);
}
