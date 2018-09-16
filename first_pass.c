/*Tal Chaklai 200854982*/
/*Boris Manashirov 310511530*/
/*group: 41*/


#include "assembler.h"
#include <string.h>
#include <ctype.h>
extern int ic ;
extern int dc ;
const char* commandArray[] = {"mov",
    "cmp",
    "add",
    "sub",
    "lea",
    "not",
    "clr",
    "inc",
    "dec",
    "jmp",
    "bne",
    "red",
    "prn",
    "jsr",
    "rts",
    "stop"};
char*  registerArray[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};
/* insert new symbol node to the symbol list */
void insertToSymbolList(struct symbol* item) {
    struct symbol* x = NULL;
    if (!headSymbol) {   /*first symbol*/
        headSymbol = item;
        item -> next = NULL;
    }
    else {
        x = headSymbol;
        while (x -> next) {
            x = x -> next;
        }
        x -> next = item;
        item -> next = NULL;
    }
}
/* insert new data node to the data list */
void insertToDataList(struct datas* item) {
    struct datas* x = NULL;
    if (!headData) { /* first node in the list */
        headData = item;
        item -> next = NULL;
    }
    else {
        x = headData;
        while (x -> next) {
            x = x -> next;
        }
        x -> next = item;
        item -> next = NULL;
    }
    dc++;
}
/* insert new extern node to the extern list */
void insertToExternList(struct externs* item) {
    struct externs* x = NULL;
    if (!headExterns) { /* first node in the list */
        headExterns = item;
        item -> next = NULL;
    }
    else {
        x = headExterns;
        while (x -> next) {
            x = x -> next;
        }
        x -> next = item;
        item -> next = NULL;
    }
}
/* insert new entry node to the entry list */
void insertToEntryList(struct entries* item) {
    struct entries* x = NULL;
    if (!headEntries) {      /* first node in the list */
        headEntries = item;
        item -> next = NULL;
    }
    else {
        x = headEntries;
        while (x -> next) {
            x = x -> next;
        }
        x -> next = item;
        item -> next = NULL;
    }
}
/* insert new code node to the code list*/
void newLine (struct codes* item) {
    struct codes* x = NULL;
    if (!headCode) {
        headCode = item;
        item -> next = NULL;
    }
    else {
        x = headCode;
        while (x -> next) {
            x = x -> next;
        }
        x -> next = item;
        item -> next = NULL;
    }
    ic++;
}
/* free symbol list space  */
void freeSymbolsSpace(struct symbol* head) {
    struct symbol* x = NULL;
    while (head) {
        x = head;
        head = x -> next;
        free(x);
    }
}
/* free data list space */
void freeDataSpace(struct datas* head) {
    struct datas* x = NULL;
    while (head) {
        x = head;
        head = x -> next;
        free(x);
    }
}
/* free code list space */
void freeCodeSpace(struct codes* head) {
    struct codes* x = NULL;
    while (head) {
        x = head;
        head = x -> next;
        free(x);
    }
}
/* free external list space */
void freeExternsSpace(struct externs* head) {
    struct externs* x = NULL;
    while (head) {
        x = head;
        head = x -> next;
        free(x);
    }
}
/* free entry list space */
void freeEntriesSpace(struct entries* head) {
    struct entries* x = NULL;
    while (head) {
        x = head;
        head = x -> next;
        free(x);
    }
}
/* free all allocated space */
void freeAll() {
    freeSymbolsSpace(headSymbol);
    freeDataSpace(headData);
    freeCodeSpace(headCode);
    freeExternsSpace(headExterns);
    freeEntriesSpace(headEntries);
    ic = 100;
    dc = 0;
}

int skipSpaces(char *n, int pointer) {

    while (n[pointer] == ' ') { pointer++; }
    return pointer;
}

