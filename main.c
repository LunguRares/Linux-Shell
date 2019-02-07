#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXBUFFSIZE 513
#define PROMPT "(☭ ͜ʖ ☭)>"
#define NUMBEROFTOKENS 50
#define SIZEOFTOKENS 50
#define DELIMITERS " \t|><&;\n\377"

/*
*    Structure used for passing tokens
*/
typedef struct{
    int tokenNumber;
    char tokens[NUMBEROFTOKENS][SIZEOFTOKENS];
    char command[MAXBUFFSIZE];
}TokenList;

void loop();
void display_prompt();
TokenList get_input();
int execute(TokenList *Tokens);

int main()
{
    /*SETUP*/


    /*Display prompt, Read, Parse, Repeat*/
    loop();

    /*CLEANING*/

    return 0;
}

/*
*    Reads commands from the user and executes them if they are valid
*/
void loop(){
    int status;
    TokenList Tokens;

    do{
        display_prompt();
        Tokens = get_input();
        status = execute(&Tokens);
    }while(status);
}


/*
*    Prints the prompt on the screen
*/
void display_prompt(){
    printf(PROMPT);
}

/*
*    Reads user input from the keyboard and parses it into tokens
*    Return Value: List of tokens and the number of tokens
*/
TokenList get_input(){

    char input[MAXBUFFSIZE] = {};
    char c;
    int index = 0;
    TokenList Tokens = {0};
    char* pointer;

    do{
        c = getchar();
        input[index] = c;
        index++;
    }while(index<MAXBUFFSIZE && !(c==EOF && index==1) && c!='\n');
    
    strcpy(Tokens.command,input);

    if(c==EOF && index==1){
        strcpy(Tokens.tokens[Tokens.tokenNumber],"exit");
        Tokens.tokenNumber++;
        return Tokens;
    }


    pointer = strtok (input,DELIMITERS);
    while (pointer != NULL){
        strcpy(Tokens.tokens[Tokens.tokenNumber],pointer);
        Tokens.tokenNumber++;
        pointer = strtok (NULL, DELIMITERS);
    }

    return Tokens;
}

/*
*    Executes commands based on the tokens
*    Parameter: Structure containing an integer representing the number of Tokens and an array of Tokens
*    Return Value: - 0 = the shell is still running
*                  - 1 = exit the shelll
*/
int execute(TokenList *Tokens){

    if(Tokens->tokenNumber==0)    //If no comments then just return 1
        return 1;

    //Uncomment this for testing (Prints the number of tokens and then each token on a separate line)

    printf("%d\n",Tokens->tokenNumber);
    for(int i=0;i<Tokens->tokenNumber;i++)
        printf("'%s'\n",Tokens->tokens[i]);


    if(strcmp(Tokens->tokens[0],"exit")==0)     //The user entered an exit command, return 0 to terminate the shell loop
        return 0;


    printf("Command not found \n");
    return 1;
}
