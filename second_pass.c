/*Tal Chaklai 200854982*/
/*Boris Manashirov 310511530*/
/*group: 41*/

#include "assembler.h"
/* Update ic at the end of first pass */
char Array32Base[32] = {'!','@','#','$','%','^','&','*','<','>','a','b','c','d','e',
    'f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v'};
void reverse_string(char *s);
void updateICAddress (void) {
    struct symbol* symb = NULL;
    symb = headSymbol;
    int last;
    last = 0;
    while (symb) {
        if (symb -> isExtern == 0 && symb -> isCommand == 0) {
            ic += ((symb -> address) - last);
            last = symb -> address;
            symb -> address = ic;
        }
        symb = symb -> next;
    }
}
/* getr the number in the string */
int findInt(char *a) {
    int x = 0, neg = 0;
    while (*a != '#') { /* reach the '#' symbol */
        a++;
    }
    a++; /* get the first character after the '#' */
    if (*a == ' ') { a += skipSpaces(a, 0); } /* skip spaces if needed */
    if (*a == '-') {
        neg = 1;
        a++; /* skip a minus sign if needed */
    }
    x = atoi(a);
    if (neg) { x *= -1; }
    return x;
}
char * decimalTo32base(int n){

    int length;
	
    int r = 0, i = 0;
	
    if(n<0)     /*if the number is negative, add 1024 (2 complement)*/
	n=n+1024;

    char temp_string[6] = {0};
    do {
        r = n % 32;
        temp_string[i] = Array32Base[r];
        n = n / 32;
        i++;
    }
    while (n > 0);
   	
    if(i==1)
	temp_string[1]='!';
    strcpy(str32base, temp_string); /* copy the received string into the 32 base string to store the the final result  */
    		
    reverse_string(str32base);      /* and reverse the order of the letters, so they will appear in the right form */
    if(str32base)
    return str32base;
}
/*helper function for 32 base*/
/*it will merge the memory word into an integer*/
int bitsToNum(memoryWord word, int memoryType){
    if (memoryType == 1){ /*command*/
        int x, y, z, res;
        x = word.Bits.commandBits.command * (pow(2,6)); /*its pow so it'll be obvious that '6' represents the number of bits left to complete a 10 bit memWord*/
        y = word.Bits.commandBits.sourceAddr * (pow(2,4));
        z = word.Bits.commandBits.destAddr * (4);
        res = x + z + y + word.era;
        return res;
    }
    if (memoryType == 2){ /*register*/
        int x, y, res;
        x = word.Bits.regBits.srcBits * (pow(2,6));
        y = word.Bits.regBits.destBits * (4);
        res = x + y + word.era;
        return res;
    }
    if (memoryType == 3){ /*value*/
        if (word.Bits.value < 0){
            unsigned int x;
            unsigned int res;
            x = word.Bits.value;
            res = (x*4) + word.era;
            return res;
        }
        else {
            int x, res;
            x = word.Bits.value * (4);
            res = x + word.era;
            return res;
        }
    return 0;
    }
}
/* reverse a string */
void reverse_string(char *s) {
    if (s == 0) { return; }
    if (*s == 0) { return; }
    /* get range */
    char *start;
    char *end;
    char x;
    start = s;
    end  = start + strlen(s) - 1;
    while (end > start) {
        x = *start;
        *start = *end;
        *end = x;
        /* move */
        ++start;
        --end;
    }
}
/* check if the symbol is external*/
int isSymbolExternal(char *label) {
    struct symbol* curr = NULL;
    for (curr = headSymbol; curr != NULL; curr = curr -> next) {
        if (!strcmp(label, curr -> name)) { return curr -> isExtern; }
    }
    return 0;
}
/* check if address exists, if it is return the address, otherwise return -1 */
int getAddress(char *ad) {
    struct symbol* p = NULL;
    for (p = headSymbol; p != NULL; p = p -> next) {
        if (!strcmp(ad, p -> name)) { return p -> address; }
    }
    return -1;
}
/* insert data into the code table */
void second_pass (char* fName) {
    char copy[MAX_LABEL]; /*  30  */
    char *p = NULL;
    int directiveType;
    int command;
    int tempAddress;
    char buf[MAX_BUFFER]; /* 100  */
    int i = 0;
    int length;
    int lineNumber = 1;
    int operand1AddressingMethod = -1;
    int operand2AddressingMethod = -1;
    struct codes* firstLine = NULL;
    struct codes* secondLine = NULL;
    struct codes* thirdLine = NULL;
    struct codes* fourthLine = NULL;  /*  max four lines */
    struct externs* externsList = NULL;
    struct entries* entriesList = NULL;
    FILE *file = NULL;
    ic = 100; /* initialize ic */
    file = fopen(fName, "r");
    if (!file) {
        printf("\nError occured while openning file.\n");
        exit(1);
    }
    while (!feof(file)) {
        if (fgets(buf, MAX_COMMAND_LINE, file) == NULL) /* get command line, max 80 chars */
            continue;                               /* if line is empty, skip to the next line       */
        p = strtok(buf, " "); /* store first word in line */
        if(strcmp(p, "\n")==0)
            continue;
        if(p[0]==';')
            continue;
        
        length=strlen(p);
        if(p[length - 1] == ':')
        {
            
            if (isValidLabel(p))
            { /* check if the label exsists in the symbol list */
                i = strlen(p);
                strncpy(copy, p, i - 1); /* copy the label name */
                copy[i - 1] = '\0';
                p = strtok(NULL, " \n"); /* next word */
                
                if (command = isCommand(p))
                { /*  if the word is a command */
                    
                    /* for each group of commands decide how to enter the data into the instruction list and add the correct number of lines */
                    if (command == MOV || command == CMP || command == ADD || command == SUB || command == LEA) { /*  first group of commands */
                        /* create a new line in the code table */
                        firstLine = (struct codes*) malloc(sizeof(codeTable));
                        firstLine -> address = ic;
                        firstLine -> word.era = 0;
                        firstLine -> word.Bits.commandBits.command = command - 1;
                        firstLine -> memoryType = 1;
                        newLine(firstLine);
                        p = strtok(NULL, ",");
                        operand1AddressingMethod = getAddressingMethod(p); /*get first operand addresing method*/
                        if (operand1AddressingMethod == IMMEDIATE)                    {
                            firstLine -> word.Bits.commandBits.sourceAddr = 0;
                            secondLine = (struct codes*)malloc(sizeof(codeTable));
                            secondLine -> address = ic;
                            secondLine -> word.era = 0;
                            secondLine -> word.Bits.value = findInt(p);
                            secondLine -> memoryType = 3;
                            newLine(secondLine);
                            p = strtok(NULL," \n");
                            operand2AddressingMethod = getAddressingMethod(p); /*get second operand addresing method*/
                            if (operand2AddressingMethod == IMMEDIATE) {
                                firstLine -> word.Bits.commandBits.destAddr = 0;
                                thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                thirdLine -> address = ic;
                                thirdLine -> word.era = 0;
                                thirdLine -> word.Bits.value = findInt(p);
                                thirdLine -> memoryType = 3;
                                newLine(thirdLine);
                            }
                            else if (operand2AddressingMethod == DIRECT) {
                                if (*p == ' ')
                                    p += skipSpaces(p, 0);
                                i = 0;
                                while (*p != ' ') {
                                    copy[i] = *p;
                                    i++;
                                    p++;
                                }
                                copy[i] = '\0';
                                if (getAddress(copy) != -1) {
                                    /*if the label is external, add it to the extern table*/
                                    if (isSymbolExternal(copy)) {
                                        
                                        externsList = (struct externs*)malloc(sizeof(externTable));
                                        strcpy(externsList -> name, copy);
                                        externsList -> address = ic;
                                        insertToExternList(externsList);
                                        
                                    }
                                    firstLine -> word.Bits.commandBits.destAddr = 1;
                                    thirdLine = (struct codes*) malloc(sizeof(codeTable));
                                    thirdLine -> address = ic;
                                    if (isSymbolExternal(copy)) {
                                              thirdLine -> word.era = 1; }
                                    else                  { thirdLine -> word.era = 2; }
                                    thirdLine -> word.Bits.value = getAddress(copy);
                                    thirdLine -> memoryType = 3;
                                    newLine(thirdLine);
                                }
                                else {
                                    printf("\nError: variable '%s' not found\n", copy);
                                    errorExists = 1;
                                }
                            }
                            else if (operand2AddressingMethod == STRUCTACCESS) {
                                if (*p == ' ')
                                    p += skipSpaces(p, 0);
                                i = 0;
                                while (*p != '.') {
                                    copy[i] = *p;
                                    i++;
                                    p++;
                                }
                                copy[i] = '\0';
                                if (isLabelExists(copy)) {
                                    firstLine -> word.Bits.commandBits.destAddr = 2;
                                    /* create a new line for the struct */
                                    thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                    thirdLine -> address = ic;
                                    thirdLine -> word.era = 2;
                                    thirdLine -> word.Bits.value = getAddress(copy);
                                    thirdLine -> memoryType = 3;
                                    newLine(thirdLine);
                                    p++;
                                    if (*p == ' ')
                                        p += skipSpaces(p, 0);
                                    fourthLine = (struct codes*)malloc(sizeof(codeTable));
                                    fourthLine -> address = ic;
                                    fourthLine -> word.era = 0;
                                    fourthLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                                    fourthLine -> memoryType = 3;
                                    newLine(fourthLine);
                                }
                                else {
                                    printf("\nvariable '%s' not found\n", copy);
                                    errorExists = 1;
                                }
                            }
                            else if (operand2AddressingMethod == DIRECTREGISTER) {
                                while(*p < '0' || *p > '7') {
                                    p++;
                                }
                                firstLine -> word.Bits.commandBits.destAddr = 3;
                                /* create a new instruction line for a register */
                                thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                thirdLine -> address = ic;
                                thirdLine -> word.era = 2;
                                thirdLine -> word.Bits.regBits.srcBits = 0;
                                thirdLine -> word.Bits.regBits.destBits = *p;
                                thirdLine -> memoryType = 2;
                                /* add the line to the insruction linked list */
                                newLine(thirdLine);
                            }
                        }
                        else if (operand1AddressingMethod  == DIRECT) {
                            if (*p == ' ')
                                p += skipSpaces(p, 0);
                            i = 0;
                            while (*p != ' ') {
                                copy[i] = *p;
                                i++;
                                p++;
                            }
                            copy[i] = '\0';
                            if (getAddress(copy) != -1) {
                                if (isSymbolExternal(copy)) {
                                    
                                    externsList = (struct externs*) malloc(sizeof(externTable));
                                    strcpy(externsList->name, copy);
                                    externsList->address = ic;
                                    insertToExternList(externsList);
                                    
                                }
                                firstLine -> word.Bits.commandBits.sourceAddr = 1;
                                secondLine = (struct codes*) malloc(sizeof(codeTable));
                                secondLine -> address = ic;
                                if (isSymbolExternal(copy)) {
                                    
                                    secondLine -> word.era = 1; }
                                else                  { secondLine -> word.era = 2; }
                                secondLine -> word.Bits.value = getAddress(copy);
                                secondLine -> memoryType = 3;
                                newLine(secondLine);
                            }
                            else {
                                printf("\nvariable '%s' not found\n", copy);
                                errorExists = 1;
                            }
                            p=strtok(NULL, " \n");
                            operand2AddressingMethod = getAddressingMethod(p);
                            if (operand2AddressingMethod == IMMEDIATE) {
                                firstLine -> word.Bits.commandBits.destAddr = 0;
                                thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                thirdLine -> address = ic;
                                thirdLine -> word.era = 0;
                                thirdLine -> word.Bits.value = findInt(p);
                                thirdLine -> memoryType = 3;
                                newLine(thirdLine);
                            }
                            else if (operand2AddressingMethod == DIRECT) {
                                if (*p == ' ')
                                    p += skipSpaces(p, 0);
                                i = 0;
                                while (*p != ' ') {
                                    copy[i] = *p;
                                    i++;
                                    p++;
                                }
                                copy[i] = '\0';
                                if (getAddress(copy) != -1) {
                                    if (isSymbolExternal(copy)) {
                                        
                                        externsList = (struct externs*) malloc(sizeof(externTable));
                                        strcpy(externsList->name, copy);
                                        externsList -> address = ic;
                                        insertToExternList(externsList);
                                        
                                    }
                                    firstLine -> word.Bits.commandBits.destAddr = 1;
                                    thirdLine = (struct codes*) malloc(sizeof(codeTable));
                                    thirdLine -> address = ic;
                                    if (isSymbolExternal(copy))
                                    {
                                        
                                        thirdLine -> word.era = 1; }
                                    else                  { thirdLine -> word.era = 2; }
                                    thirdLine -> word.Bits.value = getAddress(copy);
                                    thirdLine -> memoryType = 3;
                                    newLine(thirdLine);
                                }
                                else {
                                    printf("\nvariable '%s' not found\n", copy);
                                    errorExists = 1;
                                }
                            }
                            else if (operand2AddressingMethod == STRUCTACCESS) {
                                if (*p == ' ')
                                    p += skipSpaces(p, 0);
                                i = 0;
                                while (*p != '.') {
                                    copy[i] = *p;
                                    i++;
                                    p++;
                                }
                                copy[i] = '\0';
                                if(isLabelExists(copy)) {
                                    firstLine -> word.Bits.commandBits.destAddr=2;
                                    thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                    thirdLine -> address = ic;
                                    thirdLine -> word.era = 2;
                                    thirdLine -> word.Bits.value = getAddress(copy);
                                    thirdLine -> memoryType = 3;
                                    newLine(thirdLine);
                                    p++;
                                    if (*p == ' ')
                                        p += skipSpaces(p,0);
                                    fourthLine = (struct codes*)malloc(sizeof(codeTable));
                                    fourthLine -> address = ic;
                                    fourthLine -> word.era = 0;
                                    fourthLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                                    fourthLine -> memoryType = 3;
                                    newLine(fourthLine);
                                }
                                else {
                                    printf("\nvariable '%s' not found\n",copy);
                                    errorExists = 1;
                                }
                            }
                            else if (operand2AddressingMethod == DIRECTREGISTER) {
                                while(*p < '0' || *p > '7') {
                                    p++;
                                }
                                firstLine -> word.Bits.commandBits.destAddr = 3;
                                thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                thirdLine -> address = ic;
                                thirdLine -> word.era = 2;
                                thirdLine -> word.Bits.regBits.srcBits = 0;
                                thirdLine -> word.Bits.regBits.destBits = *p;
                                thirdLine -> memoryType = 2;
                                newLine(thirdLine);
                            }
                        }
                        else if(operand1AddressingMethod == 2) {
                            if (*p == ' ')
                                p += skipSpaces(p, 0);
                            i = 0;
                            while (*p != '.') {
                                copy[i] = *p;
                                i++;
                                p++;
                            }
                            copy[i] = '\0';
                            if (isLabelExists(copy)) {
                                firstLine -> word.Bits.commandBits.sourceAddr = 2;
                                secondLine = (struct codes*)malloc(sizeof(codeTable));
                                secondLine -> address = ic;
                                secondLine -> word.era = 2;
                                secondLine -> word.Bits.value = getAddress(copy);
                                secondLine -> memoryType = 3;
                                newLine(secondLine);
                                p++;
                                if (*p == ' ')
                                    p += skipSpaces(p, 0);
                                thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                thirdLine -> address = ic;
                                thirdLine -> word.era = 0;
                                thirdLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                                thirdLine -> memoryType = 3;
                                newLine(thirdLine);
                            }
                            p = strtok(NULL, " \n");
                            operand2AddressingMethod = getAddressingMethod(p);
                            if (operand2AddressingMethod == IMMEDIATE) {
                                firstLine -> word.Bits.commandBits.destAddr = 0;
                                thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                thirdLine -> address = ic;
                                thirdLine -> word.era = 0;
                                thirdLine -> word.Bits.value = findInt(p);
                                thirdLine -> memoryType = 3;
                                newLine(thirdLine);
                            }
                            else if (operand2AddressingMethod == DIRECT) {
                                if (*p == ' ')
                                    p += skipSpaces(p, 0);
                                i = 0;
                                while (*p != ' ' && *p != '\n' && *p != '\0') {
                                    copy[i] = *p;
                                    i++;
                                    p++;
                                }
                                copy[i] = '\0';
                                if (getAddress(copy) != -1) {
                                    if (isSymbolExternal(copy)) {
                                        
                                        externsList = (struct externs*) malloc(sizeof(externTable));
                                        strcpy(externsList->name, copy);
                                        externsList -> address = ic;
                                        insertToExternList(externsList);
                                        
                                    }
                                    firstLine -> word.Bits.commandBits.destAddr = 1;
                                    thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                    thirdLine -> address = ic;
                                    if (isSymbolExternal(copy)) {
                                        
                                        thirdLine -> word.era = 1; }
                                    else                { thirdLine -> word.era = 2; }
                                    thirdLine -> word.Bits.value = getAddress(copy);
                                    thirdLine -> memoryType = 3;
                                    newLine(thirdLine);
                                }
                                else {
                                    printf("\nvariable '%s' not found\n", copy);
                                    errorExists = 1;
                                }
                            }
                            else if (operand2AddressingMethod == STRUCTACCESS) {
                                if (*p == ' ')
                                    p += skipSpaces(p, 0);
                                i = 0;
                                while (*p != '.') {
                                    copy[i] = *p;
                                    i++;
                                    p++;
                                }
                                copy[i] = '\0';
                                if (isLabelExists(copy)) {
                                    firstLine -> word.Bits.commandBits.destAddr = 2;
                                    thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                    thirdLine -> address = ic;
                                    thirdLine -> word.era = 2;
                                    thirdLine -> word.Bits.value = getAddress(copy);
                                    thirdLine -> memoryType = 3;
                                    newLine(thirdLine);
                                    p++;
                                    if (*p == ' ')
                                        p += skipSpaces(p,0);
                                    fourthLine = (struct codes*)malloc(sizeof(codeTable));
                                    fourthLine -> address = ic;
                                    fourthLine -> word.era = 0;
                                    fourthLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                                    fourthLine -> memoryType = 3;
                                    newLine(fourthLine);
                                }
                                else {
                                    printf("\nvariable '%s' not found\n", copy);
                                    errorExists = 1;
                                }
                            }
                            else if (operand2AddressingMethod == DIRECTREGISTER) {
                                while(*p < '0' || *p > '7') {
                                    p++;
                                }
                                firstLine -> word.Bits.commandBits.destAddr = 3;
                                thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                thirdLine -> address = ic;
                                thirdLine -> word.era = 2;
                                thirdLine -> word.Bits.regBits.srcBits = 0;
                                thirdLine -> word.Bits.regBits.destBits = *p;
                                thirdLine -> memoryType = 2;
                                newLine(thirdLine);
                            }
                        }
                        else if (operand1AddressingMethod == DIRECTREGISTER) {
                            while(*p < '0' || *p > '7') {
                                p++;
                            }
                            firstLine -> word.Bits.commandBits.sourceAddr=3;
                            secondLine = (struct codes*)malloc(sizeof(codeTable));
                            secondLine -> address = ic;
                            secondLine -> word.era = 2;
                            secondLine -> word.Bits.regBits.srcBits = *p;
                            secondLine -> memoryType = 2;
                            newLine(secondLine);
                            p = strtok(NULL, " \n");
                            operand2AddressingMethod = getAddressingMethod(p);
                            if (operand2AddressingMethod == IMMEDIATE) {
                                firstLine -> word.Bits.commandBits.destAddr = 0;
                                thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                thirdLine -> address = ic;
                                thirdLine -> word.era = 0;
                                thirdLine -> word.Bits.value = findInt(p);
                                thirdLine -> memoryType = 3;
                                newLine(thirdLine);
                            }
                            else if (operand2AddressingMethod == DIRECT)
                                if (*p == ' ')
                                    p += skipSpaces(p, 0);
                            i = 0;
                            while (*p != ' ') {
                                copy[i] = *p;
                                i++;
                                p++;
                            }
                            copy[i] = '\0';
                            if (getAddress(copy) != -1) {
                                if (isSymbolExternal(copy)) {
                                    
                                    externsList = (struct externs*) malloc(sizeof(externTable));
                                    strcpy(externsList->name, copy);
                                    externsList->address = ic;
                                    insertToExternList(externsList);
                                    
                                }
                                firstLine -> word.Bits.commandBits.destAddr = 1;
                                thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                thirdLine -> address = ic;
                                if (isSymbolExternal(copy)) {
                                    
                                    thirdLine -> word.era = 1; }
                                else                { thirdLine -> word.era = 2; }
                                thirdLine -> word.Bits.value = getAddress(copy);
                                thirdLine -> memoryType = 3;
                                newLine(thirdLine);
                            }
                            else {
                                printf("\nvariable '%s' not found\n",copy);
                                errorExists = 1;
                            }
                        }
                        else if (operand2AddressingMethod == STRUCTACCESS) {
                            if (*p == ' ')
                                p += skipSpaces(p, 0);
                            i = 0;
                            while (*p != '.') {
                                copy[i] = *p;
                                i++;
                                p++;
                            }
                            copy[i] = '\0';
                            if (isLabelExists(copy)) {
                                firstLine -> word.Bits.commandBits.destAddr = 2;
                                thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                thirdLine -> address = ic;
                                thirdLine -> word.era = 2;
                                thirdLine -> word.Bits.value = getAddress(copy);
                                thirdLine -> memoryType = 3;
                                newLine(thirdLine);
                                p++;
                                if (*p == ' ')
                                    p += skipSpaces(p,0);
                                fourthLine = (struct codes*)malloc(sizeof(codeTable));
                                fourthLine -> address = ic;
                                fourthLine -> word.era = 0;
                                fourthLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                                fourthLine -> memoryType = 3;
                                newLine(fourthLine);
                            }
                            else {
                                printf("\nvariable '%s' not found\n", copy);
                                errorExists = 1;
                            }
                        }
                        else if (operand2AddressingMethod == DIRECTREGISTER) {
                            while(*p < '0' || *p > '7') {
                                p++;
                            }
                            firstLine -> word.Bits.commandBits.destAddr = 3;
                            firstLine -> word.Bits.commandBits.sourceAddr = 0;
                            secondLine = (struct codes*)malloc(sizeof(codeTable));
                            secondLine -> address = ic;
                            secondLine -> word.era = 2;
                            secondLine -> word.Bits.regBits.srcBits = 0;
                            secondLine -> word.Bits.regBits.destBits = *p;
                            secondLine -> memoryType = 2;
                            newLine(secondLine);
                        }
                    }
                    else if (command >= NOT && command <= JSR) { /*  second group of commands */
                        firstLine = (struct codes*) malloc(sizeof(codeTable));
                        firstLine -> address = ic;
                        firstLine -> word.era = 0;
                        firstLine -> word.Bits.commandBits.command = command - 1;
                        firstLine -> memoryType = 1;
                        newLine(firstLine);
                        p = strtok(NULL, ",");
                        if (p) {
                            i = strlen(p);
                            if (p[i - 1] == '\n') { p[i - 1] = '\0'; }
                            operand2AddressingMethod = getAddressingMethod(p);
                        }
                        if (operand2AddressingMethod == IMMEDIATE) {
                            firstLine -> word.Bits.commandBits.destAddr = 0;
                            firstLine -> word.Bits.commandBits.sourceAddr = 0;
                            secondLine = (struct codes*)malloc(sizeof(codeTable));
                            secondLine -> address = ic;
                            secondLine -> word.era = 0;
                            secondLine -> word.Bits.value = findInt(p);
                            secondLine -> memoryType = 3;
                            newLine(secondLine);
                        }
                        else if (operand2AddressingMethod == DIRECT) {
                            if (*p == ' ')
                                p += skipSpaces(p, 0);
                            i = 0;
                            while (*p != ' ' && *p != '\0') {
                                copy[i] = *p;
                                i++;
                                p++;
                            }
                            copy[i] = '\0';
                            if (getAddress(copy) != -1) {
                                if (isSymbolExternal(copy)) {
                                    
                                    externsList = (struct externs*) malloc(sizeof(externTable));
                                    strcpy(externsList->name, copy);
                                    externsList->address = ic;
                                    insertToExternList(externsList);
                                   
                                }
                                firstLine -> word.Bits.commandBits.sourceAddr = 0;
                                firstLine -> word.Bits.commandBits.destAddr = 1;
                                secondLine = (struct codes*)malloc(sizeof(codeTable));
                                secondLine -> address = ic;
                                if (isSymbolExternal(copy)) {
                                   
                                    secondLine -> word.era = 1; }
                                else                { secondLine -> word.era = 2; }
                                secondLine -> word.Bits.value = getAddress(copy);
                                secondLine -> memoryType = 3;
                                newLine(secondLine);
                            }
                            else {
                                printf("\nvariable '%s' not found\n",copy);
                                errorExists = 1;
                            }
                        }
                        else if (operand2AddressingMethod == STRUCTACCESS) {
                            if (*p == ' ')
                                p += skipSpaces(p, 0);
                            i = 0;
                            while (*p != '.') {
                                copy[i] = *p;
                                i++;
                                p++;
                            }
                            copy[i] = '\0';
                            if(isLabelExists(copy) ) {
                                firstLine -> word.Bits.commandBits.destAddr = 2;
                                firstLine -> word.Bits.commandBits.sourceAddr = 0;
                                secondLine = (struct codes*)malloc(sizeof(codeTable));
                                secondLine -> address = ic;
                                secondLine -> word.era = 2;
                                secondLine -> word.Bits.value = getAddress(copy);
                                secondLine -> memoryType = 3;
                                newLine(secondLine);
                                p++;
                                if (*p == ' ') /* skip spaces if needed    */
                                    p += skipSpaces(p, 0);
                                thirdLine = (struct codes*)malloc(sizeof(codeTable));
                                thirdLine -> address = ic;
                                thirdLine -> word.era = 0;
                                thirdLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                                thirdLine -> memoryType = 3;
                                newLine(thirdLine);
                            }
                            else {
                                printf("\nvariable '%s' not found\n",copy);
                                errorExists = 1;
                            }
                        }
                        else if (operand2AddressingMethod == DIRECTREGISTER) {
                            while(*p < '0' || *p > '7') {
                                p++;
                            }
                            firstLine -> word.Bits.commandBits.destAddr = 3;
                            firstLine -> word.Bits.commandBits.sourceAddr = 0;
                            secondLine = (struct codes*)malloc(sizeof(codeTable));
                            secondLine -> address = ic;
                            secondLine -> word.era = 2;
                            secondLine -> word.Bits.regBits.srcBits = 0;
                            secondLine -> word.Bits.regBits.destBits = *p;
                            secondLine -> memoryType = 2;
                            newLine(secondLine);
                        }
                    }
                    else if (command == RTS || command == STOP) { /*  third group of commands */
                        firstLine = (struct codes*)malloc(sizeof(codeTable));
                        firstLine -> address = ic;
                        firstLine -> word.era = 0;
                        firstLine -> word.Bits.commandBits.command = command - 1;
                        firstLine -> word.Bits.commandBits.sourceAddr = 0;
                        firstLine -> word.Bits.commandBits.destAddr = 0;
                        firstLine -> memoryType = 1;
                        newLine(firstLine);
                    }
                }
            }
            else if (isDirectiveWord(p)) { /* do nothing for now */ }
        }
        else if (command = isCommand(p)) {
            if (command == MOV || command == CMP || command == ADD || command == SUB || command == LEA) { /*  first group of commands */
                firstLine = (struct codes*) malloc(sizeof(codeTable));
                firstLine -> address = ic;
                firstLine -> word.era = 0;
                firstLine -> word.Bits.commandBits.command = command - 1;
                firstLine -> memoryType = 1;
                newLine(firstLine);
                p = strtok(NULL, ",");
                operand1AddressingMethod = getAddressingMethod(p);
                if (operand1AddressingMethod == IMMEDIATE) {
                    firstLine -> word.Bits.commandBits.sourceAddr = 0;
                    secondLine = (struct codes*)malloc(sizeof(codeTable));
                    secondLine -> address = ic;
                    secondLine -> word.era = 0;
                    secondLine -> word.Bits.value = findInt(p);
                    secondLine -> memoryType = 3;
                    newLine(secondLine);
                    p = strtok(NULL," \n");
                    operand2AddressingMethod = getAddressingMethod(p);
                    if (operand2AddressingMethod == IMMEDIATE) {
                        firstLine -> word.Bits.commandBits.destAddr=0;
                        thirdLine = (struct codes*)malloc(sizeof(codeTable));
                        thirdLine -> address=ic;
                        thirdLine -> word.era=0;
                        thirdLine -> word.Bits.value= findInt(p);
                        thirdLine -> memoryType = 3;
                        newLine(thirdLine);
                    }
                    else if (operand2AddressingMethod == DIRECT) {
                        if (*p == ' ')
                            p += skipSpaces(p, 0);
                        i = 0;
                        while (*p != ' ') {
                            copy[i] = *p;
                            i++;
                            p++;
                        }
                        copy[i] = '\0';
                        if (getAddress(copy) != -1) {
                            
                            if (isSymbolExternal(copy)) {
                               
                                externsList = (struct externs*) malloc(sizeof(externTable));
                                strcpy(externsList->name, copy);
                                externsList->address = ic;
                                externsList->address = ic;
                                insertToExternList(externsList);
                                
                            }
                            firstLine -> word.Bits.commandBits.destAddr = 1;
                            thirdLine = (struct codes*) malloc(sizeof(codeTable));
                            thirdLine -> address = ic;
                            thirdLine -> word.era =2;
                            thirdLine -> word.Bits.value = getAddress(copy);
                            thirdLine -> memoryType = 3;
                            newLine(thirdLine);
                        }
                        else {
                            printf("\nvariable '%s' not found\n",copy);
                            errorExists = 1;
                        }
                    }
                    else if (operand2AddressingMethod == STRUCTACCESS) {
                        if (*p == ' ')
                            p += skipSpaces(p, 0);
                        i = 0;
                        while (*p != '.') {
                            copy[i] = *p;
                            i++;
                            p++;
                        }
                        copy[i] = '\0';
                        if (isLabelExists(copy) ) {
                            firstLine -> word.Bits.commandBits.destAddr = 2;
                            thirdLine = (struct codes*)malloc(sizeof(codeTable));
                            thirdLine -> address = ic;
                            thirdLine -> word.era = 2;
                            thirdLine -> word.Bits.value = getAddress(copy);
                            thirdLine -> memoryType = 3;
                            newLine(thirdLine);
                            p++;
                            if(*p == ' ')
                                p += skipSpaces(p, 0);
                            fourthLine = (struct codes*)malloc(sizeof(codeTable));
                            fourthLine -> address = ic;
                            fourthLine -> word.era = 0;
                            fourthLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                            fourthLine -> memoryType = 3;
                            newLine(fourthLine);
                        }
                        else {
                            printf("\nvariable '%s' not found\n",copy);
                            errorExists = 1;
                        }
                    }
                    else if (operand2AddressingMethod == DIRECTREGISTER) {
                        while(*p < '0' || *p > '7') {
                            p++;
                        }
                        firstLine -> word.Bits.commandBits.destAddr = 3;
                        thirdLine = (struct codes*)malloc(sizeof(codeTable));
                        thirdLine -> address = ic;
                        thirdLine -> word.era = 2;
                        thirdLine -> word.Bits.regBits.srcBits = 0;
                        thirdLine -> word.Bits.regBits.destBits = *p;
                        thirdLine -> memoryType = 2;
                        newLine(thirdLine);
                    }
                }
                else if (operand1AddressingMethod  == DIRECT) {
                    if (*p == ' ')
                        p += skipSpaces(p, 0);
                    i = 0;
                    while (*p != ' ') {
                        copy[i] = *p;
                        i++;
                        p++;
                    }
                    copy[i] = '\0';
                    if (getAddress(copy) != -1) {
                    
                        if (isSymbolExternal(copy)) {
                           
                            externsList = (struct externs*) malloc(sizeof(externTable));
                            strcpy(externsList->name, copy);
                            externsList->address = ic;
                            insertToExternList(externsList);
                          
                        }
                        firstLine -> word.Bits.commandBits.sourceAddr = 1;
                        secondLine = (struct codes*) malloc(sizeof(codeTable));
                        secondLine -> address = ic;
                        secondLine -> word.era = 2;
                        secondLine -> word.Bits.value = getAddress(copy);
                        secondLine -> memoryType = 3;
                        newLine(secondLine);
                    }
                    else {
                        printf("\nvariable '%s' not found\n", copy);
                        errorExists = 1;
                    }
                    p=strtok(NULL, " \n");
                    operand2AddressingMethod = getAddressingMethod(p);
                    if (operand2AddressingMethod == IMMEDIATE) {
                        firstLine -> word.Bits.commandBits.destAddr = 0;
                        thirdLine = (struct codes*)malloc(sizeof(codeTable));
                        thirdLine -> address = ic;
                        thirdLine -> word.era = 0;
                        thirdLine -> word.Bits.value = findInt(p);
                        thirdLine -> memoryType = 3;
                        newLine(thirdLine);
                    }
                    else if (operand2AddressingMethod == DIRECT) {
                        if (*p == ' ')
                            p += skipSpaces(p, 0);
                        i = 0;
                        while (*p != ' ') {
                            copy[i] = *p;
                            i++;
                            p++;
                        }
                        copy[i] = '\0';
                        if (getAddress(copy) != -1) {
                            if (isSymbolExternal(copy)) {
                                
                                externsList = (struct externs*) malloc(sizeof(externTable));
                                strcpy(externsList->name, copy);
                                externsList->address = ic;
                                insertToExternList(externsList);
                                
                            }
                            firstLine -> word.Bits.commandBits.destAddr = 1;
                            thirdLine = (struct codes*) malloc(sizeof(codeTable));
                            thirdLine -> address = ic;
                            thirdLine -> word.era = 2;
                            thirdLine -> word.Bits.value = getAddress(copy);
                            thirdLine -> memoryType = 3;
                            newLine(thirdLine);
                        }
                        else {
                            printf("\nError: variable '%s' not found\n", copy);
                            errorExists = 1;
                        }
                    }
                    else if (operand2AddressingMethod == STRUCTACCESS) {
                        if (*p == ' ')
                            p += skipSpaces(p, 0);
                        i = 0;
                        while (*p != '.') {
                            copy[i] = *p;
                            i++;
                            p++;
                        }
                        copy[i] = '\0';
                        if(isLabelExists(copy) ) {
                            firstLine -> word.Bits.commandBits.destAddr=2;
                            thirdLine = (struct codes*)malloc(sizeof(codeTable));
                            thirdLine -> address=ic;
                            thirdLine -> word.era=2;
                            thirdLine -> word.Bits.value = getAddress(copy);
                            thirdLine -> memoryType = 3;
                            newLine(thirdLine);
                            while(*p < '0' || *p > '7') {
                                p++;
                            }
                            fourthLine = (struct codes*)malloc(sizeof(codeTable));
                            fourthLine -> address = ic;
                            fourthLine -> word.era = 0;
                            fourthLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                            fourthLine -> memoryType = 3;
                            newLine(fourthLine);
                        }
                        else {
                            printf("\nvariable '%s' not found\n",copy);
                            errorExists = 1;
                        }
                    }
                    else if (operand2AddressingMethod == DIRECTREGISTER) {
                        while(*p < '0' || *p > '7') {
                            p++;
                        }
                        firstLine -> word.Bits.commandBits.destAddr = 3;
                        thirdLine = (struct codes*)malloc(sizeof(codeTable));
                        thirdLine -> address = ic;
                        thirdLine -> word.era = 2;
                        thirdLine -> word.Bits.regBits.srcBits = 0;
                        thirdLine -> word.Bits.regBits.destBits = *p;
                        thirdLine -> memoryType = 2;
                        newLine(thirdLine);
                    }
                }
                else if(operand1AddressingMethod == 2) {
                    if (*p == ' ')
                        p += skipSpaces(p, 0);
                    i = 0;
                    while (*p != '.') {
                        copy[i] = *p;
                        i++;
                        p++;
                    }
                    copy[i] = '\0';
                    if (isLabelExists(copy)) {
                        firstLine -> word.Bits.commandBits.sourceAddr = 2;
                        secondLine = (struct codes*)malloc(sizeof(codeTable));
                        secondLine -> address = ic;
                        secondLine -> word.era = 2;
                        secondLine -> word.Bits.value = getAddress(copy);
                        secondLine -> memoryType = 3;
                        newLine(secondLine);
                        p++;
                        thirdLine = (struct codes*)malloc(sizeof(codeTable));
                        thirdLine -> address = ic;
                        thirdLine -> word.era = 0;
                        thirdLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                        thirdLine -> memoryType = 3;
                        newLine(thirdLine);
                    }
                    p = strtok(NULL, " \n");
                    operand2AddressingMethod = getAddressingMethod(p);
                    if (operand2AddressingMethod == IMMEDIATE) {
                        firstLine -> word.Bits.commandBits.destAddr = 0;
                        thirdLine = (struct codes*)malloc(sizeof(codeTable));
                        thirdLine -> address = ic;
                        thirdLine -> word.era = 0;
                        thirdLine -> word.Bits.value = findInt(p);
                        thirdLine -> memoryType = 3;
                        newLine(thirdLine);
                    }
                    else if (operand2AddressingMethod == DIRECT) {
                        if (*p == ' ')
                            p += skipSpaces(p, 0);
                        i = 0;
                        while (*p != ' ' && *p != '\n' && *p != '\0') {
                            copy[i] = *p;
                            i++;
                            p++;
                        }
                        copy[i] = '\0';
                        if (getAddress(copy)!= 0) {
                            if (isSymbolExternal(copy)) {
                                externsList = (struct externs*) malloc(sizeof(externTable));
                                strcpy(externsList->name, copy);
                                externsList->address = ic;
                                insertToExternList(externsList);
                                printf("\n%s was added to extern list\n",externsList->name);
                            }
                            firstLine -> word.Bits.commandBits.destAddr = 1;
                            thirdLine = (struct codes*)malloc(sizeof(codeTable));
                            thirdLine -> address = ic;
                            thirdLine -> word.era = 2;
                            thirdLine -> word.Bits.value = getAddress(copy);
                            thirdLine -> memoryType = 3;
                            newLine(thirdLine);
                        }
                        else {
                            printf("\nthe variable %s do not exist in the file\n", copy);
                            errorExists = 1;
                        }
                    }
                    else if (operand2AddressingMethod == STRUCTACCESS) {
                        if (*p == ' ')
                            p += skipSpaces(p, 0);
                        i = 0;
                        while (*p != '.') {
                            copy[i] = *p;
                            i++;
                            p++;
                        }
                        copy[i] = '\0';
                        if (isLabelExists(copy)) {
                            firstLine -> word.Bits.commandBits.destAddr = 2;
                            thirdLine = (struct codes*)malloc(sizeof(codeTable));
                            thirdLine -> address = ic;
                            thirdLine -> word.era = 2;
                            thirdLine -> word.Bits.value = getAddress(copy);
                            thirdLine -> memoryType = 3;
                            newLine(thirdLine);
                            p++;
                            if (*p == ' ')
                                p += skipSpaces(p, 0);
                            fourthLine = (struct codes*)malloc(sizeof(codeTable));
                            fourthLine -> address = ic;
                            fourthLine -> word.era = 0;
                            fourthLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                            fourthLine -> memoryType = 3;
                            newLine(fourthLine);
                        }
                        else {
                            printf("\nError: variable '%s' not found\n", copy);
                            errorExists = 1;
                        }
                    }
                    else if (operand2AddressingMethod == DIRECTREGISTER) {
                        while(*p < '0' || *p > '7') {
                            p++;
                        }
                        firstLine -> word.Bits.commandBits.destAddr = 3;
                        thirdLine = (struct codes*)malloc(sizeof(codeTable));
                        thirdLine -> address = ic;
                        thirdLine -> word.era = 0;
                        thirdLine -> word.Bits.regBits.srcBits = 0;
                        thirdLine -> word.Bits.regBits.destBits = *p;
                        thirdLine -> memoryType = 2;
                        newLine(thirdLine);
                    }
                }
                else if (operand1AddressingMethod == DIRECTREGISTER) {
                    while(*p < '0' || *p > '7') {
                        p++;
                    }
                    firstLine -> word.Bits.commandBits.sourceAddr = 3;
                    secondLine = (struct codes*)malloc(sizeof(codeTable));
                    secondLine -> address = ic;
                    secondLine -> word.era = 0;
                    secondLine -> word.Bits.regBits.srcBits = *p;
                    secondLine -> memoryType = 2;
                    newLine(secondLine);
                    p = strtok(NULL, " \n");
                    operand2AddressingMethod = getAddressingMethod(p);
                    if (operand2AddressingMethod == IMMEDIATE) {
                        firstLine -> word.Bits.commandBits.destAddr = 0;
                        thirdLine = (struct codes*)malloc(sizeof(codeTable));
                        thirdLine -> address = ic;
                        thirdLine -> word.era = 0;
                        thirdLine -> word.Bits.value = findInt(p);
                        thirdLine -> memoryType = 3;
                        newLine(thirdLine);
                    }
                    else if (operand2AddressingMethod == DIRECT) {
                        if (*p == ' ')
                            p += skipSpaces(p, 0);
                        i = 0;
                        while (*p != ' ' && *p != '\0') {
                            copy[i] = *p;
                            i++;
                            p++;
                        }
                        copy[i] = '\0';
                        if (getAddress(copy) != -1) {
                            if (isSymbolExternal(copy)) {
                                printf("\n%s symbol is extern\n",copy);
                                externsList = (struct externs*) malloc(sizeof(externTable));
                                strcpy(externsList->name, copy);
                                externsList->address = ic;
                                insertToExternList(externsList);
                                printf("\n%s was added to extern list\n",externsList->name);
                            }
                            firstLine -> word.Bits.commandBits.destAddr = 1;
                            thirdLine = (struct codes*)malloc(sizeof(codeTable));
                            thirdLine -> address = ic;
                            thirdLine -> word.era = 2;
                            thirdLine -> word.Bits.value = getAddress(copy);
                            thirdLine -> memoryType = 3;
                            newLine(thirdLine);
                        }
                        else {
                            printf("\nvariable '%s' not found\n",copy);
                            errorExists = 1;
                        }
                    }
                    else if (operand2AddressingMethod == STRUCTACCESS) {
                        if (*p == ' ')
                            p += skipSpaces(p, 0);
                        i = 0;
                        while (*p != '.') {
                            copy[i] = *p;
                            i++;
                            p++;
                        }
                        copy[i] = '\0';
                        if (isLabelExists(copy)) {
                            firstLine -> word.Bits.commandBits.destAddr = 2;
                            thirdLine = (struct codes*)malloc(sizeof(codeTable));
                            thirdLine -> address = ic;
                            thirdLine -> word.era = 2;
                            thirdLine -> word.Bits.value = getAddress(copy);
                            thirdLine -> memoryType = 3;
                            newLine(thirdLine);
                            p++;
                            if (*p == ' ')
                                p += skipSpaces(p, 0);
                            fourthLine = (struct codes*)malloc(sizeof(codeTable));
                            fourthLine -> address = ic;
                            fourthLine -> word.era = 0;
                            fourthLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                            fourthLine -> memoryType = 3;
                            newLine(fourthLine);
                        }
                        else {
                            printf("\nvariable '%s' not found\n", copy);
                            errorExists = 1;
                        }
                    }
                    else if (operand2AddressingMethod == DIRECTREGISTER) {
                        while(*p < '0' || *p > '7') {
                            p++;
                        }
                        firstLine -> word.Bits.commandBits.destAddr = 3;
                        secondLine -> word.Bits.regBits.destBits = *p;
                    }
                }
            }
            else if (command >= NOT && command <= JSR) { /*  second group of commands */
       
                firstLine = (struct codes*) malloc(sizeof(codeTable));                
                firstLine -> address = ic;                
                firstLine -> word.era = 0;                
                firstLine -> word.Bits.commandBits.command = command - 1;                
                firstLine -> memoryType = 1;                
                newLine(firstLine);                
                p = strtok(NULL, ",");
                
                if (p) {
                    i = strlen(p);
                    if (p[i - 1] == '\n') { p[i - 1] = '\0'; }
                    operand2AddressingMethod = getAddressingMethod(p);
                }
                if (operand2AddressingMethod == IMMEDIATE) {
                    firstLine -> word.Bits.commandBits.destAddr = 0;
                    firstLine -> word.Bits.commandBits.sourceAddr = 0;
                    secondLine = (struct codes*)malloc(sizeof(codeTable));
                    secondLine -> address = ic;
                    secondLine -> word.era = 0;
                    secondLine -> word.Bits.value = findInt(p);
                    secondLine -> memoryType = 3;
                    newLine(secondLine);
                }
                else if (operand2AddressingMethod == DIRECT) {
                    
                    if (*p == ' ')
                        p += skipSpaces(p, 0);
                    i = 0;
                    while (*p != ' ' && *p != '\0') {
                        copy[i] = *p;
                        i++;
                        p++;
                    }
                    copy[i] = '\0';
                    if (getAddress(copy) != -1) {
                       
                        if (isSymbolExternal(copy)) {
                            
                            externsList = (struct externs*) malloc(sizeof(externTable));
                            strcpy(externsList->name, copy);
                            externsList->address = ic;
                            insertToExternList(externsList);
                            
                        }
                        firstLine -> word.Bits.commandBits.sourceAddr = 0;
                        firstLine -> word.Bits.commandBits.destAddr = 1;
                        secondLine = (struct codes*)malloc(sizeof(codeTable));
                        secondLine -> address = ic;
                        if (isSymbolExternal(copy)) {
                            secondLine -> word.era = 1; }
                        else                { secondLine -> word.era = 2; }
                        secondLine -> word.Bits.value = getAddress(copy);
                        secondLine -> memoryType = 3;
                        newLine(secondLine);
                       
                    }
                    else {
                        printf("\nvariable '%s' not found\n",copy);
                        errorExists = 1;
                    }
                }
                else if (operand2AddressingMethod == STRUCTACCESS) {
                    if (*p == ' ')
                        p += skipSpaces(p, 0);
                    i = 0;
                    while (*p != '.') {
                        copy[i] = *p;
                        i++;
                        p++;
                    }
                    copy[i] = '\0';
                    if(isLabelExists(copy) ) {
                        firstLine -> word.Bits.commandBits.destAddr = 2;
                        firstLine -> word.Bits.commandBits.sourceAddr = 0;
                        secondLine = (struct codes*)malloc(sizeof(codeTable));
                        secondLine -> address = ic;
                        secondLine -> word.era = 2;
                        secondLine -> word.Bits.value = getAddress(copy);
                        secondLine -> memoryType = 3;
                        newLine(secondLine);
                        p++;
                        if (*p == ' ')
                            p += skipSpaces(p, 0);
                        thirdLine = (struct codes*)malloc(sizeof(codeTable));
                        thirdLine -> address = ic;
                        thirdLine -> word.era = 0;
                        thirdLine -> word.Bits.value = ((int)*p)-48; /*turn into int, ASCII 0 = 48*/
                        thirdLine -> memoryType = 3;
                        newLine(thirdLine);
                    }
                    else {
                        printf("\nvariable '%s' not found\n",copy);
                        errorExists = 1;
                    }
                }
                else if (operand2AddressingMethod == DIRECTREGISTER) {
                    while(*p < '0' || *p > '7') {
                        p++;
                    }
                    firstLine -> word.Bits.commandBits.destAddr = 3;
                    firstLine -> word.Bits.commandBits.sourceAddr = 0;
                    secondLine = (struct codes*)malloc(sizeof(codeTable));
                    secondLine -> address = ic;
                    secondLine -> word.era = 2;
                    secondLine -> word.Bits.regBits.srcBits = 0;
                    secondLine -> word.Bits.regBits.destBits = *p;
                    secondLine -> memoryType = 2;
                    newLine(secondLine);
                }
            }
            else if (command == RTS || command == STOP) { /*  third group of commands */
                firstLine = (struct codes*)malloc(sizeof(codeTable));
                firstLine -> address = ic;
                firstLine -> word.era = 0;
                firstLine -> word.Bits.commandBits.command = command - 1;
                firstLine -> word.Bits.commandBits.sourceAddr = 0;
                firstLine -> word.Bits.commandBits.destAddr = 0;
                firstLine -> memoryType = 1;
                newLine(firstLine);
            }
        }
        else if ((directiveType = isDirectiveWord(p))) {
            switch (directiveType) {
                case DATA:
                    continue;
                    break;
                case STRING:
                    continue;
                    break;
                case STRUCT:
                    continue;
                    break;
                case ENTRY:
                    p = strtok(NULL, ", ");
                    if (!p) {
                        printf("\nError: expected label after '.entry' in line %d!\n", lineNumber);
                        errorExists = 1;
                    }
                    while (p) {
                        i = strlen(p);
                        if (p[i - 1] == '\n') { p[i - 1] = '\0'; }
                        tempAddress = getAddress(p);
                        if (tempAddress == 0) {
                            errorExists = 1;
                            printf("\nError: no label found for '.entry' statement\n");
                            break;
                        }
                        entriesList = (struct entries*) malloc(sizeof(entryTable));
                        strcpy(entriesList -> name, p);
                        entriesList -> address = getAddress(p);
                        insertToEntryList(entriesList);
                        p = strtok(NULL, ", ");
                    }
                    break;
                case EXTERN:
                    p = strtok(NULL, ", ");
                    if (!p) {
                        errorExists = 1;
                        printf("\nError: expected label after 'extern' in line: %d!\n", lineNumber);
                    }
                    while (p) {
                        i = strlen(p);
                        if (p[i - 1] == '\n') { p[i - 1] = '\0'; }
                        tempAddress = getAddress(p);
                        if (tempAddress == -1) {
                            errorExists = 1;
                            printf("\nError: no label found for '.extern' statement\n");
                            break;
                        }
                        p = strtok(NULL, ", ");
                    }
                    continue;
                    break;
                case 0:
                    continue;
                    break;
            }
        }

        lineNumber++;
    }
} /*   End of read file 2 */
