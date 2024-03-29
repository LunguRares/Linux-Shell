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
#define MAX_NO_ALIASES 10
#define CONTINUE_RUNNING 1
#define ALIAS_FILE ".aliases"
#define HIST_FILE ".hist_list"
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
    char aliases[2][MAX_NO_ALIASES][MAXBUFFSIZE];
    int noOfAliasCalls[MAX_NO_ALIASES];
}HistoryAndAliases;

void loop(HistoryAndAliases* HistoryAliases);
void display_prompt();
void get_input(char input[MAXBUFFSIZE],HistoryAndAliases* HistoryAliases);
int execute(char input[MAXBUFFSIZE],HistoryAndAliases* HistoryAliases);
void tokenize(TokenList* Tokens);

int getCommandID(TokenList* Tokens,HistoryAndAliases* HistoryAliases);
int checkNoTokens(TokenList* Tokens);
int checkExitCommand(TokenList* Tokens);
int checkGetPath(TokenList* Tokens);
int checkSetPath(TokenList* Tokens);
int checkHistoryCommand(TokenList* Tokens);
int checkInvokeLastCommand(TokenList* Tokens);
int checkInvokeCommandNumber(TokenList* Tokens);
int checkInvokeCommandNegative(TokenList* Tokens);
int checkAliasCommand(TokenList* Tokens);
int checkUnaliasCommand(TokenList* Tokens);
int checkDisplayAliases(TokenList* Tokens);
int checkAlias(TokenList* Tokens,HistoryAndAliases* HistoryAliases);

int externalCommand(TokenList* Tokens);
int getpath(TokenList* Tokens);
int setpath(TokenList* Tokens);
int changeDirectory(TokenList* Tokens);

void print_history(HistoryAndAliases* HistoryAliases);
void addto_history(char command[MAXBUFFSIZE],HistoryAndAliases* HistoryAliases);
int invoke_last_command(HistoryAndAliases* HistoryAliases);
int invoke_command_number(char* input,HistoryAndAliases* HistoryAliases);
int invoke_command_negative(char* input,HistoryAndAliases* HistoryAliases);
int numberof_commands(HistoryAndAliases* HistoryAliases);
int str_to_num(char* stringof_digits);
int is_history_empty(HistoryAndAliases* HistoryAliases);
void save_history(HistoryAndAliases* HistoryAliases);
void load_history(HistoryAndAliases* HistoryAliases);
void displayAliases(HistoryAndAliases* HistoryAliases);
void alias(TokenList* Tokens, HistoryAndAliases* HistoryAliases);
void unalias(TokenList* Tokens, HistoryAndAliases* HistoryAliases);
int substituteAlias(TokenList* Tokens, HistoryAndAliases* HistoryAliases);
void saveAliases(HistoryAndAliases* HistoryAliases);
void loadAliases(HistoryAndAliases* HistoryAliases);

int noOfAliases(HistoryAndAliases* HistoryAliases);
int aliasCanBeAdded(HistoryAndAliases* HistoryAliases, char* aliasName);
void resetNoAliasCalls(HistoryAndAliases* HistoryAlias);

int setenv(const char *name, const char *value, int overwrite);

int main()
{
    /*SETUP*/
    char* path;
    char* home;
    HistoryAndAliases HistoryAliases = {0, {{0}}, {{{0},{0}}}, {0}};
    //char cwd[PATH_MAX];

    path = getenv("PATH"); //original PATH saved as path to restore on exit
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

    loadAliases(&HistoryAliases);
    load_history(&HistoryAliases);

    /*Display prompt, Read, Parse, Repeat*/
    loop(&HistoryAliases);



    /*CLEANING*/
    chdir(getenv("HOME")); //change current directory to HOME
    save_history(&HistoryAliases);
    saveAliases(&HistoryAliases);
    setenv("PATH",path,1); //PATH restored by setting it back to path saved at start of shell
    path = getenv("PATH"); 
    printf("Restored the PATH to %s\n",path);
    return 0;
}

