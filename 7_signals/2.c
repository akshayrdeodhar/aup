#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>

void print_sigchld_msg(int sig_number) {
		printf("SIGCHLD hits parent (msg)\n");
}

int main(void) {
		pid_t child_id;

		int dead_child;
		int status;

		/* set handler for SIGCHLD */
		signal(SIGCHLD, print_sigchld_msg);

		if ((child_id = fork()) == -1) {
				perror("fork");
				return errno;
		}
		else if (child_id) {
				/* parent */

				if ((dead_child = wait(&status)) == -1) {
						perror("wait failed");
						return errno;
				}
				else {
						printf("Wait returned, child %d exited\n", dead_child);
				}
		}
		else {
				/* Child */
				printf("Child sleeping\n");
				sleep(10);
		}

		return 0;
}
				
