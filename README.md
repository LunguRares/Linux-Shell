Exercise description

The aim of the exercise is to develop a simple OS shell in C for a Unix-type system.
So, you must work in a Linux environment for this exercise.

Your shell should support the following functionality:

1.Execution of external system programs including their parameters.

2.The following built-in commands:
    a.cd –change working directory
    b.getpath–print system path
    c.setpath–set system path
    d.history –print history contents (numbered list of commands in history including their parameters in ascending order from least to most recent)
    e.!! –invoke  the  last  command  from  history  (e.g.  if  your  last  command was ‘ls –lF’ then ‘!!’ should execute ‘ls –lF’)
    f.!<no> -invoke command with number <no> from history(e.g. !5 will execute the command with number 5 from the history)
    g.!-<no> -invoke  the  command  with  number  the  number  of  the  current command minus <no> (e.g. !-3 if the current command number is 5 will execute the command with number 2, or !-1 will execute again the last command)
    h.alias –print all set aliases(alias plus aliased command)
    i.alias <name> <command> -alias nameto be the command. Note that the command may  also include any  number of parameters, while(any number  of) command  parametersshould  work  correctly  with  aliasing(e.g. if I alias la to be ls –la then when I type la . the shell should execute ls –la .).Note also that aliasing should also work correctly with history (e.g. !5 will execute the command with number 5 from the historyif this command is an alias like the laabove then !5 will execute ls –la).In the enhanced  form  of  the  alias,  it  should  be  possible  to  alias history invocations (e.g. if I alias ‘five’to be !5 then when I type ‘five’the shell should execute the command with number 5 from the history). It should also be possible to alias aliases(e.g. If I alias l to be lsand then I alias la to be l–a, then when I type la theshell should execute ls –a).
    j.unalias<command> -remove any associated alias
    
3.Persistent history of user commands(save history in a file and load it when you run the shell again)

4.Persistent aliases (save aliases in a file and load it when you run the shell again)


Shell Execution Outline

    The operation of the shell should be as follows:
    
    Find the user home directory from the environment
    Set current working directory to user home directory
    Save the currentpath
    Load history
    Load aliases
    Do while shell has not terminated
        Display prompt
        Read and parse user input
        While  the  command  is  a  history  invocation  or  alias  then  replace  it  with  the appropriate command from history or the aliased command respectively
        If command is built - in invoke appropriate function
        Else execute command as an external process
    End while
    Save history
    Save aliases
    Restore original path
    Exit