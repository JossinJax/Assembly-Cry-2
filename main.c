/*
 * Author: Josselyn Munoz
 * Title: Assembler_Pass_2
 * Language: C
 * Course: COP3404
 * Prof: Scott Piersall
 * Description: Implentation of pass 2 of an Assembler in a 32-bit SIC Computer
 * Architecture Last Updated: 12/16/21
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

	while (fgets(line, MAX, fp) != NULL) // keeps reading as long as there are contents
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
			strcpy(token1, strtok(line, " \t\n\r"));
			strcpy(token2, strtok(NULL, " \t\n\r"));
			strcpy(token3, strtok(NULL, " \t\n\r"));

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
						token3,
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
				token3,
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
			strcpy(token2, strtok(line, " \t\n\r"));
    
			if (strcmp(token2, "RSUB") == 0) ////////////////////
			{
				insertOp(
					symTab,
					token2,
					token3,//"WWWWWWWWWWWWWW",
					lineCount,
					pc,
					&symCount,
					getFormat(token2, token3),
					getOp(token2));
				pc += 3;  /////////////////////////////////
				continue; ///////////////////////////////
			}
			if ((IsADirective(token2) || IsAnInstruction(token2)) &&
				token3 != NULL) {
        strcpy(token3, strtok(NULL, " \t\n\r"));
				insertOp(
					symTab,
					token2,
					token3,
					lineCount,
					pc,
					&symCount,
					getFormat(token2, token3),
					getOp(token2));
				pc = pc + getFormat(token2, token3);
				continue;
			} else {
				pc += 3;
				continue;
			}
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


// printf("\nsymCount is: %ld\n", symCount);
// 	printf("The Symbol Table: \n");
// 	for (int i = 0; i < symCount; i++)
// 		printf(
// 			"%s\t  %-7s%-7X%-7s%-2d\n",
// 			symTab[i]->symName,
// 			symTab[i]->opName,
// 			(int)symTab[i]->Address,
// 			symTab[i]->opCode,
// 			symTab[i]->format);















	// H Record
	printf("%s\n", makeHRecord(symTab, length, pc));

	char *pBuff = malloc(70 * sizeof(char));
	memset(pBuff, 48, sizeof(char) * 70);
	pBuff[69] = '\0';

  char RecordArray[100][70];
  memset(RecordArray, '\0', sizeof(char) *7000);
  
  char* Erec = malloc( 8 * sizeof(char));

    int counter = 1;
    //Creating the T Records
	for (int i = 1; i < symCount; i++) {
	
    	if(IsADirective(symTab[i]-> opName))
		{
       		if(strcmp(symTab[i]-> opName,"END") == 0) 
			{
         		sprintf(Erec,"%s\n", makeERecord(symTab,symCount));
				counter++; 
			}
			else
			{
				sprintf(RecordArray[i],"%s\n", makeDirtRecord(symTab, symCount, i));
				counter++;			
			}
        }
		else
		{	
			sprintf(RecordArray[i],"%s\n", makeTRecord(symTab, symCount, i));
			counter++;
		}
		
	}//end of loop


	//creating the M records
	
	

	for (int i = 1; i < symCount; i++) 
	{
		for (int j = 0; j < symCount; j++) 	
		{
			if (strcmp((symTab[j]->symName), symTab[i]->token3) == 0) 
			{
				sprintf(RecordArray[counter],"%s\n", makeMRecord(symTab, symCount, i));
				counter++;
			}
		}
	}

	sprintf(RecordArray[counter],"%s\n", makeERecord(symTab, symCount));
	counter++;

	
	
  int reCount = 0;	
  while(reCount != counter)
  {
      printf("%s",RecordArray[reCount]);
	  reCount++;
  }

	free(token1);
	free(token2);
	free(token3);
	fclose(fp);
	return 0;

} // end of main
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Function Name: makeMRecord
 * Input Parameters: SYMBOL* sPointerArray[], OPCODES* oPointerArray[], char*
 * token1, char* token2, char* token3, long p Description: returns a T-Record
 * string Return Value: char* tRecord
 */
