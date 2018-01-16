#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>

#define BUFFER_SIZE 2048
#define MAX_ARGUMENTS 512
#define MAX 100

//Function Prototypes
int checkSignal(int *stats);
int seperateCommand(char **arguments,char *comamndLine);
int redirect(char **arguments, int inputFile, bool background);
int execute(char **arguments, pid_t *childProcesses, int *stats, int number);
int start(char **arguments, pid_t *childProcesses, int *stats, int number);

void checkForBackgroundProcess(pid_t *childProcesses);
void makeArgumentsNULL(char ** arguments);

int seperateCommand(char **arguments,char *comamndLine)
{
	int index = 0;

	//taking the whole command from the user and 
	//seperating it by space. New line gets rid of the space 
	//to be able to use the whole word
	arguments[index] = strtok(comamndLine, " \n");

	while(arguments[index] != NULL)
	{	
		index++;
		arguments[index] = strtok(NULL, " \n");
	}
	
	index--;
	return index;
}

int start(char **arguments, pid_t *childProcesses, int *stats, int number)
{
	
	//Fork PID and wait pid
	pid_t pid; 
	pid_t wait_pid;   									
	
	//declaring execution variables
	int status;																						
	int i;
	int file;	
	int inputFile = 2;			
	bool background = false;

	pid = fork();
	 
	//checking the arfguments after parsing the 
	//whole command 
	if(strcmp(arguments[number], "&") == 0)
	{

		//make backgrounf processing true
		background = true;
		//have to make the second argument NULL because
		//for some reason it reads the others in there
		arguments[2] = NULL;
		number--;
	
		for(i = 0; i < MAX; i++)
		{
			if(childProcesses[i] == 0)
			{
				//append the child processes in the 
				//array
				childProcesses[i] = pid;
			}
		}
	}
	//if we are in background processes and the second argument
	//is an input or outpur arguments we identify them with 
	//inputfile beign zero or 1
	if(background == true || arguments[1] != NULL)
	{

		if(strcmp(arguments[1], "<") == 0) {
			
			inputFile = 0;

		}else if(strcmp(arguments[1], ">") == 0) {

			inputFile = 1;
		}
					
		//call the functon to for redirection of the input or output
		file = redirect(arguments, inputFile, background);
	}
	
	//we are in the childprocesses 
	if(pid == 0)
	{
		//we declare the child processes since our child 
		//utlizes signals
		if(background == true){
			signal(SIGINT, SIG_IGN);

		}else {

			signal(SIGINT, SIG_DFL);
		}

		//do the refirection stdin for input or output
		if(inputFile == 1 || inputFile == 0){
			
			if(inputFile == 1) {
				dup2(file, 1);

			} else if (inputFile == 0){
				dup2(file, 0);
			}
		}

		//if the argument is a command file
		//else print error
		if(execvp(arguments[0], arguments) == -1)
		{
			printf("Command not valid\n");
			exit(1);
		}
	}

	//error if not in parent or child
	else if(pid < 0) {

		printf("Error Forking\n");
	}

	//We are in the parent
	else if(pid != 0) 
	{

		if(background == false)
		{
			//loop while all the processes have finishes
			do
			{
				wait_pid = waitpid(pid, &status, WUNTRACED);

			}while( !WIFSIGNALED(status) && !WIFEXITED(status));

			//give it the last status
			*stats = status;		
	
			//print the latest status
			if(WIFSIGNALED(status))			
			{
				printf("Terminated by Signal %d\n", WTERMSIG(status));
				fflush(stdout);	
			}
		}
		
		//printing the background PID
		else
		{
			printf("Background Pid is: %d\n", pid);
			fflush(stdout);
		}
	}

	return 1;
}	


