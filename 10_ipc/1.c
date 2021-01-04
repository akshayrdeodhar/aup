#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define M1 "Mumbai"
#define M2 "Delhi"
#define M3 "Chennai"
#define M4 "Cochin"

#define BUFLEN 10

int main(void) {

		int pid, ppid;

		int pipes[5][2];

		int n;
		char buf[BUFLEN];

		int i;

		/* open 4 pipes */
		for (i = 1; i <= 4; i++) {
				if (pipe(pipes[i]) == -1) {
						perror("pipe");
						return errno;
				}
		}

		if ((pid = fork()) == -1) {
				perror("fork 1 failed");
				return errno;
		}
		else if (!pid) {
				/* C1 */

				/* 1 W */
				close(pipes[1][1]);

				/* 2 R */
				close(pipes[2][0]);

				/* 3 R */
				close(pipes[3][0]);

				/* 4 RW */
				close(pipes[4][0]);
				close(pipes[4][1]);

				pid = getpid();
				ppid = getppid();

				printf("%d is child of %d\n", pid, ppid);

				if ((n = read(pipes[1][0], buf, BUFLEN)) == -1) {
						perror("read 1");
						return errno;
				}
				printf("%d Read %s\n", pid, buf);
				close(pipes[1][0]);

				if (write(pipes[2][1], M2, strlen(M2) + 1) == -1) {
						perror("write 2");
						return errno;
				}
				printf("%d Wrote %s\n", pid, M2);
				close(pipes[2][1]);

				if (write(pipes[3][1], M3, strlen(M3) + 1) == -1) {
						perror("write 3");
						return errno;
				}
				printf("%d Wrote %s\n", pid, M3);
				close(pipes[3][1]);

				return 0;
		}

		if ((pid = fork()) == -1) {
				perror("fork 2 failed");
				return errno;
		}
		else if (!pid) {
				/* C2 */

				/* 1 RW */
				close(pipes[1][0]);
				close(pipes[1][1]);

				/* 2 RW */
				close(pipes[2][0]);
				close(pipes[2][1]);

				/* 3 W */
				close(pipes[3][1]);

				/* 4 R */
				close(pipes[4][0]);

				pid = getpid();
				ppid = getppid();
				printf("%d is child of %d\n", pid, ppid);

				if ((n = read(pipes[3][0], buf, BUFLEN)) == -1) {
						perror("read 3");
						return errno;
				}
				printf("%d Read %s\n", pid, buf);
				close(pipes[3][0]);

				if (write(pipes[4][1], M4, strlen(M4) + 1) == -1) {
						perror("write 4");
						return errno;
				}
				printf("%d Wrote %s\n", pid, M4);
				close(pipes[4][1]);

				return 0;
		}

		/* P */

		/* close read end of 1 */
		close(pipes[1][0]); 

		/* close write end of 2 */
		close(pipes[2][1]);

		/* close write end of 4 */
		close(pipes[4][1]);

		/* close both ends of 3 */
		close(pipes[3][0]);
		close(pipes[3][1]);

		pid = getpid();
		printf("%d is parent\n", pid);


		if (write(pipes[1][1], M1, strlen(M1) + 1) == -1) {
				perror("write 1");
				return errno;
		}
		printf("%d Wrote %s\n", pid, M1);
		close(pipes[1][1]);

		if ((n = read(pipes[2][0], buf, BUFLEN)) == -1) {
				perror("read 2");
				return errno;
		}
		printf("%d Read %s\n", pid, buf);
		close(pipes[2][0]);

		if ((n = read(pipes[4][0], buf, BUFLEN)) == -1) {
				perror("read 4");
				return errno;
		}
		printf("%d Read %s\n", pid, buf);
		close(pipes[4][0]);
		
		return 0;
}

