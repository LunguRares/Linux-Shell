# Stage 2: Execute external commands–10 marks

In Unix-type systems,the fork() function can be used to create a new process (a child process) that is an exact duplicate of the current one. Both processes parent (original) and  child  (new)  will continue  execution  from  the  point  immediately  after  the  fork function call.

In order to allow the child process to execute something different than the code of the parent  process,  we  need  to  use  one  of  the  exec()  system  call  variants.  One  of  the 
parameters of the exec variants is the program to be executed, while another includes the parameters with which the program will be invoked.

In the case of your shell, after fork() the parent process waits (wait() function) for the child process to complete, and then continues its execution.

See Figure 3.10 in the textbook or the lecture slides for an example.

In Unix-type  systems  like  Linux,  when  a  user  logs  in  or  starts  a  terminal  a  shell  is loaded. As part of the initializationof the shell,a number of system properties are set. These  properties  make  up  the  system  environment.  You  can  see  what  the  current environment is by typing ‘setenv’.

For this stage we are only interested in one thing from the environment the PATH –the set  of  directories  (colon  separated),  where  the  shell  searches  for  external  executable programs.

The key point for this stageis to choose an appropriate variant of exec() that will take into  account  the  PATH  environment  parameter.  Read  also  the  manual  very  carefully about how to handle external program parameters.Remember that we should be able to support any number of command parameters.

Do  not  also  forget  to  handle  the  situation  where  an  invalid  program  was  entered,  in which case you should provide the user with an appropriate error message

## Parsing simplification

In order to simplify the parsing so that built-in commands canbe easily added, it is a good idea to generate a single string array that includes all the tokens of the user input line.  This  way  you  can  parse  the  user  input 
line,  in  the  same  way,all  the  time irrespective  of  whether  it  contains  built-in  or  external  commands  or  the  number  of tokens it contains!

To make your life even easier you can use a fixed size array, thus putting a limit on the number of tokens accepted. Fifty tokens will be a reasonable number here.

# Stage 2: Testing

First, test that  the  code  you  have  added  to  your  shell  has  not  broken  the  previously working code (i.e. repeat the same tests that you carried out for stage 1). They should still work. In software engineering,this is referred to as regression testing.

To test that the new functionality in yourshell works you should invoke a number of programs (system or otherwise) and see them execute. 

In order to check that you are handling correctly any number of parameters a very good system program to use is ‘ls’ which provides a list of the contents of a directory. The good thing about itis that it takesa number of flags that change the formatting of the output and it can also take a list of directory/files (of any length) and list their contents or them.

You  can  check  whether  your  program  correctly  handles  paths  (i.e.  only  looks  for executables in the directories specified by the path) by invoking your shell from within your shell! Typically the current directory (i.e. ‘.’) is in the path so you should be able to do this if you are running your shell from the directory the executable resides in, but not from anotherdirectory (e.g. MyShell/shell and then type ‘shell’at the prompt).

Do not forget to check what happens when an invalid program is provided by the user