int execute(char **arguments, pid_t *childProcesses, int *stats, int number)
{

	int i;
	int status;
	
	//if the first argument is a # or empty
	if(arguments[0] == NULL || strcmp(arguments[0], "#") == 0){

		return 1;

	//if command is exit loop through all the 
	//child processes and kill them 
	} else if(strcmp(arguments[0], "exit") == 0) {

		for(i = 0; i < MAX; i++)
		{
			if(childProcesses[i] > 0) {
				kill(childProcesses[i], SIGKILL);
			}
		}

		return 0;

	//if argument is status it checks the status of the 
	//exit value 
	} else if(strcmp(arguments[0], "status") == 0) {

		status = *stats;

		if(WIFEXITED(status)){
			printf("exit value %d\n", WEXITSTATUS(status));

		}else if(WIFSIGNALED(status)) {
			printf("terminated by signal %d\n", WTERMSIG(status));
		}	
	
		return 1;

	//the command is cd it redirects to the home or the
	//directory entered
	} else if(strcmp(arguments[0], "cd") == 0) {

		if(arguments[1] == NULL) {

			chdir(getenv("HOME"));

		} else {
			chdir(arguments[1]);
		}

		return 1;

	} else {

		//if its not any of the 3 commands it runs the program
		//wit the the internal commands
		return start(arguments, childProcesses, stats, number);
	}
}

void checkForBackgroundProcess(pid_t *childProcesses)
{
	int i;
	int status;
	int pid;						

	//loops an checks for the finishe procdes
	do {

		pid = waitpid(-1, &status, WNOHANG);	

		//if any of the processes have finished
		if(0 < pid) {

			//checks to see the pid ID exists
			if(childProcesses[i] == pid)
			{
				printf("Background pid %d is done: ", pid);
	
				//check and prints it if its a signal or normal exits
				if(WIFEXITED(status)) {
					printf("exit value %d\n", WEXITSTATUS(status));

				} else if(WIFSIGNALED(status) != 0)	{
					printf("terminated by signal %d.\n", WTERMSIG(status));
				}
			
				//sets the array back to defualt
				childProcesses[i] = 0;											

				return;
			}
		}
 
 	} while(0 < pid);
}

//Just a function to make arguments 0 because 
//the terminal still detects it
void makeArgumentsNULL(char ** arguments) {
	arguments[1] = NULL;
	arguments[2] = NULL;
}
		
int redirect(char **arguments, int inputFile, bool background)
{
	int file;																									

	switch(inputFile) {

		//CASE INPUT TO FILE
		case 1:

		//open file to write
		file = open(arguments[2], O_WRONLY | O_CREAT | O_TRUNC, 0664);

		//if file fails to open or doesnt exist
		if(file == -1)
		{
			printf(" %s: no such file or diretory\n", arguments[2]);
		}

		makeArgumentsNULL(arguments);
		break;

		//CASE OUTPUT TO FILE
		case 0:

		//open file to read
		file = open(arguments[2], O_RDONLY);

		if(file == -1)
		{
			printf(" %s: no such file or diretory\n", arguments[2]);
		}
		
		//make the arguments NULL
		makeArgumentsNULL(arguments);
		break;
	}

	//checks the background process for undefined redirection 
	//input
	if(background == true) 
	{
		if(inputFile != 1) {
			file = open("/dev/null", O_RDONLY, 0644);
		} else if(inputFile != 0)
			file = open("/dev/null", O_WRONLY, 0644);
	}

	return file;			
}

int main()
{
	//Signal structs to enable the signals for process handling
	struct sigaction signals;
	signals.sa_flags = 0;
	signals.sa_handler = SIG_IGN;
	sigaction(SIGINT, &signals, NULL);

	//runs the program
		//Declaring all the variables for the use
	char *comamndLine;										
	char **arguments;									
	int i,number;
	bool isRunning;							
	pid_t childProcesses[MAX];				
	int *stats = malloc(sizeof(int));

	for(i = 0; i < MAX; i++) {

		//It sets the child processes array
		//to 0 for reuse.
		childProcesses[i] = 0;
	}

	do
	{	
		//before every input it checks for all the background 
		//processes.
		checkForBackgroundProcess(childProcesses);					
		printf(": ");
		
		//Allocating memory for our line and arguments
		arguments = malloc(sizeof(char*) * MAX_ARGUMENTS);
		comamndLine = malloc(BUFFER_SIZE);

		fflush(stdout);	

		//reads the whole command lind
		fgets(comamndLine, BUFFER_SIZE, stdin);			

		//retuns the number of arguments in the command entered
		number = seperateCommand(arguments, comamndLine);				

		//returns the status of the program.
		isRunning = execute(arguments, childProcesses, stats, number); 

		//Free allocated memories
		free(comamndLine);
		free(arguments);

	}while(isRunning);

	return 0;
}
