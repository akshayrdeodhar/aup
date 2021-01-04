#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <wait.h>

int main(void) {

		sigset_t mask, pending;

	    /* block SIGCHLD*/
		if (sigaddset(&mask, SIGCHLD) == -1) {
				perror("sigaddset");
				return errno;
		}

		if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
				perror("sigprocmask- modify");
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

				sleep(5);

				if (sigpending(&pending) == -1) {
						perror("sigpending");
						return errno;
				}

				int is_member;
				if ((is_member = sigismember(&pending, SIGCHLD)) == -1) {
						perror("sigismember");
						return errno;
				}
				else if (is_member) {
						printf("SIGCHLD is pending\n");
				}
				else {
						printf("SIGCHLD is not pending\n");
				}

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