/*
 *    Reads commands from the user and executes them if they are valid
 */
void loop(HistoryAndAliases* HistoryAliases){

    int status;

    do{
        char input[MAXBUFFSIZE] = "";
        resetNoAliasCalls(HistoryAliases);
        display_prompt();
        get_input(input,HistoryAliases);
        status = execute(input,HistoryAliases);
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
void get_input(char input[MAXBUFFSIZE],HistoryAndAliases* HistoryAliases){

    int c;
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
        input[4] = '\n';
        input[5] = '\0';
    }

    input[511] = '\n';
    input[512] = '\0';

    if(index>=MAXBUFFSIZE)
         while (getchar() != '\n' );

    char* pointer;
    char inputCopy[MAXBUFFSIZE];
    strcpy(inputCopy, input); // make copy of input command

    pointer = strtok(inputCopy, DELIMITERS); // split copy of command into tokens for checking.

    // add raw command to history but only if command is not empty and command is not a history invocation.
    
    if(!((index == 1 && c == '\n') || (pointer == NULL) || (*pointer == '!'))) {

        addto_history(input,HistoryAliases);
    }
}

/*
 *    Tokenizes the input command and executes commands based on the tokens
 *    Parameter: Structure containing an integer representing the number of Tokens and an array of Tokens
 *    Return Value: - 0 = the shell is still running
 *                  - 1 = exit the shelll
 */
int execute(char input[MAXBUFFSIZE],HistoryAndAliases* HistoryAliases){

    TokenList Tokens = {0};
    strcpy(Tokens.command,input);       //Saves the input in the command in case a functions needs the full command line from the user. Also useful for error checking
    tokenize(&Tokens);                  //Breaks the command into tokens

    //Uncomment this for testing (Prints the number of tokens and then each token on a separate line)
    /*
     printf("%d\n",Tokens.tokenNumber);
     for(int i=0;i<Tokens.tokenNumber;i++)
     printf("'%s'\n",Tokens.tokens[i]);
     */

    int cmdID = getCommandID(&Tokens,HistoryAliases);

    switch(cmdID){
        case 0:
            return CONTINUE_RUNNING;

        case 1:
            return substituteAlias(&Tokens,HistoryAliases);

        case 2:
            return 0;

        case 3:
            return getpath(&Tokens);

        case 4:
            return setpath(&Tokens);

        case 5:
            return changeDirectory(&Tokens);

        case 6:
            print_history(HistoryAliases);
            return CONTINUE_RUNNING;

        case 7:
            return invoke_last_command(HistoryAliases);

        case 8:
            return invoke_command_number(Tokens.tokens[0],HistoryAliases);

        case 9:
            return invoke_command_negative(Tokens.tokens[0],HistoryAliases);

        case 10:
            displayAliases(HistoryAliases);
            return CONTINUE_RUNNING;

        case 11:
            alias(&Tokens, HistoryAliases);
            return CONTINUE_RUNNING;

        case 12:
            unalias(&Tokens,HistoryAliases);
            return CONTINUE_RUNNING;

        default: {
            return externalCommand(&Tokens);
        }
    }



}

void tokenize(TokenList* Tokens){

    char* pointer;
    char commandCopy[MAXBUFFSIZE];
    strcpy(commandCopy,Tokens->command);

    pointer = strtok (commandCopy,DELIMITERS);
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

    if(strcmp(Tokens->tokens[0],"getpath")==0) 
        return 1;
    else
        return 0;
}

int checkSetPath(TokenList* Tokens){

    if(strcmp(Tokens->tokens[0],"setpath")==0)
        return 1;
    else
        return 0;
}

int checkChangeDirectory(TokenList* Tokens){

    if(strcmp(Tokens->tokens[0],"cd")==0)
        return 1;
    else
        return 0;
}

/*
 *  Checks if the last command excecuted was the 'history' command.
 */
int checkHistoryCommand(TokenList* Tokens) {

    if(strcmp(Tokens->tokens[0],"history")==0)
        return 1;
    else
        return 0;
}

/*
 *  Checks if the last command excecuted was the '!!' command.
 */
int checkInvokeLastCommand(TokenList* Tokens) {

    if(strcmp(Tokens->tokens[0], "!!") == 0)
        return 1; // returns '1'/'true' if first token is '!!'.
    else
        return 0;
}

/*
 *  Checks if the last command excecuted was the '!<number>' command.
 */
int checkInvokeCommandNumber(TokenList* Tokens) {

    if(Tokens->tokens[0][0] == '!' && Tokens->tokens[0][1] != '-')
        return 1; // returns '1'/'true' if first character of first token is '!' (and second is not '-').

    return 0;
}

/*
 *  Checks if the last command excecuted was the '!-<number>' command.
 */
int checkInvokeCommandNegative(TokenList* Tokens) {

    if(Tokens->tokens[0][0] == '!' && Tokens->tokens[0][1] == '-')
        return 1; // returns '1'/'true' if first two characters of first token are '!-'.

    return 0;
}

int checkDisplayAliases(TokenList* Tokens){
    if((strcmp(Tokens->tokens[0],"alias")==0) && Tokens->tokenNumber==1)
        return 1;
    return 0;
}

int checkAliasCommand(TokenList* Tokens){
    if(strcmp(Tokens->tokens[0],"alias")==0)
        return 1;
    return 0;
}

int checkUnaliasCommand(TokenList* Tokens){
    if(strcmp(Tokens->tokens[0],"unalias")==0)
        return 1;
    return 0;
}

int checkAlias(TokenList* Tokens,HistoryAndAliases* HistoryAliases){
    for(int i=0;i<MAX_NO_ALIASES;i++){
        if(strcmp(Tokens->tokens[0],HistoryAliases->aliases[0][i])==0){
            printf("Alias found. Substituting...\n");
            return 1;
        }
    }
    return 0;
}

int getCommandID(TokenList* Tokens,HistoryAndAliases* HistoryAliases){

    if(checkNoTokens(Tokens))
        return 0;
    if(checkAlias(Tokens,HistoryAliases))
        return 1;
    if(checkExitCommand(Tokens))
        return 2;
    if(checkGetPath(Tokens))
        return 3;
    if(checkSetPath(Tokens))
        return 4;
    if(checkChangeDirectory(Tokens))
        return 5;
    if(checkHistoryCommand(Tokens))
        return 6;
    if(checkInvokeLastCommand(Tokens))
        return 7;
    if(checkInvokeCommandNumber(Tokens))
        return 8;
    if(checkInvokeCommandNegative(Tokens))
        return 9;
    if(checkDisplayAliases(Tokens))
        return 10;
    if(checkAliasCommand(Tokens))
        return 11;
    if(checkUnaliasCommand(Tokens))
        return 12;


    return 100;
}

//gets the value of the system path and outputs it
int getpath(TokenList* Tokens){

    if(Tokens->tokenNumber!=1){ //if more than one argument inputed display error
        printf("Syntax error: getpath expects no parameters;\n");
        return 1;
    } else {
        char* path; 
        path = getenv("PATH"); //getenv returns the value of the PATH, path is set to this
        if(path==NULL){
            printf("Error: PATH variable doesn't exist;\n");
            return 0;
        } else {
            printf("%s\n",path);
            return 1;
        }
    }
}

//sets the system path to the value inputed
int setpath(TokenList* Tokens){

    if(Tokens->tokenNumber!=2){ //if there are not 2 arguments (setpath and a value to change the path to) display error
        printf("Syntax error: setpath expects one parameter;\n");
        return 1;
    } else { //setenv sets the environment PATH to the 2nd argument inputed
        if(setenv("PATH",Tokens->tokens[1],1)==-1) {  //setenv returns -1 on error, so if setting the path returns -1 then error message displayed
            perror("Error: ");
            return 1;
        }
        else{
            printf("Path succesfully changed to: %s\n",Tokens->tokens[1]);
            return 1;
        }
    }

}

int externalCommand(TokenList* Tokens){

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
                perror(Tokens->tokens[0]);
            exit(0);
        }else {
            wait(NULL);
            return 1;
        }
    }

}

    //Checks if directory user has input is valid and changes accordingly  

