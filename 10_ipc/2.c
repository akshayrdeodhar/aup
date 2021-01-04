#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


#define FIFO1 "/tmp/aup_fifo1"
#define FIFO2 "/tmp/aup_fifo2"
#define FILENAME "/tmp/aup_file"
#define BUFLEN 10

int main(void) {

		int pid;
		int fr, fw, fp;
		int i;
		char buf[BUFLEN];

		if (mkfifo(FIFO1, S_IRUSR | S_IWUSR) == -1) {
				perror("mkfifo 1");
				return errno;
		}

		if (mkfifo(FIFO2, S_IRUSR | S_IWUSR) == -1) {
				perror("mkfifo 2");
				return errno;
		}

		if ((fp = open(FILENAME, O_WRONLY | O_CREAT,
					   S_IRUSR | S_IWUSR)) == -1) {
				perror("file");
				return errno;
		}

		if ((pid = fork()) == -1) {
				perror("fork");
				return errno;
		}
		else if (pid) {
				/* P1 */

				if ((fw = open(FIFO1, O_WRONLY)) == -1) {
						perror("write 1");
						return errno;
				}

				if ((fr = open(FIFO2, O_RDONLY)) == -1) {
						perror("read 2");
						return errno;
				}

				i = 1;
				while (i <= 100) {
						sprintf(buf, "%d\n", i);

						if (write(fp, buf, strlen(buf)) == -1) {
								perror("odd write");
								return errno;
						}

						if (write(fw, "*", 1) == -1) {
								perror("sync write odd");
								return errno;
						}

						if (read(fr, buf, 1) == -1) {
								perror("sync read odd");
								return errno;
						}

						i += 2;
				}

				close(fp);
				close(fw);
				close(fr);

				return 0;
		}
		else {
				/* P1 */

				if ((fr = open(FIFO1, O_RDONLY)) == -1) {
						perror("read 1");
						return errno;
				}

				if ((fw = open(FIFO2, O_WRONLY)) == -1) {
						perror("write 2");
						return errno;
				}

				i = 2;
				while (i <= 100) {
						if (read(fr, buf, 1) == -1) {
								perror("sync read even");
								return errno;
						}

						sprintf(buf, "%d\n", i);

						if (write(fp, buf, strlen(buf)) == -1) {
								perror("odd write");
								return errno;
						}

						if (write(fw, "*", 1) == -1) {
								perror("sync write even");
								return errno;
						}
						
						i += 2;
				}

				close(fp);
				close(fw);
				close(fr);

				return 0;
		}
}
