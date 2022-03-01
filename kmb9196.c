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
// compile with gcc kmb9196.c -o msh -std=c99

int pidin = 0;
pid_t history[MAXPIDS];
char *command_history[MAXPIDS];
int commandcount = 0;
int command = 0;

// functional pidhistory displays the forked PIDs satisfies requirement 11
void pidhistory()
{
    // set i to the most recent pidin value to fetch the most recent index
    int i = pidin;
    // for loop iterates through the pidhistory array and prints all pids that have been assigned a value via a forked process in the main while loop
    // because we declared the array to all have values of -1, we can use that value to determine whether or not a specific index in the command history contains a valid PID or not
    for (int j = 0, k = 0; j < MAXPIDS; j++)
    {
        
        if (history[i] != -1)
        {
            
            printf("%d: %d\n", k++, history[i]);
        }
        i++;
        // if the index i surpasses the MAX amount of PIDS, set it back to 0 so that it replaces the oldest PID in the array to display
        if (i > MAXPIDS-1)
        {
            i = 0;
        }
    }
}

// functional commandhistory displayes the previous commands that were entered by the user
// satisfies requirement 12 partially as it does not take into account the '!' case
void commandhistory()
{
    int i = commandcount;
    //iterate through the commandhistory and set the index back to 0 if it surpasses the MAX of 15 commands so that most recent commands replace the old commands in the array
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
    char * newcom = (char*)malloc(MAXCOMMANDSIZE);
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
        // the command value is set to -1 if the !n command is not entered (because an array index of -1 is not possible or used in this case)
        // if the !n command is not used, iterate through the while loop normally, taking an input from the user
        if (command == -1)
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
            // the command being a positive number means that the !n command was used and therefore we copy the newcom string into command_string to take it in as input
            // after this else statement, go through the loop normally with the newcom/command_string acting as the input
            strncpy(command_string, newcom, MAXCOMMANDSIZE);
            //printf("%s is executed\n", command_string);
        }
        

        /* Parse input */
        char *token[MAXARGS];

        int   token_count = 0;                                 
                                                            
        // Pointer to point to the token
        // parsed by strsep
        char *argument_ptr;                                         
                                                            
        char *working_string  = strdup( command_string );     
        command_history[commandcount] = strdup(command_string);
        commandcount++;
        // we log the command into the command_history via strdup so that the memory is allocated for the index in command_history to avoid any seg faults
        if (commandcount > MAXPIDS-1)
        {
            commandcount = 0;
        }
        // if the amount of commands in the history surpasses the MAX amount of of commands required (15), we set the count back to 0 so that it replaces old commands via FILO first in last out

        // we are going to move the working_string pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *head_ptr = working_string;
        command = -1;
        // set the command value back to -1 because the !n command is finished and the command value is not needed anymore
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

        int token_index  = 0;
        // for( token_index = 0; token_index < token_count; token_index ++ ) 
        // {
        // printf("token[%d] = %s\n", token_index, token[token_index] );  
        // }
        
        // If the user issuies a return without any characters, token[0] is NULL: describes a blank line entered as an input
        // continue through the loop if there was no input
        // satisfies requirement 6
        if (token[0] == NULL) 
        {
            commandcount--;
            command_history[commandcount] = NULL;
            continue;
        }
        else
        { 
            // satisfies requirement 5: straightforward string comparison, if the first argument in the input is either quit or exit
            // we will free the string buffer and string array that was allocated and return with a status zero
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
                commandhistory();
                
            }
            else if (!strcmp(token[0], "cd"))
            {
                //printf("Do CD\n");
                chdir(token[1]);
                // we use the index 1 in token array to fetch the directory that is desired by the user
                // ability to use cd satisfies requirement 10
                
            }
            else if (token[0][0] == 33) // character '!' has an ASCII value of 33, compare the first character of the first argument with '!'
            {
                // if statement allows for the functionality of the !n command comppleting and satisfying the requirement 12
                //printf("Do !\n");
                // for character pointer 'in' increment in the first token so that we get rid of the '!' character and fetch the integer that was entered by the user 
                char* in = ++token[0];
                //printf("%s\n", in);
                command = atoi(in);
                // set the command integer to the integer input after the '!' that was entered by the user
                //printf("%d\n", command); //debug line to make sure we are getting the right value
                // for this if statement, we are making sure that the command index meets all boundaries, if it fails, then command not in history
                if(command>commandcount || command > MAXPIDS-1 || command < 0)
                {
                    printf("Command not in history.\n");
                }
                else
                {
                    // if it is still within the boundaries, we duplicate the corresponding comand in the command history array into newcom (newcommand)
                    // newcom is then used in the next iteration of the while loop because command value != -1
                    newcom = strdup(command_history[command]);
                    //printf("%s happens\n", newcom); // debug line to make sure we fetched the right value
                }
            }
            else
            {
                //  if the command entered by the user is located within the path
                //  we fork a new process via a new pid_t 'new' and add that new process into the pidhistory array
                pid_t new = fork();
                history[pidin] = new;
                
                // A new process is forked and is later compared to 0 to determine if the process is a child process
                
                
                if (new == 0) //is this a child process?
                {
                    // use of execvp satisfies requirement 8 via its ability to search in path
                    int ret = execvp(token[0], &token[0]); //Yes it is a child process, execute the first argument by looking for the right executable within the path
                    //printf("pid = %d\n", pidin);
                    // The if statement satisfies requirement 2 as it looks to see if the execvp function fails... returning the integer -1
                    // The terminal then prompts that the command does not exist and continues to loop over the outer loop to keep displaying >msh awaiting other inputs
                    if(ret == -1) // execvp returns -1 if something went wrong 
                    {
                        
                        printf("%s: Command not found.\n", token[0]);  //There's no corresponding executable command in the path? 
                        // Display that the command is not found and continue to loop over the while loop
                        _exit(EXIT_FAILURE); //Because the exec fails via invalid command, _exit is called so that the child process does not interfere with the parent process
                    }
                }
                else
                {
                    // wait until the child process finishes terminating to avoid having zombie processes
                    // requirement 9 satisfies through the use of fork(), wait(), and execvp()
                    int status;
                    wait(&status);
                }
                // increment the pidindex and make sure it doesn't surpass the max amount of pids for the array by setting it back to 0 if it surpasses that MAXPID
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