int changeDirectory(TokenList* Tokens){
    char s[100];

    if(Tokens ->tokenNumber>2){
        printf("Syntax error: cd expects one parameter;\n");
        return 1;
    }

    if(Tokens-> tokenNumber == 1){
        if(chdir(getenv("HOME"))!=0){
            perror("Error");
            return 1;
        }else {
            printf("Current directory: %s\n", getcwd(s, 100));
            return 1;
        }
    }else {
        if(chdir(Tokens->tokens[1])!=0){
            perror(Tokens->tokens[1]);
            return 1;
        }else {
            printf("Current directory: %s\n", getcwd(s, 100));
            return 1;
        }
    }
}

/*
 *  Prints history in chronological order when the 'history' command is excecuted from circular history array.
 */
void print_history(HistoryAndAliases* HistoryAliases) {

    int no_commands = numberof_commands(HistoryAliases);
    int next_print;

    if(no_commands == SIZEOFHISTORY)
        next_print = HistoryAliases->next;
    else
        next_print = HistoryAliases->next - no_commands; // calculating the first command that should be printed from circular array.


    int count = 0;
    for(int i = 0; i < no_commands; i++) {

        printf("%d, ", i + 1);

        while(HistoryAliases->memArray[next_print][count] != '\0') {

            printf("%c", HistoryAliases->memArray[next_print][count]);
            count++;
        }
        next_print = (next_print + 1) % SIZEOFHISTORY; // iterating through circular array using remainder operation.
        count = 0;
    }
}

