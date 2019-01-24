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