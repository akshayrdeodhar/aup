#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(void) {
		int child_pid;

		if ((child_pid = fork()) == -1) {
				/* fork failed */
				perror("fork");
				return errno;
		}
		else if (child_pid) {
				/* parent */
				sleep(5);
				printf("Parent is Alive\n");
		}
		else {
				/* child */
				if (kill(getppid(), SIGINT) == -1) {
						perror("SIGINT to parent");
						return errno;
				}
				printf("Child sent kill signal\n");
				sleep(5);
				printf("Child slept for time equal to parent\n");
		}
		
		return 0;
}