char *makeMRecord(SYMBOL *symTab[], unsigned long symCount, int currentIndex) {
	// local variables
	char *addy = malloc(6);//starting address of mod
	char *format = malloc(2);//length of mod
	char *modFlag = malloc(1);
	char *code = malloc(6);
	char *trash = malloc(100);
	char *MRecord = calloc(17, sizeof(char));
	MRecord[0] = 'M'; // column 0
    int searchedIndex = search(symTab, symCount, symTab[currentIndex]->token3);
	// enters the starting address of the object code in the 2 - 7th columns
	int temp0 = strlen(ltoa(symTab[currentIndex]->Address, trash, 16));
	if (temp0 > 6) {
		printf("ASSEMBLY ERROR: INVALID M-RECORD\n");
		exit(0);
	}

	sprintf(addy, "%06X", (int)symTab[currentIndex]->Address);
	strcat(MRecord, addy);

	// insert the length of the object code
	//printf("Format is: %02X\n", (int)symTab[currentIndex]->format);
	sprintf(format, "%02X", symTab[currentIndex]->format);
	strcat(MRecord, format);



	if(symTab[currentIndex]->Address > 8000 || symTab[currentIndex]->Address    )
	{
		strcat(MRecord, modFlag);


	}
	else
	{
		printf("ASSEMBLY ERROR: INVALID T-RECORD\n");
		exit(0);
	}

  if(searchedIndex != -1){
    //printf("Searched symbol is: %s", symTab[searchedIndex]->symName);
	sprintf(code,"%s%X",symTab[currentIndex]->opCode,(int)symTab[searchedIndex]->Address); // Search for symbol in symbol table
  //printf("Searched: %s",symTab[searchedIndex]->token3);
	  strcat(MRecord, code);
    MRecord[69] = '\0'; // last column
	  return MRecord;
   }
   else if(strstr(symTab[currentIndex]->token3, ",X") != NULL)
   {
		strcpy(code, strtok(symTab[currentIndex]->token3, ","));
    strcpy(symTab[currentIndex]->token3, code);
	// else if (searchedIndex == -1){
    searchedIndex = search(symTab, symCount, symTab[currentIndex]->token3);
	// printf("Modified symbol is: %s\n", symTab[currentIndex]->token3);
    // printf("Searched symbol is: %s\n", symTab[searchedIndex]->symName);
    // printf("Searched index is: %d\n", searchedIndex);
    // printf("Address at index is: %X\n", (int)symTab[searchedIndex]->Address);
		 
	 	 
		 sprintf(code,"%s%X",symTab[currentIndex]->opCode,(int)symTab[searchedIndex]->Address); // Search for symbol in symbol table
	 //printf("Searched: %s",symTab[searchedIndex]->token3);
	  strcat(MRecord, code);
	// 	tRecord[69] = '\0'; // last column
	 	return MRecord;
	}
  {
	  printf("Fail String: %s\n", symTab[currentIndex]->token3);
    return "Failed -----T RECORD------\n";
  }
	return "Failed -----T RECORD------\n";
}
/*
 * Function Name: makeTRecord
 * Input Parameters: SYMBOL* sPointerArray[], OPCODES* oPointerArray[], char*
 * token1, char* token2, char* token3, long p Description: returns a T-Record
 * string Return Value: char* tRecord
 */
