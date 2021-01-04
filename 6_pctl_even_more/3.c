#define _DEFAULT_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>

int is_process_foreground(void) {

		int p_pgrp, fg_pgrp;

		if ((p_pgrp = getpgrp()) == -1) {
				perror("parent getpgrp");
				return errno;
		}

		if ((fg_pgrp = tcgetpgrp(STDIN_FILENO)) == -1) {
				perror("parent tcgetpgrp");
				return errno;
		}

		if (fg_pgrp == p_pgrp) {
				return 1;
		}
		else {
				return 0;
		}

}



int main(void) {

		int cpid;
		int status;

		if ((cpid = fork())) {
				/* parent */
				if (setpgid(cpid, 0) == -1) {
						perror("setpgid in parent");
						return errno;
				}

				if (tcsetpgrp(STDIN_FILENO, cpid) == -1) {
						perror("tcsetpgrp in parent");
						return errno;
				}

				if (is_process_foreground()) {
						printf("Before Wait: Parent is foreground\n");
				}
				else {
						printf("Before Wait: Parent is NOT foreground\n");
				}

				if (wait(&status) == -1) {
						perror("wait");
						return errno;
				}

				if (is_process_foreground()) {
						printf("After Wait: Parent is foreground\n");
				}
				else {
						printf("After Wait: Parent is NOT foreground\n");
				}
		}
		else {
				/* child */


				if (setpgid(0, 0) == -1) {
						perror("setpgid in child");
						return errno;
				}


				if (tcsetpgrp(STDIN_FILENO, getpgid(0)) == -1) {
						perror("tcsetpgrp in parent");
						return errno;
				}

				if (system("ps -o cmd,pid,ppid,pgid,tpgid") == -1) {
						perror("ps");
						return errno;
				}

				if (is_process_foreground()) {
						printf("In Child: Process is foreground\n");
				}
				else {
						printf("In Child: Process is NOT foreground\n");
				}
		}
}