/*
 *  Adds a command to history and increments value of next available space in history to implement circular history array.
 */
void addto_history(char command[MAXBUFFSIZE], HistoryAndAliases* HistoryAliases) {

    int next = HistoryAliases->next;

    for(int i = 0; i < MAXBUFFSIZE; i++) { // copying raw command to history array.

        HistoryAliases->memArray[next][i] = *(command + i);
    }

    HistoryAliases->next = (next + 1) % SIZEOFHISTORY; // incrementing index of next availaible space in circular history array.
}

/*
 *  Function for invoking the last command from history when the '!!' command is excecuted.
 */
int invoke_last_command(HistoryAndAliases* HistoryAliases) {

    char last_command[MAXBUFFSIZE] = {0};
    int no_commands = numberof_commands(HistoryAliases);
    int index;
    int count = 0;

    if(is_history_empty(HistoryAliases)){
        printf("Error: no commands in history.\n");
        return CONTINUE_RUNNING; // print error message and continue running if history is empty.
    }

    else if(no_commands == SIZEOFHISTORY && HistoryAliases->next == 0)
        index = 19;

    else
        index = HistoryAliases->next - 1; // calculating index of last command in circular array.

    while(HistoryAliases->memArray[index][count] != '\0'){ // copying command from history for excecution.

        last_command[count] = HistoryAliases->memArray[index][count];
        count++;
    }
    
    return execute(last_command,HistoryAliases); //excecuting command from history and returning.
}

/*
 *  Function for invoking a command from history when the '!<number>' command is excecuted.
 */
