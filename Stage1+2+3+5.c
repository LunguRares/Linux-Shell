//
//  Stage1+2+3+5.c
//  
//
//  Created by Benjamin Garside on 2/28/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXBUFFSIZE 513
#define PROMPT "(☭ ͜ʖ ☭)> "
#define NUMBEROFTOKENS 50
#define SIZEOFTOKENS 50
#define DELIMITERS " \t|><&;\n\377"
#define SIZEOFHISTORY 20
//#define PATH_MAX 256

/*
 *    Structure used for passing tokens
 */
typedef struct{
    int tokenNumber;
    char tokens[NUMBEROFTOKENS][SIZEOFTOKENS];
    char command[MAXBUFFSIZE];
}TokenList;

typedef struct{
    int next;
    char memArray[SIZEOFHISTORY][MAXBUFFSIZE];
}History;

void loop();
void display_prompt();
void get_input(char input[MAXBUFFSIZE]);
int execute(char input[MAXBUFFSIZE]);
void tokenize(TokenList* Tokens);

int getCommandID(TokenList* Tokens);
int checkNoTokens(TokenList* Tokens);
int checkExitCommand(TokenList* Tokens);
int checkGetPath(TokenList* Tokens);
int checkSetPath(TokenList* Tokens);
int checkHistoryCommand(TokenList* Tokens);
int checkInvokeLastCommand(TokenList* Tokens);
int checkInvokeCommandNumber(TokenList* Tokens);
int checkInvokeCommandNegative(TokenList* Tokens);


int aliasOrExternalCommand(TokenList* Tokens);
int getpath(TokenList* Tokens);
int setpath(TokenList* Tokens);

int setenv(const char *name, const char *value, int overwrite);


void print_history();
void addto_history(char command[MAXBUFFSIZE]);
int invoke_last_command();
int invoke_command_number(char* input);
int invoke_command_negative(char* input);
int numberof_commands();
int str_to_int(char* stringof_digits);
int is_empty();

History history = {0, {0}};

int main()
{
    /*SETUP*/
    char* path;
    char* home;
    //char cwd[PATH_MAX];
    
    path = getenv("PATH");
    home = getenv("HOME");
    
    //if (getcwd(cwd, sizeof(cwd)) == NULL)
    //perror("Error in getcwd()");
    //printf("%s \n",cwd);
    
    if(chdir(home)==-1){
        perror("Error in chdir()");
        return -1;
    }
    
    //getcwd(cwd,sizeof cwd);
    //printf("%s \n",cwd);
    
    /*Display prompt, Read, Parse, Repeat*/
    loop();
    
    /*CLEANING*/
    setenv("PATH",path,1);
    return 0;
}

/*
 *    Reads commands from the user and executes them if they are valid
 */
