#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

void str_reverse(char *buf, int len) {
		int left, right;
		left = 0;
		right = len - 1;
		char temp;
		while (left < right) {
				temp = buf[left];
				buf[left] = buf[right];
				buf[right] = temp;
				++left;
				--right;
		}
}

#define BUFSIZE 4096

int main(int argc, char *argv[]) {
		if (argc != 2) {
				printf("usage: ./tac.out <filename>\n");
				return EINVAL;
		}

		int fp, bytes, bytes_prev, bytes_read;
		char buf[BUFSIZE];


		/* try to open file */
		if ((fp = open(argv[1], O_RDONLY)) == -1) {
				perror(argv[1]);
				return errno;
		}

		/* assert: file exist and fd has a descriptor which points to file */

		/* go to end of file */
		if ((bytes = lseek(fp, 0, SEEK_END)) == -1) {
				perror("cannot lseek to end of file");
				goto graceful_exit;
		}
		/* printf("Size of file = %d\n", bytes); */

		bytes_prev = bytes;
		/* printf("bytes read = %d\n", bytes_prev); */
		while ((bytes = lseek(fp, -BUFSIZE, SEEK_CUR)) != -1) {
				/* while we *can* move BUFSIZE bytes backward */
				bytes_prev = bytes;

				if ((bytes_read = read(fp, buf, BUFSIZE)) != BUFSIZE) {
						perror("read");
						goto graceful_exit;
				}
				/* assert: read BUFSIZE bytes into buffer */

				str_reverse(buf, BUFSIZE);
				/* reverse the buffer */

				if (write(1, buf, BUFSIZE) != BUFSIZE) {
						perror("unable to write chunk to stdout");
						goto graceful_exit;
				}
				/* wrote the reverse of the buffer to stdout */

				if (lseek(fp, -BUFSIZE, SEEK_CUR) != bytes_prev) {
						perror("unable to seek to previous position");
						goto graceful_exit;
				}

				/* assert: at the end of the next block to be read */
		}

		/* assert: cannot lseek BUFSIZE bytes backward, this means that there
		 * are less than BUFSIZE bytes left OR we have read the last chunk */

		/* assert: bytes_prev contains the offset of the last chunk that was
		 * *read* */

		if (errno != EINVAL) {
				/* assert: error is not due to going before 0 */
				perror("wrong error when lseeking to negative offset");
				goto graceful_exit;
		}

		if (lseek(fp, 0, SEEK_SET) != 0) {
				perror("cannot seek to beginning of file");
				goto graceful_exit;
		}

		/* assert: we are at the beginning of file */

		if (read(fp, buf, bytes_prev) != bytes_prev) {
				/* printf("bytes_read = %d\n", bytes_prev); */
				perror("unable to read last chunk");
				goto graceful_exit;
		}

		str_reverse(buf, bytes_prev);
		if (write(1, buf, bytes_prev) != bytes_prev) {
				perror("unable to write last chunk");
				goto graceful_exit;
		}

		close(fp);
		return 0;

graceful_exit:
		close(fp);
		return errno;
}