int invoke_command_number(char* input,HistoryAndAliases* HistoryAliases) {

    char prev_command[MAXBUFFSIZE] = {0};
    int no_commands = numberof_commands(HistoryAliases);
    int num_arg = str_to_num(input+1); //converting number argument to an integer.
    int index;
    int count = 0;

    if(is_history_empty(HistoryAliases)){
        printf("Error: no commands in history.\n");
        return CONTINUE_RUNNING; // print error message and continue running if history is empty.
    }

    else if(no_commands == SIZEOFHISTORY)
        index = (HistoryAliases->next + num_arg) % SIZEOFHISTORY - 1;

    else
        index = HistoryAliases->next - no_commands + num_arg - 1; // calculating index in circular array of command to be invoked.


    if(index >= 0 && index < 20 && num_arg > 0 && num_arg <= no_commands) {

        while(HistoryAliases->memArray[index][count] != '\0'){

            prev_command[count] = HistoryAliases->memArray[index][count]; // copying command from history for excecution.
            count++;
        }
    }

    else {

        printf("Error: incorrect argument given.\n"); // error message if number argument given is out of range.
        return CONTINUE_RUNNING;
    }

    return execute(prev_command, HistoryAliases); //excecuting command from history and returning.
}

/*
 *  Function for invoking a command from history when the '!-<number>' command is excecuted.
 */
int invoke_command_negative(char* input,HistoryAndAliases* HistoryAliases) {

    char prev_command[MAXBUFFSIZE] = {0};
    int no_commands = numberof_commands(HistoryAliases);
    int num_arg = str_to_num(input+2); // converting number argument to an integer.
    int index;
    int count = 0;

    if(is_history_empty(HistoryAliases)){
        printf("Error: no commands in history.\n");
        return CONTINUE_RUNNING; // print error message and continue running if history is empty.
    }

    else if(no_commands == SIZEOFHISTORY)
        index = (HistoryAliases->next + no_commands - num_arg) % SIZEOFHISTORY;

    else
        index = HistoryAliases->next - num_arg; // calculating index in circular array of command to be invoked.
    
    if(index >= 0 && index < 20 && num_arg > 0 && num_arg <= no_commands) {

        while(HistoryAliases->memArray[index][count] != '\0'){

            prev_command[count] = HistoryAliases->memArray[index][count]; // copying commmand from history for excecution.
            count++;
        }
    }

    else {

        printf("Error: incorrect argument given.\n"); // error message if number argument given is out of range.
        return CONTINUE_RUNNING;
    }

    return execute(prev_command,HistoryAliases); //excecuting command from history and returning.
}

/*
 *  Returns the number of commands currently saved in history.
 */
int numberof_commands(HistoryAndAliases* HistoryAliases) {

    int size = 0;
    for (int i = 0; i < SIZEOFHISTORY; i++) {

        if(HistoryAliases->memArray[i][0] == '\0')
            continue;

        size++; // incrementing value for number of commands in history if first character of line is not null character:- '\0'.
    }

    return size;
}

/*
 *  Converts a string of digits to a natural number; returns -1 if string contains a character that is not a digit.
 */
int str_to_num(char* stringof_digits) {

    int total = 0;
    int number;

    int count = 0;
    while(*(stringof_digits + count) != '\0') {
        
        number = *(stringof_digits + count) - '0'; // converting character into integer.
        
        if(number < 0 || number > 9) // return negative number if charaacter is not a digit.
            return -1;
        
        total = total * 10 + number; // adding to accumumative value multiplied by ten.
        count++;
    }

    return number;
}

/*
 *  Returns 1 if history is currently empty, or returns 0 if history has at least 1 command saved.
 */
int is_history_empty(HistoryAndAliases* HistoryAliases) {

    int no_commands = numberof_commands(HistoryAliases);

    if(no_commands == 0 && HistoryAliases->next == 0)
        return 1;

    return 0;
}
/*
 *  Saves history to .hist_list file for continuous memory.
 */
void save_history(HistoryAndAliases* HistoryAliases) {

    int next_line;
    int num_commands = numberof_commands(HistoryAliases);
    FILE *fp;
    fp = fopen(HIST_FILE, "w"); // opening file for writing.

    printf("Saving history...\n");
    
    if(num_commands == SIZEOFHISTORY)
        next_line = HistoryAliases->next;
    else
        next_line = HistoryAliases->next - num_commands; // calculating the first command that should be saved from circular array.

    int count = 0;
    for(int i = 0; i < num_commands; i++) {

        while(HistoryAliases->memArray[next_line][count] != '\0') {

            fprintf(fp, "%c", HistoryAliases->memArray[next_line][count]); // saving history array to file one character at a time.
            count++;
        }
        next_line = (next_line + 1) % SIZEOFHISTORY; // iterating through circular array using remainder operation.
        count = 0;
    }
    
    printf("History successfully saved.\n");
}

