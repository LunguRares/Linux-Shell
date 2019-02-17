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
void get_input(char input[MAXBUFFSIZE]);
int execute(char input[MAXBUFFSIZE]);
void tokenize(TokenList* Tokens);
int noTokens(TokenList* Tokens);
int exitCommand(TokenList* Tokens);

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

    do{
        char input[MAXBUFFSIZE] = {};
        display_prompt();
        get_input(input);
        status = execute(input);
    }while(status);
}


/*
*    Prints the prompt on the screen
*/
void display_prompt(){
    printf(PROMPT);
}

/*
*    Reads user input from the keyboard
*    Return Value: List of tokens and the number of tokens
*/
void get_input(char input[MAXBUFFSIZE]){

    char c;
    int index = 0;

    do{
        c = getchar();
        input[index] = c;
        index++;
    }while(index<MAXBUFFSIZE && !(c==EOF && index==1) && c!='\n');

    if(c==EOF && index==1){
        input[0] = 'e';
        input[1] = 'x';
        input[2] = 'i';
        input[3] = 't';
    }

}

/*
*    Tokenizes the input command and executes commands based on the tokens
*    Parameter: Structure containing an integer representing the number of Tokens and an array of Tokens
*    Return Value: - 0 = the shell is still running
*                  - 1 = exit the shelll
*/
int execute(char input[MAXBUFFSIZE]){

    TokenList Tokens = {0};
    strcpy(Tokens.command,input);       //Saves the input in the command in case a functions needs the full command line from the user. Also useful for error checking
    tokenize(&Tokens);                  //Breaks the command into tokens

    if(noTokens(&Tokens))    //If no argumnts then just return 1
        return 1;

    //Uncomment this for testing (Prints the number of tokens and then each token on a separate line)
/*
    printf("%d\n",Tokens.tokenNumber);
    for(int i=0;i<Tokens.tokenNumber;i++)
        printf("'%s'\n",Tokens.tokens[i]);
*/

    if(exitCommand(&Tokens))
        return 0;

    printf("Command not found \n");
    return 1;
}

void tokenize(TokenList* Tokens){

    char* pointer;

    pointer = strtok (Tokens->command,DELIMITERS);
        while (pointer != NULL){
            strcpy(Tokens->tokens[Tokens->tokenNumber],pointer);
            Tokens->tokenNumber++;
            pointer = strtok (NULL, DELIMITERS);
        }

}

int noTokens(TokenList* Tokens){
    if(Tokens->tokenNumber==0)
        return 1;
    else
        return 0;
}

int exitCommand(TokenList* Tokens){
    if(strcmp(Tokens->tokens[0],"exit")==0)     //The user entered an exit command, return 0 to terminate the shell loop
        return 1;
    else
        return 0;
}
