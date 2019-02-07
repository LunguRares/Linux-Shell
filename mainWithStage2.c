#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <errno.h>   
#include <sys/wait.h>

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


int callExternal(char *,TokenList *Tokens);

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

//filepath here needs to be set inside the program, will be given by another part of the program

	char *filepath={"/home/cgb17145/Documents/Study/"};
	if(!callExternal(filepath, Tokens))
		return 1;
	

    printf("Command not found \n");
    return 1;
}

//Call External Function
//Function takes in two strings, filename and filepath, and then forks, creates a child process, this process executes the specified file adn returns, there is a variety of error handling to deal with incorrect input or errors in the system.

int callExternal(char *filepath, TokenList *Tokens){ 

char fullpath[80];
strcpy(fullpath,filepath);
strcat(fullpath,Tokens->tokens[0]);

char *args[Tokens->tokenNumber+1];
char **ptr=args;

for ( int i=0;i<(Tokens->tokenNumber);i++){
	ptr[i]=Tokens->tokens[i];
}

ptr[(Tokens->tokenNumber)+1]='\0';


//Incase of Error uncomment to print filename, path and argument to check correct combination

printf("%s \n%s \n", filepath, fullpath);


   pid_t  pid; 
   int ret = 1; 
   int status; 
   pid = fork(); 
   if (pid == -1){ 
  
     
      printf("Error 001: Fork impossible\n"); 
      exit(EXIT_FAILURE); 
   } 
   else if (pid == 0){ 
    
        execvp(fullpath,args); 

	printf("Error 002: program failed wrong name supplied\n");
	printf("arg[0]-%s  arg[1]-%s \n", args[0], args[1]);
      
      return 0; 
   } 
   else{ 
    
        if (waitpid(pid, &status, 0) > 0) { 
              
            if (WIFEXITED(status) && !WEXITSTATUS(status))  
              printf("new process execution successfull\n"); 
              
            else if (WIFEXITED(status) && WEXITSTATUS(status)) { 
                if (WEXITSTATUS(status) == 127) { 
                    printf("Error 003: execv failed\n"); 
                } 
                else 
                    printf(" Error 004/5: program terminated normally,"
                       " but returned a non-zero status 004\n");                 
            } 
            else 
               printf("program didn't terminate normally 005\n");             
        }  
        else { 
           // waitpid() failed 
           printf("Error 006: waitpid() failed\n"); 
        } 
      return 0; 
   } 
   return 0; 
} 
