#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void child_1_action(void) {
		printf("I am the first-born: %d\n", getpid());
		exit(0);
}

void child_2_action(void) {
		sleep(100);
		exit(0);
}

int main(int argc, char *argv[]) {

		int childpid;
		void (*child_action[])(void) = {child_1_action, child_2_action};

		for (int i = 0; i < 2; i++) {
				if ((childpid = fork()) == -1) {
						perror("fork");
						return errno;
				}
				else if (!childpid) {
						/* in child */
						child_action[i]();
						/* will never return here, function has exit() in it */
				}
				/* is parent */
		}

		if (system("ps -o command,pid,ppid,state") == -1) {
				perror("ps -o command,pid,ppid,state");
		}

		return 0;
}