void loop(){
    
    int status;
    
    do{
        char input[MAXBUFFSIZE] = "";
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
        input[4] = '\0';
    }
    
    char* pointer;
    pointer = strtok(input, DELIMITERS);
    
    if(!((index == 1 && c == '\n') || /*(*pointer == '!')*/)) {
        
        addto_history(input);
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
    
    //Uncomment this for testing (Prints the number of tokens and then each token on a separate line)
    /*
     printf("%d\n",Tokens.tokenNumber);
     for(int i=0;i<Tokens.tokenNumber;i++)
     printf("'%s'\n",Tokens.tokens[i]);
     */
    
    int cmdID = getCommandID(&Tokens);
    
    switch(cmdID){
        case 0:
            return 1;
            
        case 1:
            return 0;
            
        case 2:
            return getpath(&Tokens);
            
        case 3:
            return setpath(&Tokens);
            
        case 5:
            print_history();
            return 1;
            
        case 6:
            return invoke_last_command();
            
        case 7:
            return invoke_command_number(Tokens.tokens[0]);
            
        case 8:
            return invoke_command_negative(input);
            
        default: {
            return aliasOrExternalCommand(&Tokens);
        }
    }
    
    
    
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

int checkNoTokens(TokenList* Tokens){
    
    if(Tokens->tokenNumber==0)
        return 1;
    else
        return 0;
}

int checkExitCommand(TokenList* Tokens){
    
    if(strcmp(Tokens->tokens[0],"exit")==0)     //The user entered an exit command, return 0 to terminate the shell loop
        return 1;
    else
        return 0;
}

int checkGetPath(TokenList* Tokens){
    
    if(strcmp(Tokens->tokens[0],"getpath")==0)     //The user entered an exit command, return 0 to terminate the shell loop
        return 1;
    else
        return 0;
}

int checkSetPath(TokenList* Tokens){
    
    if(strcmp(Tokens->tokens[0],"setpath")==0)     //The user entered an exit command, return 0 to terminate the shell loop
        return 1;
    else
        return 0;
}

int checkHistoryCommand(TokenList* Tokens) {
    
    if(strcmp(Tokens->tokens[0],"history")==0)
        return 1;
    else
        return 0;
}

int checkInvokeLastCommand(TokenList* Tokens) {
    
    if(strcmp(Tokens->tokens[0], "!!") == 0)
        return 1;
    else
        return 0;
}

int checkInvokeCommandNumber(TokenList* Tokens) {
    
    if(Tokens->tokens[0][0] == '!' && Tokens->tokens[0][1] != '-' && strlen(Tokens->tokens[0]) <= 3)
        return 1;
    
    return 0;
}

int checkInvokeCommandNegative(TokenList* Tokens) {
    
    if(Tokens->tokens[0][0] == '!' && Tokens->tokens[0][1] == '-' && strlen(Tokens->tokens[0]) <= 4)
        return 1;
    
    return 0;
}

int getCommandID(TokenList* Tokens){
    
    if(checkNoTokens(Tokens))
        return 0;
    if(checkExitCommand(Tokens))
        return 1;
    if(checkGetPath(Tokens))
        return 2;
    if(checkSetPath(Tokens))
        return 3;
    if(checkHistoryCommand(Tokens))
        return 5;
    if(checkInvokeLastCommand(Tokens))
        return 6;
    if(checkInvokeCommandNumber(Tokens))
        return 7;
    if(checkInvokeCommandNegative(Tokens))
        return 8;
    
    return 100;
}

int getpath(TokenList* Tokens){
    
    if(Tokens->tokenNumber!=1){
        printf("Syntax error: Too many arguments;\n");
        return 1;
    } else {
        char* path;
        path = getenv("PATH");
        if(path==NULL){
            printf("Error: PATH variable doesn't exist;\n");
            return 0;
        } else {
            printf("%s\n",path);
            return 1;
        }
    }
}

int setpath(TokenList* Tokens){
    
    if(Tokens->tokenNumber!=2){
        printf("Syntax error: Incorect number of arguments;\n");
        return 1;
    } else {
        if(setenv("PATH",Tokens->tokens[1],1)==-1) {
            perror("Error: ");
            return 1;
        }
        else{
            printf("Path succesfully changed to: %s\n",Tokens->tokens[1]);
            return 1;
        }
    }
    
}

int aliasOrExternalCommand(TokenList* Tokens){
    
    //check if command matches any of the aliases
    //if it does, do return execute(cmd), where cmd is the saved command for the alias
    
    
    //if it doesn't, try to run the command as external program
    
    char *args[Tokens->tokenNumber];
    
    for (int i=0;i<(Tokens->tokenNumber);i++){
        args[i]=Tokens->tokens[i];
    }
    
    args[(Tokens->tokenNumber)]=NULL;
    
    
    pid_t  pid;
    pid = fork();
    
    if (pid == -1){
        perror("Error in fork()");
        exit(EXIT_FAILURE);
    }else {
        if(pid==0){
            if(execvp(*args,args)<0)
                perror("Error");
            exit(0);
        }else {
            wait(NULL);
            return 1;
        }
    }
    
}

void print_history() {
    
    int no_commands = numberof_commands();
    int next_print;
    
    if(no_commands == SIZEOFHISTORY)
        next_print = history.next;
    else
        next_print = history.next - no_commands;
    
    
    int count = 0;
    for(int i = 0; i < no_commands; i++) {
        
        printf("%d, ", i + 1);
        
        while(history.memArray[next_print][count] != '\0') {
            
            printf("%c", history.memArray[next_print][count]);
            count++;
        }
        
        printf("\n");
        next_print = (next_print + 1) % SIZEOFHISTORY;
        count = 0;
    }
}

void addto_history(char command[MAXBUFFSIZE]) {
    
    int next = history.next;
    
    for(int i = 0; i < MAXBUFFSIZE; i++) {
        
        history.memArray[next][i] = *(command + i);
    }
    
    history.next = (next + 1) % SIZEOFHISTORY;
}

int invoke_last_command() {
    
    char last_command[MAXBUFFSIZE] = {0};
    int no_commands = numberof_commands();
    int index;
    int count = 0;
    
    if(is_empty())
        printf("Error: no commands in history.\n");
    
    else if(no_commands == SIZEOFHISTORY && history.next == 0)
        index = 19;
    
    else
        index = history.next - 1;
    
    while(history.memArray[index][count] != '\0'){
        
        last_command[count] = history.memArray[index][count];
        //printf("%c",last_command[count]);
        count++;
    }
    //printf("\n");
    return execute(last_command);
}

int invoke_command_number(char* input) {
    
    char prev_command[MAXBUFFSIZE] = {0};
    int no_commands = numberof_commands();
    int num_arg = str_to_int(input+1);
    int index;
    int count = 0;
    
    if(is_empty())
        printf("Error: no commands in history.\n");
    
    else if(no_commands == SIZEOFHISTORY)
        index = (history.next + num_arg) % SIZEOFHISTORY - 1;
    
    else
        index = history.next - no_commands + num_arg - 1;
    
    //printf("%d\n", index);
    //printf("%d\n", num_arg);
    
    if(index >= 0 && index < 20 && num_arg <= no_commands) {
        
        while(history.memArray[index][count] != '\0'){
            
            prev_command[count] = history.memArray[index][count];
            printf("%c", prev_command[count]);
            count++;
        }
        printf("\n");
    }
    
    else {
        
        printf("Error: incorrect argument given.\n");
    }
    
    return execute(prev_command);
}

int invoke_command_negative(char* input) {
    
    char prev_command[MAXBUFFSIZE] = {0};
    int no_commands = numberof_commands();
    int num_arg = str_to_int(input+2);
    int index;
    int count = 0;
    
    if(is_empty())
        printf("Error: no commands in history.\n");
    
    else if(no_commands == SIZEOFHISTORY)
        index = (history.next + no_commands - num_arg) % SIZEOFHISTORY;
    
    else
        index = history.next - num_arg;
    
    if(index >= 0 && index < 20 && num_arg <= no_commands) {
        
        while(history.memArray[index][count] != '\0'){
            
            prev_command[count] = history.memArray[index][count];
            printf("%c", prev_command[count]);
            count++;
        }
        printf("\n");
    }
    
    else {
        
        printf("Error: incorrect argument given.\n");
    }
    
    return execute(prev_command);
}

int numberof_commands() {
    
    int size = 0;
    for (int i = 0; i < SIZEOFHISTORY; i++) {
        
        if(history.memArray[i][0] == '\0')
            continue;
        
        size++;
    }
    
    return size;
}

int str_to_int(char* stringof_digits) {
    
    int number = 0;
    
    int count = 0;
    while(*(stringof_digits + count) != '\0') {
        
        //printf("%c\n", *(stringof_digits + count));
        number = number * 10 + (*(stringof_digits + count) - '0');
        count++;
    }
    
    return number;
}

int is_empty() {
    
    int no_commands = numberof_commands();
    
    if(no_commands == 0 && history.next == 0)
        return 1;
    
    return 0;
}
