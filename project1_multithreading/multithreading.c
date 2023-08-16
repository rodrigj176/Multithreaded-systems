#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>

void logStart(char* tID);//function to log that a new thread is started
void logFinish(char* tID);//function to log that a thread has finished its time

void startClock();//function to start program clock
long getCurrentTime();//function to check current time since clock was started
time_t programClock;//the global timer/clock for the program

typedef struct thread //represents a single thread
{
	char tid[4]; //id of the thread as read from file
	pthread_t tpid;
	int start;
	int aliveTime;
	char finish;
	
//add more members here as per requirement
} Thread;

void* threadRun(void* t);//the thread function, the code executed by each thread
int readFile(char* fileName, Thread** threads);//function to read the file content and build array of threads

int main(int argc, char *argv[])
{
	if(argc<2)
	{
		printf("Input file name missing...exiting with error code -1\n");
		return -1;
	}

    //you can add some suitable code here as per problem sepcification
	Thread* threads; //pointer too specific thread in container of threads
    int lines = readFile(argv[1], &threads); //read lines from file line by line BEFORE clock starts
    int numThreadsToDo = lines; // the number of threads left to do
    startClock();

	while(numThreadsToDo)//put a suitable condition here to run your program ---> While there are still more threads to be finished...
	{
	    //write suitable code here to run the threads
		 for (int i = 0; i < lines; i++) {
            if (threads[i].finish == 0 && threads[i].start <= getCurrentTime()) {
                //create thread
                pthread_create(&(threads[i].tpid), NULL, threadRun, &(threads[i]));
                //mark as finished
                threads[i].finish = 1;
                numThreadsToDo--;
            }
        }
    }
    // join threads
    for (int i = 0; i < lines; i++) {
        pthread_join(threads[i].tpid, NULL);
    }
    // free memory and finish
    free(threads);
	return 0;
}

int readFile(char* fileName, Thread** threads)//use this method in a suitable way to read file
{
	FILE *in = fopen(fileName, "r");
	if(!in)
	{
		printf("Child A: Error in opening input file...exiting with error code -1\n");
		return -1;
	}

	struct stat st;
	fstat(fileno(in), &st);
	char* fileContent = (char*)malloc(((int)st.st_size+1)* sizeof(char));
	fileContent[0]='\0';	
	while(!feof(in))
	{
		char line[100];
		if(fgets(line,100,in)!=NULL)
		{
			strncat(fileContent,line,strlen(line));
		}
	}
	fclose(in);

	char* command = NULL;
	int threadCount = 0;
	char* fileCopy = (char*)malloc((strlen(fileContent)+1)*sizeof(char));
	strcpy(fileCopy,fileContent);
	command = strtok(fileCopy,"\r\n");
	while(command!=NULL)
	{
		threadCount++;
		command = strtok(NULL,"\r\n");
	}
	*threads = (Thread*) malloc(sizeof(Thread)*threadCount);

	char* lines[threadCount];
	command = NULL;
	int i=0;
	command = strtok(fileContent,"\r\n");
	while(command!=NULL)
	{
		lines[i] = malloc(sizeof(command)*sizeof(char));
		strcpy(lines[i],command);
		i++;
		command = strtok(NULL,"\r\n");
	}

	for(int k=0; k<threadCount; k++)
	{
		char* token = NULL;
		//int j = 0;
		token =  strtok(lines[k],";");
		while(token!=NULL)
		{
//this loop tokenizes each line of input file
//write your code here to populate instances of Thread to build a collection

			strncpy((*threads + k)->tid, token, 4); // get the ID of thread
		
            token = strtok(NULL, ";"); //update 
            
            if(token) {  //get start time
                (*threads + k)->start = strtol(token , NULL, 10);
                token = strtok(NULL, ";");//update
            }
            if(token) { //get alivetime time
                (*threads + k)->aliveTime = strtol(token , NULL, 10);
                token = strtok(NULL, ";");//update
            }
            (*threads + k)->finish = 0; // thread is not finished

		}
	}
	return threadCount;
}

void logStart(char* tID)//invoke this method when you start a thread
{
	printf("[%ld] New Thread with ID %s is started.\n", getCurrentTime(), tID);
}

void logFinish(char* tID)//invoke this method when a thread is over
{
	printf("[%ld] Thread with ID %s is finished.\n", getCurrentTime(), tID);
}

void* threadRun(void* t)//implement this function in a suitable way
{
	char* tid = ((Thread *)t)->tid;// get thread ID
    int aliveTime = ((Thread *)t)->aliveTime; //get alivetime 
    
    logStart(tid);//log start
    
    long current = getCurrentTime(); //get the current time
    
    while(getCurrentTime() < (current + aliveTime)){ //loop untill Currenttime reaches the original current time + the total alivetime
        // waiting
    }
    
    logFinish(tid);//log Finish when alivetime is over
	
    return NULL;
}

void startClock()//invoke this method when you start servicing threads
{
	programClock = time(NULL);
}

long getCurrentTime()//invoke this method whenever you want to check how much time units passed
//since you invoked startClock()
{
	time_t now;
	now = time(NULL);
	return now-programClock;
}