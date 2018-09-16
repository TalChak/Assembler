/*Tal Chaklai 200854982*/
/*Boris Manashirov 310511530*/
/*group: 41*/

#include "assembler.h"
#include <string.h>		  

int ic = 100;        /*instruction counter*/
int dc = 0;         /*data counter */
struct symbol* headSymbol = NULL;    /* head of symbol likned list */
struct datas* headData = NULL;                      /* head of data linked list */
struct externs* headExterns = NULL;                        /* head of externs linked list */
struct entries* headEntries = NULL;                        /* head of enries linked list */
struct codes* headCode = NULL; 						/*head of code linked list*/
int errorExists = 0;                      /*when an error occures,errorExists gets 1 */
char str32base[MAX_LENGTH_32BSTRING] = {0}; /* store the address in 32 weird base  */




/* create the files */
void assembler(char * fName) 
{

    FILE *ent = NULL;
    FILE *ext = NULL;
    FILE *ob  = NULL;
    struct datas* dataPtr   = NULL;
    struct entries*  entryPtr  = NULL;
    struct externs*  externPtr = NULL;
    struct codes* objPtr = NULL;
    char c[MAX_FILENAME]; /* store file name */
    strcpy(c, fName);


    char * file = strtok(c, ".");            /* store file name */
    char * fileExtension = strtok(NULL, "\n"); /* stoe file extention */

    if (strcmp(fileExtension, "as")) { /* if it's not an .as file */
        printf("\nError: file type expected: .as\n");
        exit(1);
    }

    /* First Pass */
    printf("\nPerforming first pass\n");
	
    first_pass(fName);

    /* Update ic */
    updateICAddress();

    /* Second pass*/
    printf("\nPerforming second pass\n");

    second_pass(fName);

    if (!errorExists) { printf("\nfinished with no errors\n"); }


    /* is there are no errors, create files*/
    if (!errorExists) {

    	printf("\nic :\t%d\n",ic);
    	printf("\ndc:\t%d\n",dc);
	/*print_list_ext(headExterns);*/
        printf("\ncreating files\n");

        if (headEntries != NULL) { /* check if list is empty */

            char entFile[strlen(file) + 4]; /*  store file name and .ent extension */
            strcpy(entFile, file);
            entFile[strlen(file) + 0] = '.';
            entFile[strlen(file) + 1] = 'e';
            entFile[strlen(file) + 2] = 'n';
            entFile[strlen(file) + 3] = 't';
            entFile[strlen(file) + 4] = '\0';

            ent = fopen(entFile, "w"); /* write to entries file */
            fprintf(ent, "\nLabel\t|\t32 Base\nname\t|\tAddress\n\n");

            for (entryPtr = headEntries; entryPtr != NULL; entryPtr = entryPtr->next) {
                fprintf(ent, "%s\t|\t%s\n", entryPtr-> name,decimalTo32base(entryPtr->address) );
            }

            fclose(ent);
        }
        else { printf("\nEntry labes does not exists.\n\n"); }

        if (headExterns != NULL) { /* if the extern list is not empty */

            char extFile[strlen(file) + 4]; /* store file name and .ext extension */
            strcpy(extFile, file);
            extFile[strlen(file) + 0] = '.';
            extFile[strlen(file) + 1] = 'e';
            extFile[strlen(file) + 2] = 'x';
            extFile[strlen(file) + 3] = 't';
            extFile[strlen(file) + 4] = '\0';

            ext = fopen(extFile, "w"); /* write to externs file */
            fprintf(ext, "\nLabel\t|\t32 Base\nname\t|\tAddress\n");
            
            
            for (externPtr = headExterns; externPtr != NULL; externPtr = externPtr->next) {
                fprintf(ext, "\n%s\t\t|\t%s\n", externPtr->name, decimalTo32base(externPtr->address));
            }

            fclose(ext);
        }
        else { printf("\nextern labels does not exists.\n\n"); }

        char objFile[strlen(file) + 3]; /* store file name and.ob extension */
        strcpy(objFile, file);
        objFile[strlen(file) + 0] = '.';
        objFile[strlen(file) + 1] = 'o';
        objFile[strlen(file) + 2] = 'b';
        objFile[strlen(file) + 3] = '\0';

        if (headCode) {
            ob = fopen(objFile, "w"); /* write to object file */
            fprintf(ob, "Base 32\t|\tBase 32\nAddress\t|\tCode\n");
            for (objPtr = headCode; objPtr != NULL; objPtr = objPtr->next) {
                if (objPtr->memoryType == 1) {
                    fprintf(ob, "%s\t|\t", decimalTo32base(objPtr->address));
                    fprintf(ob, "%s\n", decimalTo32base(bitsToNum(objPtr->word, COMMAND)));
                }
                else if (objPtr->memoryType == 2) {
                    fprintf(ob, "%s\t|\t", decimalTo32base(objPtr->address));
                    fprintf(ob, "%s\n", decimalTo32base(bitsToNum(objPtr->word, REGISTER)));
                }
                else if (objPtr->memoryType == 3) {
                    fprintf(ob, "%s\t|\t", decimalTo32base(objPtr->address));
                    fprintf(ob, "%s\n", decimalTo32base(bitsToNum(objPtr->word, VALUE)));
                }
            }

            /* add data to object file */
            for (dataPtr = headData; dataPtr != NULL; dataPtr = dataPtr->next) {
                fprintf(ob, "%s\t|\t",decimalTo32base(dataPtr->address + ic));
                fprintf(ob, "%s\n", decimalTo32base(dataPtr->data));
            }
            fclose(ob);
            
            
        }
        else { printf("\nObject file is empty.\n\n"); }
        printf("\nThree files were created\n");
    }
    else {
        printf("\nErrors were found in file\n");
	exit(1);
    }
 
}

int main(int argc ,char *argv[]) {

    int i;

    /* check if there is no input file */
    if (argc < 2) {
        printf("\nNo input file\n");
        exit(1);
    }

    for (i = 1; i < argc; i++) {

        printf("\nfile: %s\n", argv[i]);
        
	assembler(argv[i]);

	/* free all memory space*/
        freeAll();

        printf("\n");
    }

    return 0;
}
