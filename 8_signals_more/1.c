#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <wait.h>

void print_message(int signo) {
		printf("%d hit by signal %d\n", getpid(), signo);
}


int main(void) {
		struct sigaction sigint_handler;
		sigset_t old, new;

		sigint_handler.sa_handler = print_message;
		sigemptyset(&(sigint_handler.sa_mask));

		/* read current mask */
		if (sigprocmask(0, NULL, &new) == -1) {
				perror("sigprocmask- read");
				return errno;
		}

		if (sigaction(SIGINT, &sigint_handler, NULL) == -1) {
				perror("sigaction");
				return errno;
		}

		/* block SIGINT */
		if (sigaddset(&new, SIGINT) == -1) {
				perror("sigaddset");
				return errno;
		}

		if (sigprocmask(SIG_BLOCK, &new, &old) == -1) {
				perror("sigprocmask- modify");
				return errno;
		}

		printf("Press any key to continue\n");

		getchar();

		int child_pid, status, wait_ret;
		if ((child_pid = fork()) == -1) {
				perror("fork");
				return errno;
		}
		else if (child_pid) {
				/* parent */
				/* stop blocking SIGINT */
				printf("Parent: %d\n", getpid());
				if (sigprocmask(SIG_SETMASK, &old, NULL) == -1) {
						perror("sigprocmask- reset old, parent");
						return errno;
				}

				if ((wait_ret = wait(&status)) == -1) {
						perror("wait");
						return errno;
				}

				printf("Child %d returned\n", wait_ret);

		}
		else {
				/* child */
				printf("Child: %d\n", getpid());
				/* stop blocking SIGINT */
				if (sigprocmask(SIG_SETMASK, &old, NULL) == -1) {
						perror("sigprocmask- reset old, child");
						return errno;
				}
		}
		
		return 0;
}