/*
 *  Loads history from hidden .hist_list file to implement continuous history.
 */
void load_history(HistoryAndAliases* HistoryAliases) {

    char command[MAXBUFFSIZE];
    int count = 0;
    FILE *fp;
    fp = fopen(HIST_FILE, "r"); //opening file for reading.

    printf("Loading history...\n");
    
    if(fp == NULL){

        printf("Error loading history: could not find file %s.\n", HIST_FILE);
        return;
    }

    while(fgets(command, sizeof(command), fp)) {
        strcpy(HistoryAliases->memArray[count], command); // copying contents of file into history array line by line.
        count++;
    }
    HistoryAliases->next = count%SIZEOFHISTORY; // setting the index of the next available space in history.
    fclose(fp);
    
    printf("History successfully loaded.\n");
}

void displayAliases(HistoryAndAliases* HistoryAliases){
    int aliasesNumber = noOfAliases(HistoryAliases);
    if(aliasesNumber==0){
        printf("There are no aliases to display. This feels empty...\n");
        return;
    }
    int displayIndex = 1;
    for(int i=0;i<MAX_NO_ALIASES;i++)
        if(HistoryAliases->aliases[0][i][0]!='\0'){
            printf("%d. %s %s",displayIndex,HistoryAliases->aliases[0][i],HistoryAliases->aliases[1][i]);
            displayIndex++;
        }
}

void alias(TokenList* Tokens, HistoryAndAliases* HistoryAliases){

    if(Tokens->tokenNumber<3){
        printf("Error: Incorrect number of arguments\n");
        return;
    }

    if(strcmp(Tokens->tokens[1],"unalias")==0){
        printf("That's a bad idea. I'm sorry, I'm not going to let you do that\n");
        return;
    }

    int possibleLocation;
    possibleLocation = aliasCanBeAdded(HistoryAliases,Tokens->tokens[1]);
    if(possibleLocation!=-1){
            strcpy(HistoryAliases->aliases[0][possibleLocation],Tokens->tokens[1]);

            int parameterStart = 0;
            int parameterStop=0;
            while(parameterStop!=2){
                if(strchr(DELIMITERS,Tokens->command[parameterStart])!=NULL){
                    parameterStop++;
                }
                parameterStart++;
            }
            char letter = Tokens->command[parameterStart];
            int index = 0;
            char command[MAXBUFFSIZE];
            while(letter!='\0'){
                command[index] = letter;
                index++;
                letter = Tokens->command[parameterStart+index];
            }
            command[index+1] = '\0';
            for(int i=index;i<MAXBUFFSIZE;i++){
            command[i] = '\0';
            }
            strcpy(HistoryAliases->aliases[1][possibleLocation],command);

            printf("Alias successfully added\n");
            return;
    }else {
        printf("Error: Max number of aliases reached. Couldn't add aliases. Unalias something first\n");
        return;
    }
}

void unalias(TokenList* Tokens, HistoryAndAliases* HistoryAliases){
    if(Tokens->tokenNumber!=2){
        printf("Error: Incorrect number of parameters\n");
        return;
    }

    for(int i=0;i<MAX_NO_ALIASES;i++){
        if(strcmp(Tokens->tokens[1],HistoryAliases->aliases[0][i])==0){
            printf("Alias found. Unaliasing now...\n");
            HistoryAliases->aliases[0][i][0] = '\0';
            HistoryAliases->aliases[1][i][0] = '\0';
            printf("Alias succesfully deleted\n");
            return;
        }
    }

    printf("Error: No alias found with provided name\n");
}

