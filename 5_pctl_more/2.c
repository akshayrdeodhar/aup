#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define SIZE 100
static pid_t children[SIZE];
static int child_sleep_time[SIZE];

int main(int argc, char *argv[]) {
		if (argc < 3) {
				fprintf(stderr, "usage: ./2 <no_of_children> <parent_sleep_time> [<child1_sleeptime> ... ]\n");
				return EINVAL;
		}

		int n_child, n_parent_sleep;

		n_child = atoi(argv[1]);
		
		if (n_child > SIZE) {
				fprintf(stderr, "Maximum number of children is %d\n", SIZE);
				return EINVAL;
		}

		if (argc != (n_child + 3)) {
				fprintf(stderr, "Please specify sleep time for each child\n");
				return EINVAL;
		}

		n_parent_sleep = atoi(argv[2]);


		int i;
		int status;
		int pidret;

		for (i = 0; i < n_child; i++) {
				child_sleep_time[i] = atoi(argv[3 + i]);
		}


		for (i = 0; i < n_child; i++) {
				if ((children[i] = fork()) == -1) {
						perror("fork");
						return errno;
				}
				else if (!children[i]) {
						sleep(child_sleep_time[i]);
						exit(0);
				}
		}


		// C1 2
		// C2 2
		// C3 4
		// C4 4

		for (i = 0; i < n_child; i++) {
				printf("Child: %d\t", children[i]);
				if ((pidret = waitpid(children[i], &status, WNOHANG)) == 0) {
						/* child process has not changed state, is still running
						 * */
						printf("RUNNING\n");
				}
				else if (pidret != -1) {
						printf("EXITED\n");
				}
				else {
						printf("ERROR\n");
				}
		}

		sleep(n_parent_sleep);

		system("ps");

		return 0;
}