/*if the string is a command, return its nummber, otherwise return 0*/
int isCommand(char *x)
{
    int len = strlen(x);
    if (x[len - 1] == '\n') { x[len - 1] = '\0'; }
    int i;
    for (i=0; i<NUMBER_OF_COMMANDS; i++) {
        if (strcmp(x, commandArray[i]) == 0){
           
            return i+1;
        }
    }
    return NONCOMMAND;
    printf("\nis %s a command?\n", x);
    int commandNum=0;
    int index=0;
    
}



/*check if the label is valid, make sure its length is equal or less than 30 characters,
 first letter is alphabetic.
 if it is a label return 1, otherwise return 0 */

int isValidLabel (char * string) {
    int length = strlen(string);
    int i=0;
    char c[MAX_LABEL];
    if (length > MAX_LABEL || length < 1)
    {
        return FALSE;
    }
    else
    {
        while(string[i]==' ')
        {
            i++;
        }
        if(!isalpha(string[i]))
            return FALSE;
        else
        {
            while (i < length-1 && isalnum(string[i]) )
            {   i ++;}
            if(i < length-1 && !isalnum(string[i]))
                return FALSE;
            if((string[length-1]==':'))  /*if we are in the first pass, we already checked that the last char is":", so we can remove it and check that the label is not a register,command or directive words*/
            {
                char * string2 = (char*)malloc(length);
                strncpy(string2,string, length-1);
                string2[length-1]=0;
                string=string2;
 
            }
            if (! (isCommand(string)|| isRegister(string)|| isDirectiveWord(string)))    /*can not be one of these words*/
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }
}
/* check if the string is register, return 1 if it is, and 0 otherwise*/
int isRegister(char *x) {
    int index=0;
    while (x[index] == ' ')
    {
        index++;
    }
    char * reg= (char*)malloc(2); /*check the two letters that should represent a register*/
    strncpy(reg,x+index, 2);
    reg[2]='\0';
    int i = 0;
    for (i; i<7; i++)
    {
        if (strcmp (registerArray[i], reg) == 0)
        {
            index=index+2;         /*continue checking that there are only spaces after the register*/
            while (x[index] == ' ')
            {
                index++;
            }
            if (x[index] == '\0')
                return 1;
            else
                return 0;
        }
    }
    free(reg);
    return 0;
}
/* check if it a variable*/
int isVariable(char *x ,int index)
{
    if (x[index] == ' ')
    {
        index = skipSpaces(x, index);
    }
    while(x[index]) {
        if( isalnum(x[index]) )
        {
            index++;
        }
        else if (x[index] == ' ')
        {
            index = skipSpaces(x, index);
            break;
        }
        else
        {
            return FALSE;
        }
    }
    if (x[index] == '\0')
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
/*check if the string is a directive word, if it is, the method returns its number, else it returns 0 */
int isDirectiveWord(char *x)
{
	
  int len = strlen(x);
    if(x[0] == '.' && x[len - 1] == '\n')
	 { 
		x[len - 1] = '\0'; 
	 } /*  remove \n characters if needed */


    int res1, res2, res3, res4, res5;
    res1 = strcmp(x, ".data");
    res2 = strcmp(x, ".string");
    res3 = strcmp(x, ".struct");
    res4 = strcmp(x, ".entry");
    res5 = strcmp(x, ".extern");

    if(res1==0)
        return 1;
    else if(res2==0)
        return 2;
    else if(res3==0)
        return 3;
    else if(res4==0)
        return 4;
    else if(res5==0)
        return 5;
    else
        return 0;
}
/*check if the symbol is already found in the symbol list*/
int isLabelExists(char *label) {
    struct symbol* symb = headSymbol;
    if (symb == NULL)
    {
        return FALSE;
    }
    else
    {
        while (symb)
        {
            if (strcmp(symb -> name, label) == 0)
            {
                return TRUE;
            }
            symb = symb -> next;
        }
    }
    return FALSE;
}
/* copy characters to new array*/
char *mystrncpy (char *str, char *cpy, size_t n) {
    int i;
    char x;
    x = cpy;
    for (i = 0; i < n; i++)
    {
        *cpy++ = *str++;
    }
    cpy[i] = '\0';
    *str=*str-n-1;
    return x;
}
/* check if the string is a number */
int isNumber(char *str, int i)
{
    if (str[i] == ' ')
    {
        i = skipSpaces(str, i);
    }
    if ( (str[i] >= '0' && str[i] <= '9') || str[i] == '+' || str[i] == '-' )
    {
        i++;
        while (str[i])
        {
            if (str[i] >= '0' && str[i] <= '9')
            {
                i++;
            }
            else
            {
                return FALSE;
            }
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
/*  check if the given sub-string is a struct (from the current index).
 * returns 1 if it is a struct, 0 otherwise */
int isStruct(char *strct, int i) {
   
    while (strct[i] != '.') { /*B check if there is a valid name for the struct in the part until the dot '.' character */
        if ( isalnum(strct[i]) || ((strct[i] >= '0') || (strct[i]) <= '9')){
            i++;
           
        }
        else
            return 0;
    }
    i++;        /*move iterator to the field after the dot character*/
   
    if (strct[i] == ' ') {      /*  remove space characters if needed */
        i = skipSpaces(strct, i);
    }
    if ((strct[i] != '1') && (strct[i] != '2'))  /* if the struct tries to access fields other than 1 or 2, throw an error*/
        return 0;
    i++;
    if (strct[i] == ' ') {      /*  remove space characters if needed */
        i = skipSpaces(strct, i);
    }
    if (strct[i] != '\0')        /*Boris: if there is any other character after indication for field 1 / 2, throw an error */
        return 0;
    return 1;
}
/*add the number to the data list. if there are errors, it returns 0 */
int addNumber(char *num) {
    int i = 0;
    int minus = 0;
    int final_integer = 0;
    int buf_counter = 0;
    struct datas* new = NULL;
    char buf[100] = {0};
    if (num[i] == ' ')
        i = skipSpaces(num, i);
    if (num[i] == '+') {
        i++;
    }
    else if (num[i] == '-' && minus == 0) {
        minus = 1;
        i++;
    }
    if (num[i] == ' ')
        i = skipSpaces(num, i);
    while (num[i]) {
        if (num[i] >= '0' && num[i] <= '9') {
            buf[buf_counter] = num[i];
            buf_counter++;
            i++;
        }
        else if (num[i] == '\n')
            break;
        else if (num[i] == ' ') {
            i = skipSpaces(num, i);
            break;
        }
        else
            return 0;
    }
    if (num[i] != '\n' && num[i] != '\0')
        return 0;
    final_integer = atoi(buf);
    if (minus == 1) {
        final_integer = final_integer * -1;
    }
    new = (struct datas*)malloc(sizeof(dataTable));
    new -> address = dc;
    new -> data = final_integer;
    insertToDataList(new);
    return 1;
}
/* add the string to the data list. if there are errors, it returns 0 */
int addString(char *str) {
    struct datas* new = NULL;
    str[strlen(str)] = '\n';
    while ((*str) != '\"' && (*str) != '\n')
    {
        new = (struct datas*)malloc(sizeof(dataTable));
        new -> address = dc;
        new -> data = (*str);
        insertToDataList(new);
        str++;
    }
    if ((*str)=='\n') {
        *str='\0';
        return 0;
    }
    new = (struct datas*)malloc(sizeof(dataTable));
    new -> address = dc;
    new -> data = '\0';
    insertToDataList(new);
    return 1;
}

/*return 1 if struct is legal and was successfully added to data table, 0 otherwise*/
int addStruct(char *p)
{
    char copy[80]; /*store the string in an array so we can use strtok func*/
    strcpy(copy, p);
    int flag = 1;
    char *bufNum;    /*buffer for the .data part of struct*/
    char *bufString;    /*buffer for the .string part of struct*/
    bufNum = strtok(copy,",");
    if ((flag = addNumber(bufNum)) == 0)    /*if the .data part fails - return 0*/
        return 0;
    bufString = strtok(NULL, "\n");
	if(*bufString == ' ')
                            {
                                bufString += skipSpaces(bufString, 0);
                            }
                            if (*bufString != '\"')
                            {
                                errorExists = TRUE;
                            }
                            else
                            {
                                bufString++;
                            }
    if ((flag = addString(bufString)) == 0)    /*if the .data part fails - return 0*/
        return 0;
    return 1;
}

/* check if a command is legal according to operands rules in page 29 */
int isCommandLegal(char* cmd, char *parameters) {
    int errorExists = 0;
    char buf[100] = {0};
    char *operand;
    char *firstOp;
    char *secondOp;
    int srcOperandAddress = -1;
    int destOperandAddress= -1;
    int at;
    if (parameters)
    {
        strcpy(buf, parameters);
    }
    switch (isCommand(cmd))
    {
        case MOV:
        case ADD:
        case SUB:
            operand = strtok(buf,",");
            firstOp=operand;
            operand = strtok(NULL, "\n");
            secondOp=operand;
            if (!firstOp)
            {
                printf ("\nError: two operands expected\n");
                errorExists = TRUE;
            }
            else{
                srcOperandAddress = getAddressingMethod(firstOp);
               
            }
            if (!secondOp)
            {
                printf ("\nError: two operands expected\n");
                errorExists = TRUE;
            }
            else
            {
                destOperandAddress= getAddressingMethod(secondOp);
               
            }
            operand = strtok(NULL, ",");
            if (operand)
            {
                printf ("\nError: too many operands\n");
                errorExists = TRUE;
            }
            if (!errorExists)
            {
                switch(srcOperandAddress)
                {
                    case IMMEDIATE:
                    case DIRECT:
                        if (destOperandAddress == DIRECT ||destOperandAddress== DIRECTREGISTER)
                        {
                            ic += 2;
                            return TRUE;
                        }
                        else if (destOperandAddress == STRUCTACCESS)
                        {
                            ic += 3;
                            return TRUE;
                        }
                        else
                        {
                            printf ("\nError: destination operand address type doesn't match\n");
                            errorExists = TRUE;
                            return FALSE;
                        }
                        break;
                    case STRUCTACCESS:
                        if (destOperandAddress == DIRECT ||destOperandAddress== DIRECTREGISTER)
                        {
                            ic += 3;
                            return TRUE;
                        }
                        else if (destOperandAddress == STRUCTACCESS)
                        {
                            ic += 3;
                            return TRUE;
                        }
                        else {
                            printf ("\nError: destination operand address type doesn't match\n");
                            errorExists = TRUE;
                            return FALSE;
                        }
                        break;
                    case DIRECTREGISTER:
                        if (destOperandAddress == DIRECT) {
                            ic += 2;
                            return TRUE;
                        }
                        else if (destOperandAddress == STRUCTACCESS) {
                            ic += 3;
                            return TRUE;
                        }
                        else if (destOperandAddress == DIRECTREGISTER) {
                            ic += 1;
                            return TRUE;
                        }
                        else {
                            printf ("\nError: destination operand address type is not legal\n");
                            errorExists = TRUE;
                            return FALSE;
                        }
                        break;
                    case ADDRESSINGERROR:
                        printf ("\nError: source operand address type doesn't match\n");
                        errorExists = TRUE;
                        return FALSE;
                }
            }
            else
            {
                printf ("\nnumber of operands is not legal\n");
                errorExists = TRUE;
                return FALSE;
            }
            return TRUE;
            break;
        case CMP:
            operand = strtok(buf,",");
            firstOp=operand;
            operand = strtok(NULL, "\n");
            secondOp=operand;
            if (!firstOp)
            {
                printf ("\nError: two operands expected\n");
                errorExists = TRUE;
            }
            else{
                srcOperandAddress = getAddressingMethod(firstOp);
            }
            if (!secondOp)
            {
                printf ("\nError: two operands expected\n");
                errorExists = TRUE;
            }
            else
            {
                destOperandAddress= getAddressingMethod(secondOp);
            }
            operand = strtok(NULL, ",");
            if (operand)
            {
                printf ("\nError: too many operands\n");
                errorExists = TRUE;
            }
            if (!errorExists)
            {
                switch(srcOperandAddress)
                {
                    case IMMEDIATE:
                    case DIRECT:
                        if (destOperandAddress == IMMEDIATE ||destOperandAddress== DIRECT ||destOperandAddress== DIRECTREGISTER)
                        {
                            ic += 2;
                            return TRUE;
                        }
                        else if (destOperandAddress == STRUCTACCESS)
                        {
                            ic += 4;
                            return TRUE;
                        }
                        else
                        {
                            printf ("\nError: destination operand is not legal\n");
                            errorExists = TRUE;
                            return FALSE;
                        }
                        break;
                    case STRUCTACCESS:
                        if (destOperandAddress == IMMEDIATE ||destOperandAddress== DIRECT ||destOperandAddress== DIRECTREGISTER)
                        {
                            ic += 3;
                            return TRUE;
                        }
                        else if (destOperandAddress == STRUCTACCESS)
                        {
                            ic += 4;
                            return TRUE;
                        }
                        else {
                            printf ("\nError: destination operand is not legal\n");
                            errorExists = TRUE;
                            return FALSE;
                        }
                        break;
                    case DIRECTREGISTER:
                        if (destOperandAddress == IMMEDIATE ||destOperandAddress== DIRECT)
                        {
                            ic += 2;
                            return TRUE;
                        }
                        else if (destOperandAddress == STRUCTACCESS) {
                            ic += 3;
                            return TRUE;
                        }
                        else if (destOperandAddress == DIRECTREGISTER) {
                            ic += 1;
                            return TRUE;
                        }
                        else {
                            printf ("Error: destination operand is not legal\n");
                            errorExists = TRUE;
                            return 0;
                        }
                        break;
                    case ADDRESSINGERROR:
                        printf ("\nError: destination operand is not legal\n");
                        errorExists = TRUE;
                        return FALSE;
                        break;
                }
            }
            else {
                printf ("\nnumber of operands is not legal\n");
                errorExists = TRUE;
                return FALSE;
            }
            return TRUE;
            break;
        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case RED:
        case JSR:
            operand = strtok(buf, " ,\n");
            if (!operand)
            {
                printf ("\nError: one operand expected\n");
                errorExists = TRUE;
            }
            else
            {
                destOperandAddress= getAddressingMethod(operand);
		
            }
            operand = strtok(NULL, "\n");
            if (operand)
            {
                printf ("\nError: too many operands\n");
                errorExists = TRUE;
            }
            if (!errorExists)
            {
                switch(destOperandAddress)
                {
                    case DIRECT:
                    case DIRECTREGISTER:
                        ic += 1;
                        return TRUE;
                        break;
                    case STRUCTACCESS:
                        ic += 2;
                        return TRUE;
                        break;
                    default :
                        printf ("\nError: destination operand is not legal\n");
                        errorExists = TRUE;
                        return FALSE;
                        break;
                }
            }
            else
            {
                printf ("\nError: number of operand is not legal\n");
                errorExists = TRUE;
                return FALSE;
            }
            return TRUE;
            break;
        case LEA:
            operand = strtok(buf,",");
            firstOp=operand;
            operand = strtok(NULL, "\n");
            secondOp=operand;
            if (!firstOp)
            {
                printf ("\nError: two operands expected\n");
                errorExists = TRUE;
            }
            else{
                srcOperandAddress = getAddressingMethod(firstOp);
                
            }
            if (!secondOp)
            {
                printf ("\nError: two operands expected\n");
                errorExists = TRUE;
            }
            else
            {
                destOperandAddress= getAddressingMethod(secondOp);
                
            }
            operand = strtok(NULL, " ,");
            if (operand)
            {
                printf ("\nError: too many operands\n");
                errorExists = 1;
            }
            if (!errorExists)
            {
                switch(srcOperandAddress)
                {
                    case IMMEDIATE:
                        if (destOperandAddress == 1 ||destOperandAddress== 3) {
                            ic += 2;
                            return 1;
                        }
                        else if (destOperandAddress == 2) {
                            ic += 3;
                            return 1;
                        }
                        else {
                            printf ("\nError: destination operand is not legal\n");
                            errorExists = 1;
                            return 0;
                        }
                        break;
                    case STRUCTACCESS:
                        if (destOperandAddress == 1 ||destOperandAddress== 3) {
                            ic += 3;
                            return 1;
                        }
                        else if (destOperandAddress == 2) {
                            ic += 4;
                            return 1;
                        }
                        else {
                            printf ("\nError: destination operand is not legal\n");
                            errorExists = 1;
                            return 0;
                        }
                        break;
                    default:
                        printf ("\nError: source operand is not legal\n");
                        errorExists = 1;
                        return 0;
                        break;
                }
            }
            else
            {
                printf ("\nError: number of operands is not legal\n");
                errorExists = 1;
                return 0;
            }
            return 1;
            break;
        case PRN:
            operand = strtok(buf, " \n");
            if (!operand)
            {
                printf ("\nError: one operand expected\n");
                errorExists = 1;
            }
            if (operand)
            {
                destOperandAddress= getAddressingMethod(operand);
            }
            operand = strtok(NULL, " ,\n");
            if (operand) {
                printf ("\nError: teo many operands\n");
                errorExists = 1;
            }
            if (!errorExists)
            {
                switch(destOperandAddress)
                {
                    case IMMEDIATE:
                    case DIRECT:
                    case DIRECTREGISTER:
                        ic += 1;
                        return 1;
                        break;
                    case STRUCTACCESS:
                        ic += 2;
                        return TRUE;
                        break;
                    default:
                        printf ("\nError: destination operand is not legal\n");
                        errorExists = TRUE;
                        return FALSE;
                        break;
                }
            }
            else
            {
                printf ("\nError: number of operands is not legal\n");
                errorExists = TRUE;
                return FALSE;
            }
            return TRUE;
            break;
        case RTS:
        case STOP:
            operand = strtok(buf, " ,\n");
            if (operand)
            {
                printf ("\nError: no operands expected\n");
                errorExists = 1;
                return 0;
            }
            else
            {
                return 1;
            }
            break;
        case NONCOMMAND:
            printf("\nError: command is not valid\n");
            errorExists = 1;
            return 0;
            break;
        default:
        {
            errorExists = 1;
            return 0;
        }
            break;
    }
}
/* get the address type number*/
int getAddressingMethod (char* x) {
    int addressingType;
    int i = 0;
    if (x[i]==' ')
    {
        
        i = skipSpaces(x, i);
    }
    
    while (x[i])
    {
        if (x[i] == '#')
        {
            i++;
            if (isNumber(x, i))
            {
                addressingType = IMMEDIATE;
            }
            else
            {
                addressingType = ADDRESSINGERROR;
            }
        }
        else if (isRegister(x))
        {
            addressingType = DIRECTREGISTER;
        }
        else if ( isalnum(x[i]) )
        {
            if(isStruct(x, i))
            {
                addressingType=STRUCTACCESS;
            }
            else if (isVariable(x,i))
            {
                addressingType=DIRECT;
            }
            else
            {
                return ADDRESSINGERROR;
            }
        }
        else
        {
            return ADDRESSINGERROR;
        }
        return addressingType;
    }
    return ADDRESSINGERROR;
}


/* first pass: enter labels to symbo table, all data to data table. */
void first_pass(char * fName) {
    struct symbol* new_node = NULL;
    int command = -1;
    char *p1 = NULL; /* stores first two words in each line (label or command) */
    char *p2 = NULL; /* stores parameters after a command was found */
    char buf[MAX_BUFFER];    /* 100  */
    char c[MAX_LABEL];  /* 30  */
    int lineNum = 1;
    int directiveType = 0;
    int length = 0;
    int firstChar; /* if the first character is ';' or '\n'  */
    FILE *fIn = NULL;
    fIn = fopen(fName, "r");
    if(!fIn)
    {
        printf("\nError occured while openig file\n");  /*error*/
        exit(1);
    }
    while (!feof(fIn))
    {
        if (fgets(buf, MAX_COMMAND_LINE, fIn) == NULL)
        { /*get a command line*/
            lineNum++;
            continue;
        }
        firstChar = skipSpaces(buf, 0); /* skip to the first character */
        if (buf[firstChar] == ';' ) /*if it is a comment*/
        {
            lineNum++;
            continue;
        }
        if( buf[firstChar] == '\n')  /*if it is an empty line*/
        {
            lineNum++;
            continue;
        }
        p1 = strtok(buf, " "); /* first word in  buffer */
        /* is it a label */
        
        length=strlen(p1);
        if(p1[length - 1] == ':')
        {
            if (isValidLabel(p1)) /*check that the label is valid*/
            {
                
                if(!isLabelExists(p1)) /*check that the label does not already exists in the symbol list*/
                {
                    length = strlen(p1);
                    strncpy(c, p1, length - 1); /* copy the label name */
                    c[length - 1] = '\0';
                    new_node = (struct symbol*)malloc(sizeof(symbolTable));
                    strcpy(new_node -> name, c); /* enter label name to the node */
                    new_node -> address = ic; /* enter current address to the node */
                    length = strlen(p1);
                    p1 = strtok(NULL, " \n"); /* command after label */
                    
                    length=strlen(p1);
                    if (isCommand(p1))
                    {
                        
                        p2 = strtok(NULL, "\n");         /* p2 stores the rest of the parameters   */
                        
                        if (isCommandLegal(p1, p2))  /*check that the command is legal*/
                        {
                            
                            new_node -> isExtern = FALSE;
                            insertToSymbolList(new_node);  /*insert a new node to the symbol table */
                            ic++;
                        }
                    }
                    else if ((directiveType = isDirectiveWord(p1))!= NONDIRECTIVE)
                    {
                        
                        new_node -> address = dc;
                        new_node -> isCommand = FALSE;
                        new_node -> isExtern = FALSE;
                        insertToSymbolList(new_node);
                        if (directiveType == DATA)
                        {
                            p1 = strtok(NULL, ",");
                            while (p1) {
                                if (addNumber(p1))
                                {  /*insert a new node to the data table */
                                    p1 = strtok(NULL, ",");
                                }
                                else {
                                    printf("\nError: line: %d parameters %s are not valid\n", lineNum, p1);
                                    p1 = strtok(NULL, ",");
                                }
                            }
                        }
                        else if (directiveType == STRING)
                        {
                            p1 = strtok(NULL, "\n");
                            if(*p1 == ' ')
                            {
                                p1 += skipSpaces(p1, 0);
                            }
                            if (*p1 != '\"')
                            {
                                errorExists = TRUE;
                            }
                            else
                            {
                                p1++;
                            }
                            if (addString(p1))
                            { /* add a new data to the data table */ }
                            else
                            {                                errorExists = TRUE;
                                printf("\nError: line: %d prameters %s are not valid\n", lineNum, p1);
                            }
                        }
                        /* a .struct directive */
                        else if (directiveType == STRUCT)
                        {
                            p1 = strtok(NULL,"\n");
                            if(*p1 == ' ')
                            {
                                p1 += skipSpaces(p1, 0);        /*skip spaces if needed*/
                            }
                            if (addStruct(p1))
                            {
                              
                            }    /* 0 == failure*/
                            else {
                                printf("\nError: line: %d the word %s is not legal\n", lineNum, p1);
                            }
                        }
                        else if (directiveType == ENTRY) {
                            /*no action needed for now */
                            continue;
                        }
                        else if (directiveType == EXTERN) {
                            p1 = strtok(NULL, ", ");
                            if (!p1) {
                                printf("\nError: label expected after '.extern' line: %d!\n", lineNum);
                                errorExists = 1;
                            }
                            while (p1) {
                                length = strlen(p1);
                                if (p1[length - 1] == '\n') { p1[length - 1] = '\0'; } /* remove \n characters in the end */
                                if (isValidLabel(p1)) {
					
                                    new_node = (struct symbol*)malloc(sizeof(symbolTable));
                                    strcpy(new_node -> name, p1);
                                    new_node -> address = 0;  /* temp address */
                                    new_node -> isExtern = TRUE;
                                    new_node -> isCommand = FALSE;
                                    insertToSymbolList(new_node);
                                    p1 = strtok(NULL, ", ");
                                }
                                else {
                                    printf("\nError: invalid label after .extern directive line: %d!\n", lineNum);
                                    errorExists = TRUE;
                                    p1 = strtok(NULL, ", ");
                                }
                            }
                        }
                    }
                    else {
                        printf("\nError: line: %d , word %s is not legal\n", lineNum, p1);
                    }
                }
                else
                {
                    printf("\nError: line: %d ,label %s already exists in symbol table \n", lineNum, c);
                }
            }
            else
            {
                printf("\nError: line: %d, the label %s is not legal \n", lineNum, c);
            }
        }
        else if (isCommand(p1))
        {
            ic++;
            p2 = strtok(NULL, "\n");
            isCommandLegal(p1, p2);  /*check that the command is legal and update the ic accordingly*/
            
        }
        else if ((directiveType = isDirectiveWord(p1)) != NONDIRECTIVE)
        {
		
            if (directiveType == DATA)
            {
                p1 = strtok(NULL,",");
                while (p1)
                {
                    if(addNumber(p1)) /* add new data into the data table */
                        p1=strtok(NULL,",");
                    else{
                        printf("\nError: line: %d , the prameters %s are not valid\n", lineNum, p1);
                        p1 = strtok(NULL, ",");
                    }
                }
            }
            else if (directiveType == STRING)
            {
                p1 = strtok(NULL, "\n");
                if(*p1 == ' ')
                    p1 += skipSpaces(p1, 0);
                if (*p1 != '\"'){
                    errorExists = TRUE;
                }
                else
                    p1++;
                if (addString(p1)) { /* add new data into the data table */ }
                else{
                    errorExists = 1;
                    printf("\nError: line: %d , the prameters %s are not valid\n", lineNum, p1);
                }
            }
            else if (directiveType == STRUCT)
            {
                p1 = strtok(NULL, "\n");
                if (*p1 == ' ')
                    p1 += skipSpaces(p1, 0);
                if (addStruct(p1)) {

                }    /*0 == failure*/
                else {
                    printf("\nError: line: %d, the word %s is not legal\n", lineNum, p1);
                }
            }
            else if (directiveType == ENTRY) {
                lineNum++;
                /* no action is needed for now */
                continue;
            }
            else if (directiveType == EXTERN) {
                p1 = strtok(NULL, ", ");
		
                if (!p1) { printf("\nError: label expected after '.extern' in line: %d!\n", lineNum); }
                while (p1) {
                    length = strlen(p1);
                    if (p1[length - 1] == '\n') { p1[length - 1] = '\0'; }
                    if(isValidLabel(p1)) {

                        new_node = (struct symbol*)malloc(sizeof(symbolTable));

                        strcpy(new_node -> name, p1);
                        new_node -> address = 0;
                        new_node -> isExtern = TRUE;
                        new_node -> isCommand = FALSE;
                        insertToSymbolList(new_node);
                        p1 = strtok(NULL, ", ");
                    }
                    else {
                        printf ("\nError: invalid label after '.extern' in line: %d!\n", lineNum);
                        errorExists = TRUE;
                        p1 = strtok(NULL, ", ");
                    }
                }
            }
        }
        else { /* if the word is not a directive, command or label*/
            printf("\nError: line: %d, '%s' is not a legal word. \n", lineNum, p1);
        }
        lineNum++; /* update line number */
    }
    fclose(fIn);
}