int substituteAlias(TokenList* Tokens, HistoryAndAliases* HistoryAliases){
    char command[MAXBUFFSIZE] = "";
    for(int i=0;i<MAX_NO_ALIASES;i++){
        if(strcmp(Tokens->tokens[0],HistoryAliases->aliases[0][i])==0){
            if(HistoryAliases->noOfAliasCalls[i]!=0){
                printf("Error: alias loop detected. Terminating process\n");
                return CONTINUE_RUNNING;
            }
            strcpy(command,HistoryAliases->aliases[1][i]);
            HistoryAliases->noOfAliasCalls[i]++;
            break;
        }
    }
    char* space = " ";
    for(int i=1;i<Tokens->tokenNumber;i++){
        strcat(command,space);
        strcat(command,Tokens->tokens[i]);
    }

    return execute(command,HistoryAliases);
}

void saveAliases(HistoryAndAliases* HistoryAliases){
    FILE * fp;

    fp = fopen(ALIAS_FILE,"w");
    if(fp==NULL){
        perror("Error while saving aliases");
        return;
    }

    printf("Saving aliases...\n");

    for(int i=0;i<MAX_NO_ALIASES;i++){
        if(HistoryAliases->aliases[0][i][0]!='\0'){
            fprintf(fp,"%s",HistoryAliases->aliases[0][i]);
            fprintf(fp," ");
            fprintf(fp,"%s",HistoryAliases->aliases[1][i]);
        }
    }
    fclose(fp);
    printf("Aliases successfully saved;\n");
}

void loadAliases(HistoryAndAliases* HistoryAndAliases){
    FILE * fp;

    fp = fopen(ALIAS_FILE,"r");
    if(fp==NULL){
        perror("Error while loading aliases");
        return;
    }

    printf("Loading aliases...\n");

    char line[MAXBUFFSIZE];
    int aliasIndex = 0;
    while(fgets(line,MAXBUFFSIZE,fp)!=NULL && aliasIndex<MAX_NO_ALIASES) {
        char letter = line[0];
        int aliasCharIndex = 0;
        while(letter!=' '){
            HistoryAndAliases->aliases[0][aliasIndex][aliasCharIndex] = letter;
            aliasCharIndex++;
            letter = line[aliasCharIndex];
        }
        HistoryAndAliases->aliases[0][aliasIndex][aliasCharIndex] = '\0';
        aliasCharIndex++;

        int commandIndex=0;
        letter = line[aliasCharIndex+commandIndex];
        while(letter!='\n' && letter!=EOF){
            HistoryAndAliases->aliases[1][aliasIndex][commandIndex] = letter;
            commandIndex++;
            letter = line[aliasCharIndex+commandIndex];
        }
        HistoryAndAliases->aliases[1][aliasIndex][commandIndex] = '\n';
        HistoryAndAliases->aliases[1][aliasIndex][commandIndex+1] = '\0';
        aliasIndex++;
    }

    fclose(fp);
    printf("Aliases successfully loaded;\n");

}


int noOfAliases(HistoryAndAliases* HistoryAliases){
    int noOfAliases = 10;
    for(int i=0;i<MAX_NO_ALIASES;i++){
        if(HistoryAliases->aliases[0][i][0]=='\0')
            noOfAliases--;
    }
    return noOfAliases;
}

int aliasCanBeAdded(HistoryAndAliases* HistoryAliases, char* aliasName){

    for(int i=0;i<MAX_NO_ALIASES;i++){
        if(HistoryAliases->aliases[0][i][0]=='\0')
            return i;
        if(strcmp(HistoryAliases->aliases[0][i],aliasName)==0){
            printf("Warning: Previous alias will be overwritten\n");
            return i;
        }
    }

    return -1;
}

void resetNoAliasCalls(HistoryAndAliases* HistoryAlias){
    for(int i=0;i<MAX_NO_ALIASES;i++)
        HistoryAlias->noOfAliasCalls[i] = 0;
}
