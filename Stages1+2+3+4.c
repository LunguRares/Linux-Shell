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
//#define PATH_MAX 256

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

int getCommandID(TokenList* Tokens);
int checkNoTokens(TokenList* Tokens);
int checkExitCommand(TokenList* Tokens);
int checkGetPath(TokenList* Tokens);
int checkSetPath(TokenList* Tokens);
int checkChangeDirectory(TokenList* Tokens);

int aliasOrExternalCommand(TokenList* Tokens);
int getpath(TokenList* Tokens);
int setpath(TokenList* Tokens);
int changeDirectory(TokenList* Tokens);

int setenv(const char *name, const char *value, int overwrite);

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
    printf("Restored PATH to %s\n",path);
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
	
	case 4:
	    return changeDirectory(&Tokens);

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

int checkChangeDirectory(TokenList* Tokens){

    if(strcmp(Tokens->tokens[0],"cd")==0)     //The user entered an exit command, return 0 to terminate the shell loop
        return 1;
    else
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
    if(checkChangeDirectory(Tokens))
	return 4;

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

int changeDirectory(TokenList* Tokens){
	char s[100];
	
    if(Tokens ->tokenNumber>2){
	printf("Syntax error: Incorrect number of arguments;\n");
	return 1;
    }
      
	if(Tokens-> tokenNumber == 1)
		{
			if(chdir(getenv("HOME"))!=0)
			{
				perror("Error");
				return 1;
			}
			else
			{
				printf("Current directory: %s\n", getcwd(s, 100));
				return 1;
			}
		}

	else{
			
	
		
			if(chdir(Tokens->tokens[1])!=0)
			{
				perror("Error");
				return 1;
			}
			else
			{
				printf("Current directory: %s\n", getcwd(s, 100));
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