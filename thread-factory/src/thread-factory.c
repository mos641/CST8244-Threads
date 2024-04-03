#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

void sigint_handler(int sig);
int user_input(char msg[]);
void* child_thread();

volatile sig_atomic_t usr1Happened = 0; // false
// pointer to a semaphore
sem_t *sem;

int main(void) {
	char *sem_name = "threads_semaphore";

	// initialize signal action handler
	struct sigaction sigAction;
	sigAction.sa_handler = sigint_handler;
	sigAction.sa_flags = 0;
	sigemptyset(&sigAction.sa_mask);

	// register signal - exit if an error occurs
	if (sigaction(SIGUSR1, &sigAction, NULL) == -1) {
		perror("signal action failure");
		exit(EXIT_FAILURE);
	}

	// ask and store the amount of threads
	int numOfChildren = user_input("Enter the number of threads:\n");

	// create semaphore
	sem = sem_open(sem_name, O_CREAT, S_IWOTH, 0);

	if (sem == SEM_FAILED) {
		printf("Creating semaphore failed - exiting");
		return (EXIT_FAILURE);
	}

	printf("Parent thread PID = %d \n", getpid());

	// create the number of children threads
	int i = 0;
	while (i < numOfChildren) {
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_create(NULL, &attr, &child_thread, NULL);
		pthread_attr_destroy(&attr);

		i++;
	}

	// wait for kill signal
	while (usr1Happened == 0) {
		sleep(1);
	}

	// close and unlink semaphore
	printf("sem_close: %d\n", sem_close(sem));
	printf("sem_unlink: %d\n", sem_unlink(sem_name));
	sleep(8);

	exit(EXIT_SUCCESS);
}

void sigint_handler(int sig) {
	usr1Happened = 1; // true
}

int user_input(char msg[]) {
	char *end, buffer[100];
	long numInput;

	do {
		// prompt for user input
		printf("%s", msg);
		if (!fgets(buffer, sizeof buffer, stdin)) {
			break;
		}

		// remove the last character of new line
		buffer[strcspn(buffer, "\n")] = '\0';

		// convert string to integer
		numInput = strtol(buffer, &end, 10);

		// confirm the entire line was used in integer conversion
	} while (end != buffer + strlen(buffer));

	// return user input as integer
	return numInput;
}

void* child_thread() {
	/*
	 print message that thread has been created; display thread ID

	 while true:

	 wait on the semaphore

	 print message the thread has unblocked; display thread ID

	 sleep for 5 secs (simulates work being done by thread)

	 return NULL
	 */

	printf("Thread #%d created \n", pthread_self());
	int status;
	while (1) {
		// wait for semaphore
		status = sem_wait(sem);
		while (status > 0) {
			status = sem_wait(sem);
			sleep(1);
		}

		// print info
		printf("Thread #%d unblocked  		Status: %d \n", pthread_self(), status);

		sleep(5);
	}

	exit(EXIT_SUCCESS);
}

