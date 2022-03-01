/*
Name: Karl Dylan Baes
ID: 1001839195
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#define WHITESPACE " \t\n"
#define MAXARGS 10
#define MAXPIDS 15
#define MAXCOMMANDSIZE 255
// MAX values are defined for concrete required values of max arguments and max pids via requirements 7 and 11

int pidin = 0;
pid_t history[MAXPIDS];
char *command_history[MAXPIDS];
int commandcount = 0;
int command = 0;
void pidhistory()
{
    int i = pidin;
    for (int j = 0, k = 0; j < MAXPIDS; j++)
    {
        
        if (history[i] != -1)
        {
            
            printf("%d: %d\n", k++, history[i]);
        }
        i++;
        if (i > MAXPIDS-1)
        {
            i = 0;
        }
    }
}

void commandHistory()
{
    int i = commandcount;
    for (int j = 0, k = 0; j < MAXPIDS; j++)
    {
        if (command_history[i] != NULL)
        {
            printf("%d: %s", k++, command_history[i]);
        }
        i++;
        if (i > MAXPIDS-1)
        {
            i = 0;
        }
        
    }
}

int main()
{

    char * command_string = (char*) malloc( MAXCOMMANDSIZE );

    // Assigning all of the vacant pidhistory indexes to -1 so that we are able to distinguish between what is vacant and what contains a PID value in the pidhistory call
    // Also allocating memory for the string array that whill hold the commands
    
    for (int i = 0; i < MAXPIDS; i++)
    {
        history[i] = -1;
        command_history[i] = NULL;        
    }
    
    // Outer while loop satisfies requirement 1 as it prints "msh>" and waits for an input to be given and executes the input
    // Also satisfies requirement 4: after taking an input: ANY input unless it is quit or exit, it will continue to execute and loop
    while( 1 ) 
    {
        // Print out the msh prompt
        if (command == 0)
        {
            printf ("msh> ");

            // Read the command from the commandline.  The
            // maximum command that will be read is MAXCOMMANDSIZE
            // This while command will wait here until the user
            // inputs something since fgets returns NULL when there
            // is no input
            while( !fgets (command_string, MAXCOMMANDSIZE, stdin) );
        }
        else
        {
            command_string = command_history[command];
        }
        

        /* Parse input */
        char *token[MAXARGS];

        int   token_count = 0;                                 
                                                            
        // Pointer to point to the token
        // parsed by strsep
        char *argument_ptr;                                         
                                                            
        char *working_string  = strdup( command_string );     
        command_history[commandcount] = strdup(command_string);
        //command_history[commandcount][strlen(command_history[commandcount])-1] = '\0';
        commandcount++;
        if (commandcount > MAXPIDS-1)
        {
            commandcount = 0;
        }
        // we are going to move the working_string pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *head_ptr = working_string;
        command = 0;
        // Tokenize the input strings with whitespace used as the delimiter
        while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
                (token_count<MAXARGS))
        {
        token[token_count] = strndup( argument_ptr, MAXCOMMANDSIZE );
        if( strlen( token[token_count] ) == 0 )
        {
            token[token_count] = NULL;
        }
            token_count++;
        }

        // Now print the tokenized input as a debug check
        // \TODO Remove this code and replace with your shell functionality

        int token_index  = 0;
        for( token_index = 0; token_index < token_count; token_index ++ ) 
        {
        printf("token[%d] = %s\n", token_index, token[token_index] );  
        }
        
        // If the user issuies a return without any characters, token[0] is NULL
        // continue through the loop if there was no input
        if (token[0] == NULL) 
        {
            commandcount--;
            command_history[commandcount] = NULL;
            continue;
        }
        else
        { 
            // satisfies requirement 5: straightforward string comparison, if the first argument in the input is either quit or exit
            // we will free the string buffer that was allocated and return with a status zero
            if (!strcmp(token[0], "quit") || !strcmp(token[0], "exit"))
            {
                for (int i = 0; i < MAXPIDS; i++)
                {
                    free(command_history[i]);
                }
                free(working_string);
                return 0;
            }
            else if (!strcmp(token[0], "pidhistory"))
            {
                pidhistory();
                
            }
            else if (!strcmp(token[0], "history"))
            {
                commandHistory();
                
            }
            else if (!strcmp(token[0], "cd"))
            {
                //printf("Do CD\n");
                chdir(token[1]);
                
            }
            else if (token[0][0] == 33)
            {
                //printf("Do !\n");
                char* in = ++token[0];
                //printf("%s\n", in);
                command = atoi(in);
                printf("%d\n", command);
                if(command>commandcount || command > MAXPIDS-1 || command < 0)
                {
                    printf("Command not in history.\n");
                }
                else
                {
                    char* newcom = command_history[command];
                    printf("%s happens\n", newcom);
                }
            }
            else
            {
                pid_t new = fork();
                history[pidin] = new;
                
                // A new process is forked and is later compared to 0 to find if the process is a child process
                
                
                if (new == 0) //is this a child process?
                {
                    int ret = execvp(token[0], &token[0]); //Yes it is a child process, execute the first argument by looking for the right executable within the path
                    //printf("pid = %d\n", pidin);
                    // The if statement satisfies requirement 2 as it looks to see if the execvp function fails... returning the integer -1
                    // The terminal then prompts that the command does not exist and continues to loop over the outer loop to keep displaying >msh awaiting other inputs
                    if(ret == -1)
                    {
                        
                        printf("%s: Command not found.\n", token[0]);  //There's no corresponding executable command in the path? Display that the command is not found and continue to loop over the while loop
                        _exit(EXIT_FAILURE); //Because the exec fails via invalid command, _exit is called so that the child process does not interfere with the parent process
                    }
                }
                else
                {
                    // wait until the child process finishes terminating to avoid having zombie processes
                    int status;
                    wait(&status);
                }
                pidin++;
                if (pidin > MAXPIDS-1)
                {
                    pidin = 0;
                }
            }
        }
        free( head_ptr );

    }
    free(command_string);
    return 0;
}