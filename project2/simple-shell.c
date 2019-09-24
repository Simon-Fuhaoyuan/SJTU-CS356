/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

char *args[MAX_LINE/2 + 1];     /* command line (of 80) has max of 40 arguments */
char arg[MAX_LINE/2 + 1][10];
char hisBuf[MAX_LINE/2 + 1][10];
int should_run = 1;
char buffer;
int argsNum = 0;
int hisArgsNum = 0;
bool needPipe = false;
char *pipes[MAX_LINE/2 + 1];
int pipeArgsNum = 0;
int ifWait = true;

int main(void)
{
    	while (should_run)
	{
		pid_t pid;
		//if(pid == 0) printf("pid is 0\n");
		for (int i = 0; i < MAX_LINE/2 + 1; ++i) //clear buffer
			args[i] = NULL;
                
		printf("osh>");
                fflush(stdout);
		if(!ifWait) wait(NULL);
                argsNum = 0;
		pipeArgsNum = 0;
		ifWait = true;
		needPipe = false;

                while (scanf("%s", arg[argsNum])) // get the instructions
		{		
	                scanf("%c", &buffer);
			args[argsNum] = arg[argsNum];	
	       		argsNum++;
		       	if(buffer == '\n') break; // get '\n' and finish
		}

		if (strcmp(args[0], "!!") != 0) // not "!!", update history
		{
			for (int i = 0; i < argsNum; i++)
			{
				strcpy(hisBuf[i], arg[i]);
			}
			hisArgsNum = argsNum;
		}
		else if (strlen(hisBuf[0]) == 0) // is "!!", but no history
		{
			printf("NO commands in history\n");
			continue;
		}
		else // get history
		{
			for (int i = 0; i < hisArgsNum; ++i)
			{
				args[i] = hisBuf[i];
				printf("%s ", args[i]);
			}
			printf("\n");
			argsNum = hisArgsNum;
		}

		if (strcmp(args[0], "exit") == 0) return 0;
	
		if (strcmp(args[argsNum - 1], "&") == 0) 
		{
			ifWait = false; // parent process need to wait
			args[argsNum - 1] = NULL; // delete "&"
			argsNum--;
		}
		//if(ifWait) printf("need wait\n");
		for (int i = 0; i < argsNum; ++i)
		{
			if (strcmp(args[i], "|") == 0)
			{
				needPipe = true;
				args[i] = NULL;
				//printf("set needPipe!\n");
				for (int j = i + 1; j < argsNum; ++j)
				{
					pipes[pipeArgsNum] = arg[j];
					args[j] = NULL;
					pipeArgsNum++;
				}
				argsNum -= pipeArgsNum;
				break;
			}
		}

		/*if (needPipe)
			for (int i = 0; i < pipeArgsNum; ++i)
				printf("%s\n", pipes[i]);
*/
		pid = fork();
		if (pid < 0)
		{
	                fprintf(stderr, "Fork Failed");
	                return 1;
	        }
		else if (pid == 0)
		{
			//printf("Child:\n");
			
			/*for (int j = 0; j < argsNum; ++j)
	                        printf("%s\n", args[j]);
			*/
			if (argsNum >= 3 && strcmp(args[argsNum - 2], ">") == 0)
			{
				//printf("redirection\t%s\n", args[argsNum - 1]);
				int fd;
				fd = open(args[argsNum - 1],O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
				dup2(fd, STDOUT_FILENO);
				args[argsNum - 2] = NULL;
				args[argsNum - 1] = NULL;
				argsNum -= 2;
			}
			if (argsNum >= 3 && strcmp(args[argsNum - 2], "<") == 0)
			{
				int fd = open(args[argsNum - 1],O_RDONLY);
				dup2(fd, STDIN_FILENO);
				args[argsNum - 2] = NULL;
                                args[argsNum - 1] = NULL;
                                argsNum -= 2;
			}

			//if (needPipe) printf("testPipe\n");

			if (needPipe)
			{
				printf("IN PIPE\n");
				pid_t pid2;
				pid2 = fork();
				if (pid2 < 0)
				{
					fprintf(stderr, "Fork Failed");
		                        return 1;
				}
				else if (pid2 == 0)
				{
					int fd = open("pipe.txt", O_RDONLY);
					dup2(fd, STDIN_FILENO);
					execvp(pipes[0], pipes);
					exit(0);
				}
				else
				{
					int fd = open("pipe.txt",O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
					dup2(fd, STDOUT_FILENO);
					execvp(args[0], args);
					wait(NULL);
				}
			}
			else execvp(args[0], args);
			//printf("child!!!\n");
			exit(0);
		}
		else
		{
			//if(ifWait) printf("needWait\n");
			if (ifWait) 
				wait(NULL);
    			//printf("Parent Complete\n");
			//if (!ifWait)
			//	wait(NULL);
			
		}
	}
	return 0;
}