char *makeTRecord(SYMBOL *symTab[], unsigned long symCount, int currentIndex) {
	// local variables
	char *addy = malloc(6);
	char *format = malloc(2);
	char *code = malloc(6);
	char *trash = malloc(100);
	char *tRecord = calloc(70, sizeof(char));
	tRecord[0] = 'T'; // column 0
    int searchedIndex = search(symTab, symCount, symTab[currentIndex]->token3);
	// enters the starting address of the object code in the 2 - 7th columns
	int temp0 = strlen(ltoa(symTab[currentIndex]->Address, trash, 16));
	if (temp0 > 6) {
		printf("ASSEMBLY ERROR: INVALID T-RECORD\n");
		exit(0);
	}

	sprintf(addy, "%06X", (int)symTab[currentIndex]->Address);
	strcat(tRecord, addy);

	// insert the length of the object code
	//printf("Format is: %02X\n", (int)symTab[currentIndex]->format);
	sprintf(format, "%02X", symTab[currentIndex]->format);
	strcat(tRecord, format);

// 	// insert the object code
//   printf("opCode is: %s\n", symTab[currentIndex]->opCode);

  if(searchedIndex != -1){
    //printf("Searched symbol is: %s", symTab[searchedIndex]->symName);
	sprintf(code,"%s%X",symTab[currentIndex]->opCode,(int)symTab[searchedIndex]->Address); // Search for symbol in symbol table
  //printf("Searched: %s",symTab[searchedIndex]->token3);
	  strcat(tRecord, code);
    tRecord[69] = '\0'; // last column
	  return tRecord;
   }
   else if(strstr(symTab[currentIndex]->token3, ",X") != NULL)
   {
		strcpy(code, strtok(symTab[currentIndex]->token3, ","));
    strcpy(symTab[currentIndex]->token3, code);
	// else if (searchedIndex == -1){
    searchedIndex = search(symTab, symCount, symTab[currentIndex]->token3);
	// printf("Modified symbol is: %s\n", symTab[currentIndex]->token3);
    // printf("Searched symbol is: %s\n", symTab[searchedIndex]->symName);
    // printf("Searched index is: %d\n", searchedIndex);
    // printf("Address at index is: %X\n", (int)symTab[searchedIndex]->Address);
		 
	 	 
		 sprintf(code,"%s%X",symTab[currentIndex]->opCode,(int)symTab[searchedIndex]->Address); // Search for symbol in symbol table
	 //printf("Searched: %s",symTab[searchedIndex]->token3);
	  strcat(tRecord, code);
	// 	tRecord[69] = '\0'; // last column
	 	return tRecord;
	}
  {
	  printf("Fail String: %s\n", symTab[currentIndex]->token3);
    return "Failed -----T RECORD------\n";
  }
	return "Failed -----T RECORD------\n";
}
/*(int)
 * Function Name: makeDirtRecord
 * Input Parameters: SYMBOL* sPointerArray[], OPCODES* oPointerArray[], char*
 * token1, char* token2, char* token3, long p Description: returns a T-Record
 * string Return Value: char* tRecord
 */
