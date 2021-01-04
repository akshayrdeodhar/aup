#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <wait.h>

void reaper(int signo) {
		int status, wait_ret;
		printf("Waiting in signal handler\n");
		if ((wait_ret = wait(&status)) == -1) {
				printf("Wait in signal handler failed\n");
		}
		else {
				printf("Wait in signal handler returned %d\n", wait_ret);
		}
}


int main(void) {
		struct sigaction sigint_handler;

		sigint_handler.sa_handler = reaper;
		sigemptyset(&(sigint_handler.sa_mask));

		if (sigaction(SIGCHLD, &sigint_handler, NULL) == -1) {
				perror("sigaction");
				return errno;
		}

		int child_pid, status, wait_ret;
		if ((child_pid = fork()) == -1) {
				perror("fork");
				return errno;
		}
		else if (child_pid) {
				/* parent */
				printf("Parent: %d\n", getpid());
				printf("Waiting in parent, after fork\n");
				if ((wait_ret = wait(&status)) == -1) {
						printf("Wait in parent after fork failed\n");
				}
				else {
						printf("Wait in parent after fork returned %d\n", wait_ret);
				}

		}
		else {
				/* child */
				printf("Child: %d\n", getpid());
				return 0;
		}
		
		return 0;
}
