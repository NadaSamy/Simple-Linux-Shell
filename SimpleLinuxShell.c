#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

//GLOBAL VARIABLES
int child_id;

//HANDLER FUNC
void sigchld_handler(int sig)
{
	//create file pointer
	FILE *fp;
	// string to be printed in file
	char data[]= "child process was terminated, ";
	// process ID to be printed in file when child is terminated
	char processID[]= "process ID = ";
	char newlineString[] = "\n";
	// open file
	fp = fopen("log.txt","a");
	if(fp == NULL)
	{
		//print error message
		printf("Error opening file.\n");
	}
	else
	{
		//print strings to file  "child process was terminated + child ID"
		fprintf(fp,"%s",data);
		fprintf(fp,"%s",processID);
		fprintf(fp,"%d",child_id);
		fprintf(fp,"%s",newlineString);
		
	}
	fclose(fp);
}


int main()
{
    char * command[10];
    char enteredCommand[50];
    char * token;
    char exitCommand[] = "exit", changeCommand[] = "cd", s[100];
    int i, flag = 0, execvp_return;
    
    // infinte loop to take multiple commands from user until user enters exit
    while(1)
    {
    	// wait for signal when child is terminated
    	signal(SIGCHLD, sigchld_handler);

    	flag = 0;
    	execvp_return = 0;
    	
    	// take command from user
    	printf("Enter Command: ");
        scanf("%[^\n]%*c",&enteredCommand);

        // if command is equal to "exit" then exit the program else continue
        if(strcmp(exitCommand,enteredCommand) == 0)
        {
        	exit(0);
        }
        else
        {
        	i = 0;
        	// extracting the first token
        	token = strtok(enteredCommand, " ");
        	// looping through the string to extract all the remanining tokens
        	while( token != NULL)
        	{
        		command[i] = token;
        		token = strtok(NULL, " ");
        		i++;
        	}
        	/*insert NULL at the end of the string*/
        	command[i] = NULL;
        	
        	/*create child process to perform command*/
        	pid_t p = fork();
        	if(p == -1)
        	{
        		// print error message
            		printf("Error Occurred in fork\n");
        	}
        	else if(p != 0)
        	{
        		/*PARENT PROCESS*/
        		
        		/*store child id for use in handler*/
        		child_id = p;
        		//signal(SIGCHLD, sigchld_handler);
        		
        		/*if first argument is "cd" then execute it*/
        		if(strcmp(command[0],"cd") == 0)
        		{
        			chdir(command[1]);
        		}
        		else
        		{
        			/*if last argument is "&"*/
				if(strcmp(command[i-1],"&") != 0)
				{
					/*wait on certain child*/
					waitpid(p,NULL,0);
				}
				
        		}
        		
        	}
        	else
        	{
        	        /*CHILD PROCESS*/
        	        
        	        /*check if first argument is "cd" then flag == 0*/
        	        flag = strcmp(command[0],"cd");
        	        /*check if last argument is "&" then remove it*/
        	        if(strcmp(command[i-1],"&") == 0)
        	        {
        	        	command[i-1] = NULL;
        	        	
        	        }
        	        // see return value of execvp
        	        execvp_return = execvp(command[0],command);
        	        if(execvp_return == -1)
        	        {
        	        	/*if 1st argument is not "cd" print error of execvp*/
        	        	if(flag != 0)
        	        	{
        	        		printf("Error, invalid command.\n");
        	        	}
        	        	exit(0);
        	        }

        	}
        	
        }
        
    }
    
	return 0;
}