char* makeDirtRecord(SYMBOL *symTab[], unsigned long symCount, int currentIndex)
{
	// local variables
	char *addy = malloc(6);
	char *format = malloc(2);
	char *code = malloc(59);
	char* chub = malloc(59);
	char *trash = malloc(100);
	char *tRecord = calloc(70, sizeof(char));
	tRecord[0] = 'T'; // column 0
    int searchedIndex = search(symTab, symCount, symTab[currentIndex]->token3);
	// enters the starting address of the object code in the 2 - 7th columns
	int temp0 = strlen(ltoa(symTab[currentIndex]->Address, trash, 16));
	if (temp0 > 6) {
		printf("ASSEMBLY ERROR: INVALID T-RECORD\n");
		exit(0);
	}
	//concat the starting address
	sprintf(addy, "%06X", (int)symTab[currentIndex]->Address);
	strcat(tRecord, addy);

	// insert the length of the object code
	//printf("Format is: %02X\n", (int)symTab[currentIndex]->format);
	sprintf(format, "%02X", symTab[currentIndex]->format);
	strcat(tRecord, format);
    //printf("current op be: %s\n", symTab[currentIndex]->opName);
	// insert the object code
	//printf("opCode is: %s\n", symTab[currentIndex]->opCode);
    if(strcmp(symTab[currentIndex]->opName, "RSUB") == 0)
    {
        printf("RSUB found\n");
    	sprintf(code,"%s", "000000");
    }
	else if((strcmp(symTab[currentIndex]->opName, "RESW") == 0))
		sprintf(code,"%06lX",strtol(symTab[currentIndex]->token3, &trash, 10) * 3);	

	else if((strcmp(symTab[currentIndex]->opName, "RESB") == 0))
		sprintf(code,"%06lX", strtol(symTab[currentIndex]->token3, &trash, 10));
	
	else if((strcmp(symTab[currentIndex]->opName, "WORD") == 0))
      sprintf(code,"%06lX", strtol(symTab[currentIndex]->token3, &trash, 10));

	else if((strcmp(symTab[currentIndex]->opName, "BYTE") == 0))
	{
		//printf("BRUHHHHH: %s %s\n",symTab[currentIndex]->opName, symTab[currentIndex]-> token3);
		// int len = strlen(strtok(token3, "X'"));
		if (strstr(symTab[currentIndex]->token3, "X'") != NULL && strlen(strtok(symTab[currentIndex]->token3, "X'")) % 2 == 0) {
			for (int i = 2; i <= strlen(strtok(symTab[currentIndex]->token3, "X'")) + 1; i++) {
				if (isxdigit(symTab[currentIndex]->token3[i]) || symTab[currentIndex]->token3[i] == 39 || symTab[currentIndex]->token3[i] == 96)
					continue;
				else {
					printf(
						"ASSEMBLY ERROR 1: INVALID HEX VALUE: %s: %c\n,",
						symTab[currentIndex]->token3,
						symTab[currentIndex]->token3[i]);
					exit(0);
				}
			}
				for (int i = 2; i <= strlen(strtok(symTab[currentIndex]->token3, "X'")) + 1; i++)
				  sprintf(code,"%s%X",symTab[currentIndex]->opCode, (unsigned int)strtol(symTab[currentIndex]->token3, &trash, 16));
        //for (int i = 2; i <= strlen(strtok(symTab[currentIndex]->token3, "X'")) + 1; i++)
				  //printf("%s%X\n",symTab[currentIndex]->opCode, (unsigned int)strtol(symTab[currentIndex]->token3, &trash, 10));
		//end of x
		} else if (strstr(symTab[currentIndex]->token3, "C'") != NULL) {
        for (int i = 2; i <= strlen(strtok(symTab[currentIndex]->token3, "C'")) + 1; i++) {
          if (symTab[currentIndex]->token3[i] == 39 || symTab[currentIndex]->token3[i] == 96 || symTab[currentIndex]->token3[i] == 32 || (symTab[currentIndex]->token3[i] > 65 && symTab[currentIndex]->token3[i] < 90 )){
				    sprintf(chub, "%X", symTab[currentIndex]->token3[i]);
		  	    strcat( code, chub);
            	continue;
		      }
          else {
            printf(
              "ASSEMBLY ERROR 2: INVALID HEX VALUE: %s: %c\n,",
              symTab[currentIndex]->token3,
              symTab[currentIndex]->token3[i]);
            exit(0);
          }
        }
        sprintf(code,"%s",symTab[currentIndex]->opCode);
        char tempChar[] = "\0\0";
          for (int i = 2; i <= strlen(strtok(symTab[currentIndex]->token3, "C'")) + 1; i++)
          {
             sprintf(tempChar,"%X", symTab[currentIndex]->token3[i]);
             strcat(code,tempChar);
          }
            
		//end of c
		} else {
			printf(
						"ASSEMBLY ERROR: 3 INVALID HEX VALUE: %s:\n,",
						symTab[currentIndex]->token3);
					exit(0);
		}

	}
	else
		return "ASSEMBLY ERROR: makeDirTRecord() Failed\n";
		
    /*for RESW,RESB,WORD use token 3 and strtoi or similar
    */
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
char *makeHRecord(SYMBOL *structPointerArray[], unsigned long length, long pc) 
{
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
	for (int i = 0, j = 0; j < temp0;
		 i++, j++) // this loop fills the record with the name
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
 * Function Name: makeMRecord
 * Input Parameters: SYMBOL* sPointerArray[], OPCODES* oPointerArray[], char*
 * token1, char* token2, char* token3, long p Description: returns a T-Record
 * string Return Value: char* tRecord
 */
// char* makeMRecord()
// {

// }
/*
 * Function Name: makeERecord
 * Input Parameters: SYMBOL* sPointerArray[], OPCODES* oPointerArray[], char*
 * token1, char* token2, char* token3, long p Description: returns a T-Record
 * string Return Value: char* tRecord
 */
char* makeERecord(SYMBOL *symTab[], unsigned long symCount){
	char *eRecord = malloc(sizeof(char) * 8);
	memset(eRecord, '\0' , sizeof(char) * 20);
	char *target = malloc(sizeof(char) * 8);
	memset(target, '\0' , sizeof(char) * 20);
	char *addy = calloc(6, sizeof(char));
	
	eRecord[0] = 'E';

	for (int i = 0; i < symCount; i++)
	{  
		if(strcmp(symTab[i]->opName, "End"))
		{
			strcpy(target, symTab[i]->token3); 
				for (int j = 0; j < symCount; j++)
				{
					if(strcmp(symTab[j]->symName, target))
					{
						sprintf(addy, "%06X", (unsigned int)symTab[j]->Address);
						strcat(eRecord, addy);	
						return eRecord;				
					}
				}
		}	
	}		

	return "ASSEMBLY ERROR: E-Record cannot be produced\n";
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
	char *opNam,
	char *t3,
	unsigned int lineCount,
	unsigned int programCounter,
	unsigned long *symCount,
	short form,
	char *opC) {
	SYMBOL *newEntry = malloc(sizeof(SYMBOL));
	strcpy(newEntry->symName, newSymbol);
	strcpy(newEntry->opName, opNam);
	strcpy(newEntry->token3, t3);
	strcpy(newEntry->opCode, opC);
	newEntry->lineCount = lineCount;
	newEntry->Address = programCounter;
	newEntry->format = form;
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
	char *opNam,
	char *t3,
	unsigned int lineCount,
	unsigned int programCounter,
	unsigned long *symCount,
	short form,
	char *opC) {
	SYMBOL *newEntry = malloc(sizeof(SYMBOL));
    strcpy(newEntry->symName, "NULL");
	strcpy(newEntry->token3, t3);
	strcpy(newEntry->opName, opNam);
	strcpy(newEntry->opCode, opC);
	newEntry->lineCount = lineCount;
	newEntry->Address = programCounter;
	newEntry->format = form;
	// printf("opCode being inserted: %s\n",opCode);
	structPointerArray[*symCount] = newEntry; // before I had: structPointerArray[*symCount++] = newEntry
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

/*
 * Function Name: ltoa
 * Author: Josselyn Munoz from sources
 * Input Parameters: long decimal, char* buffer, int base
 * Description: converts a long into the given base then to a string
 * Return Value: char* str
 */
// searches the table for a symbol and returns the index location if found
// Else returns -1 for error an error
int search(SYMBOL *symTab[], unsigned long symCount, char *testSym) {
	int index = -1;
	for (int i = 0; i < symCount; i++) {
		if (strcmp(testSym, symTab[i]->symName) == 0) {
			index = i;
			break;
		}
	}
	return index;
}

/*
 * Function Name: ltoa
 * Author: Josselyn Munoz from sources
 * Input Parameters: long decimal, char* buffer, int base
 * Description: converts a long into the given base then to a string
 * Return Value: char* str
 */
int searchOperator(SYMBOL *symTab[], unsigned long symCount, char *testOP) {
	int index = -1;
	for (int i = 0; i < symCount; i++) {
		if (strcmp(testOP, symTab[i]->opName) == 0) {
			index = i;
			break;
		}
	}
	return index;
}







/*
OOF = 001008
OOF BYTE C'Hello, this is a very long character message, but is still within
bounds.' 78 > 69
//START OF BYTE T REC 50C  //Count how many chars stored in the T record,
increment the starting address by that many
//STARTS AT END OF LAST ONE T0956
T001008000F05929394972194754203523023458656..."messag..e.."
T00140509123145105929583984

T0100 -->
T0103 -->
main()
{
if(BYTE)
{
  linked list start; [LLS] //I make a linked list to store all the characters
that have been parsed func to parse characters(LLS); //I pass it to my function
to populate the linked list
  //From here I actually make the T record using the LLS
  while(LLS is not empty or I am not at the end)
  {
	make t Record from LLS;
	Increment address by T record;
	LLS position + 1;
  }
}

}
int temp = strlen(byte constant);
char* tempo = calloc(60) chars);
for(int i = 0; i < temp; i++)
{
  parse char into hex;
  if(strlen(tempo) == 59)
  {
	LLS+1
	strcpy(LLS+1,tempo);
	memset(tempo, '\0',sizeof(char)*60);
  }
}

*/