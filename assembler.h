/*Tal Chaklai 200854982*/
/*Boris Manashirov 310511530*/
/*group: 41*/


#ifndef ASSEMBLER_H
#define ASSEMBLER_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_LABEL 				31 /*maximum letters in label: 30 + '\0'*/
#define MAX_FILENAME 			30
#define MAX_COMMAND_LINE 		80
#define NUMBER_OF_COMMANDS		16
#define NUMBER_OF_REGISTERS		8
#define MAX_BUFFER  100

#define FALSE 0
#define TRUE  1

/*directive word types*/
#define NONDIRECTIVE  			0
#define DATA   					1
#define STRING					2
#define STRUCT					3
#define ENTRY					4
#define EXTERN					5

/*commands*/
#define NONCOMMAND 				0
#define MOV						1
#define CMP						2
#define ADD						3
#define SUB						4
#define LEA						5
#define NOT						6
#define CLR						7
#define INC						8
#define DEC						9	
#define JMP						10
#define BNE						11
#define RED						12
#define PRN						13
#define JSR						14
#define RTS						15
#define STOP						16


/*Addressing Methods*/
#define ADDRESSINGERROR	   				-1
#define IMMEDIATE					0	
#define DIRECT						1
#define STRUCTACCESS					2	
#define DIRECTREGISTER					3


#define MAX_LENGTH_32BSTRING 6 



 struct command
{
	char string[4];
	int cmd;
	short int  numOfOps;
};	




enum eOPCodeType
{
	Absolute,
	Relocatable,
	External
};

/*memory types*/
enum memType 
{ 
	COMMAND = 1, 
	REGISTER = 2,
	VALUE = 3 
};


/* Structure that represents the symbol table */

typedef struct symbol {
    char name[30];
    int address;
    int isExtern;
    int isCommand;
    struct symbol* next;
}symbolTable;    

/*structure that represents the data table*/
typedef struct datas{
    int address;
    short data;
    struct datas* next;
}dataTable;

/*structure that represnts the memory word */
typedef struct { 

    unsigned int era : 2;

    union { 
        struct {
            unsigned int command		: 4;
            unsigned int sourceAddr  	        : 2;	
            unsigned int destAddr		: 2;	
            unsigned int           		: 1; 	/*unsigned bit*/
        } commandBits;

        struct {
            unsigned int destBits 		: 4;
            unsigned int srcBits  		: 4;
            unsigned int          		: 1;	/* unsigned bit*/
        } regBits;
		
        int value : 9; 

    } Bits; 

}memoryWord;

/*structure that represents the code table*/
typedef struct codes {
    int address;
    enum memType memoryType; 
    memoryWord word; 
    struct codes* next;
}codeTable;

/*structure that represents the extern labels*/
typedef struct externs 
{
    char name[MAX_LABEL];
    int  address;
    struct externs* next;
}externTable;

/*structure that represents the entry labels table*/
typedef struct entries 
{
    char name[MAX_LABEL];
    int  address;
    struct entries* next;
}entryTable;


void   insertToDataList(struct datas* item);    /*insert data item in to the data table*/
void   insertToEntryList(struct entries* item); /*insert entry item in to the entries table*/
void   insertToExternList(struct externs* item); /*insert extern item in to the externs table*/
void   insertToSymbolList (struct symbol* item); /*insert symbol to the symbols table*/
void   newLine (struct codes* m); /*add a new line to the table*/

/*free allocated memory space for tables*/
void freeSymbolsSpace(struct symbol* head); 
void freeDataSpace(struct datas* head);
void freeCodeSpace(struct codes* head);
void freeExternsSpace(struct externs* head);
void freeEntriesSpace(struct entries* head);
void freeAll ();

int    isLabelExists (char *label); /*check if the line starts with a label*/
int    isNumber (char *str, int i); /*is the string a number*/
int    isRegister (char *x);       /*does the string represent a register*/
int	isStruct(char *strct, int i);   /*does the string represent a struct, if it is, add ths the number and the string to the data list*/ 
int    isDirectiveWord(char *s);   /*does the string represent a directive word*/
int    isVariable(char *x ,int index); /*does the string represent a variable*/
int    isCommand (char *s);         /*does the string represent a command*/
int    isCommandLegal (char *cmd, char *parameters);  /*make sure the command is legal according to all rules*/
int    isValidLabel (char * string); /*make sure the label is legal according to all rules*/
int    isSymbolExternal (char *label); /*is the symbol external*/

int    isRemark (char* s); /*remark starts with ';'*/
void   updateICAddress(void); /*update the instruction counter after the first run to 0*/
int    getAddressingMethod (char* x); /*check what is the addressing method of the given operand*/
int    addNumber(char *num); /*add a number data to the data table. if it's not a number it returns 0 */
int    addString (char *str); /*add a string data to the data table. if it's not a string it returns 0 */
int	addStruct(char *p); /*add a struct of data to the data table. if it's not a struct it returns 0 */
int    getAddress (char *ad); /*get the address from the symbol table if it exists. otherwise, return -1*/
char * mystrncpy (char *str,char *cpy, size_t n); /*copy a string to an array to the n index*/
int    isSymbolExternal(char *label); /*is the label external*/
void   assembler (char * fName); /*this function creates the files, whiles reading the information frfom the input file*/
void   first_pass(char * fName); /*insert labels to the symbol table, and data to the data table, and check if there are any errors */
void   second_pass (char * fName); /*insert data to the doce table, and check if there are any errors*/
int    findInt (char *a); /*returns an integer from a string*/
int    skipSpaces(char *n, int pointer); /*skip ' ' values */ 
char * decimalTo32base(int n);		/*convert base 10 to base 32 weird */

extern struct externs* headExterns; /*represents the head of the externs list*/
extern struct entries* headEntries; /*represents the head of the eentries list*/
extern struct codes* headCode; /*represents the head of the ecode list*/
extern struct symbol* headSymbol; /*represents the head of the symbol list*/
extern struct datas* headData; /*represents the head of the data list*/
extern int ic;  /*instruction counter*/
extern int dc; /*data counter*/
extern int errorExists; /*is there an error*/
extern char   str32base[MAX_LENGTH_32BSTRING];


#endif /*ASSEMBLER_H*/
