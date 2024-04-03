#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

int user_input(char msg[]);

int main(void) {
	char *sem_name = "threads_semaphore";
	int numOfThreads = -1;
	// pointer to a semaphore
	sem_t *sem;
	int i = 0;
	sem = sem_open(sem_name, 0);

	if (sem == SEM_FAILED) {
		printf("Getting semaphore reference failed - exiting");
		return (EXIT_FAILURE);
	}

	printf("PID = %d \n", getpid());

	// loop until user enters 0
	while (numOfThreads != 0) {
		// ask and store the amount of threads
		numOfThreads = user_input(
				"How many threads do you want to wake up (enter 0 to exit)?\n");

		// increment semaphore
		for (i = 0; i < numOfThreads; i++) {
			sem_post(sem);
		}
	}
	sem_close(sem);
	printf("exiting...");
	return (EXIT_SUCCESS);
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

		if (numInput < 0) {
			// re prompt if less than 0
			printf("Enter a value of 0 or greater\n");
			end = NULL;
		}

		// confirm the entire line was used in integer conversion
	} while (end != buffer + strlen(buffer));

	// return user input as integer
	return numInput;
}
