#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define BUFSIZE 512
#define FIRSTCHARS 10

int main(int argc, char *argv[]) {
		if (argc != 2) {
				fprintf(stderr, "usage: weirdprocess <filename>\n");
				return EINVAL;
		}

		int fp, count;
		char smallbuf[FIRSTCHARS + 1];

		if ((fp = open(argv[1], O_RDWR)) == -1) {
				perror(argv[1]);
				return errno;
		}

		/* read first 10 characters from the file */
		if (read(fp, smallbuf, FIRSTCHARS) != FIRSTCHARS) {
				fprintf(stderr, "%s does not have %d characters\n", argv[1], FIRSTCHARS);
				return EINVAL;
		}

		smallbuf[FIRSTCHARS] = '\n';

		/* write first 10 characters to stdout */
		if ((count = write(0, smallbuf, FIRSTCHARS + 1)) != FIRSTCHARS + 1) {
				if (count == -1) {
						perror(argv[1]);
						return errno;
				}
				else {
						fprintf(stderr, "could not write first %d chars to stdout\n", 
										FIRSTCHARS + 1);
						return EINVAL;
				}
		}
		close(fp);

		if ((fp = open(argv[1], O_APPEND | O_WRONLY)) == -1) {
				perror(argv[1]);
				return errno;
		}

		// assert: we have reached the end of the given file

		strcpy(smallbuf, "hello");

		/* write "hello" at the end of the file */
		if ((count = write(fp, smallbuf, strlen(smallbuf))) != strlen(smallbuf)) {
				if (count == -1) {
						perror(argv[1]);
						return errno;
				}
				else {
						fprintf(stderr, "could not write %s at the end of file %s\n", 
										smallbuf, argv[1]);
						return EINVAL;
				}
		}

		if (close(fp)) {
				perror(argv[1]);
				return errno;
		}

		return 0;
}
