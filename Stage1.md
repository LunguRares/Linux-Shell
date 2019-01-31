# Stage 1: Prompt user, and Read and parse user input, Exit shelland Initialise the working directory

## Display prompt

In  most shells,the  prompt  can  be  configured  by  the  user.  Sometimes  it  is  just  a character,  e.g.  ‘>’  or  ‘$’,  sometimes  something  more  sophisticated,  e.g.  ‘current working directory>’, or include the username, etc. 

For your shell just keep it simple to a fewcharacters.

The prompt is printed on the screen and the user is expected to type in the next command next to it.

## Read and parse user input

The  user  can  type  pretty  much  anything  next  to  the  prompt.  However,the inputis finished when the user types <enter>. You can use the gets() function to read a whole line of text from thestandard input. By making the simplifying assumption that each command is at most 512 characters long, then  you know that what  you will get back won’t be more than that many characters long.Note that you can also use the fgets() function,  which  is  the  file  reading  version  of  gets().  Note  that  file  versions  of input/output functions can be used even when  you use the standard input, output and error, by using the stdin, stdout and stderr file po
inters respectively.

Parsing the input line involves breaking it into parts(tokens)based on white space (i.e. ‘  ‘ space or ‘\t’ tab), ‘|’, ‘>’,‘<’, ‘&’, and ‘;’(these symbols have a special meaning in Unix-like shells as they are used to provide output of one program as input to another, or to direct the output to a particular file, or direct the input to a particular file, or provide background execution of commands, or separate multiple commands respectively). In order to do so,you could use the strtok() function. The function uses a set of delimiters to tokenize a string (i.e. in your case to take thewhole user input lineand break into tokens). The function is a bit strange as it remembersprevious invocations. So, by using NULL as the string youare basically saying to it to getthe next token from the last used string, particularly useful for iterating through the string to retrieve all its tokens.

## Continuous reading and processing of user input and exiting the shell

Your  shell  should  continuously  be  prompting  the  user  for  input  and  reading  and processing  any  user inputuntil the user types ‘exit’ (strcmp()  function  can  be  handy here) or‘<ctrl>-D’(both  are  required).  Note  that  in  the  latter  case,  we  are  in  fact closing the standard input, which will give an error when trying to read from it. So, you need to use this as a way of exiting the loop.

# Stage 1: Testing

To test that your shell works correctly you should be able to prompt the user to enter a line of input and then print the list of tokens one at a time. It is probably a good idea to put each printed token in a single line and enclose it within a particular pair of characters(e.g. quotations)in order to be certain about the characters included in the token.

Remember to test your code extensively, i.e. with as many words as possible. Don’t forget to also test what happens when you exceed the character limitor when the line is empty (i.e. just enter typed in, by the way in this case there are no tokens). In general,you should ensure that your program works correctly no matter what the user types in. This is very important for a shell as it should never crash.

You should also check that your shell keeps on working correctly no matter how many lines the user inputs.

You should finally check that your shell exits when the user types ‘exit’or‘<ctrl>-D’.
Be careful that no segmentation faults happen on exit.