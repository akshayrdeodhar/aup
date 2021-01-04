#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define TEXT "0123456789"

int filter(char *dest, char *src, int n) {
		int l1, l2;
		l1 = l2 = 0;
		while (l1 < n) {
				if (src[l1]) {
						dest[l2++] = src[l1];
				}
				l1++;
		}
		return l2;
}



int main(int argc, char *argv[]) {
		int fi, fo;
		char buf[256];
		char buf2[256];
		int bytes;

		if (argc != 3) {
				fprintf(stderr, "usage: ./hole.out <file1> <file2>\n");
				return EINVAL;
		}

		if ((fo = open(argv[1], 
					O_WRONLY | O_CREAT | O_TRUNC, 
					S_IRUSR | S_IWUSR)) == -1) {

				perror(argv[1]);
				return errno;
		}

		if (lseek(fo, 10, SEEK_SET) == -1) {
				perror("lseek 10");
				close(fo);
				return errno;
		}

		if (write(fo, TEXT, 10) != 10) {
				perror("write [10, 20)");
				close(fo);
				return errno;
		}

		if (lseek(fo, 30, SEEK_SET) == -1) {
				perror("lseek 30");
				close(fo);
				return errno;
		}

		if (write(fo, TEXT, 10) == -1) {
				perror("write [30, 40)");
				close(fo);
				return errno;
		}

		printf("Created file %s with hole\n\n", argv[1]);

		close(fo);

		sprintf(buf, "od -c %s", argv[1]);

		if (system(buf) == -1) {
				perror(buf);
				return errno;
		}

		if ((fi = open(argv[1], O_RDONLY)) == -1) {
				perror(argv[1]);
				return errno;
		}

		if ((fo = open(argv[2], 
					O_WRONLY | O_CREAT | O_TRUNC, 
					S_IRUSR | S_IWUSR)) == -1) {
				perror(argv[2]);
				close(fi);
				return errno;
		}

		printf("\nCopied contents from %s to %s,"
				"removing NULL characters\n\n", 
				argv[1], argv[2]);

		int len;
		while((bytes = read(fi, buf, 256)) == 256) {
				len = filter(buf2, buf, 256);
				if (write(fo, buf2, len) != len) {
						perror("write failed");
						close(fi);
						close(fo);
						return errno;
				}
				return 0;
		}

		len = filter(buf2, buf, bytes);
		if (write(fo, buf2, len) != len) {
				perror("write failed");
				close(fi);
				close(fo);
				return errno;
		}

		close(fi);
		close(fo);

		sprintf(buf, "od -c %s", argv[2]);

		if (system(buf) == -1) {
				perror(buf);
				return errno;
		}

		struct stat f1, f2;

		if (lstat(argv[1], &f1) == -1) {
				sprintf(buf, "lstat %s", argv[1]);
				perror(buf);
				return errno;
		}

		if (lstat(argv[2], &f2) == -1) {
				sprintf(buf, "lstat %s", argv[2]);
				perror(buf);
				return errno;
		}

		printf("%s- Blocks: %ld, Bytes: %ld\n", argv[1], f1.st_blocks, f1.st_size);

		printf("%s- Blocks: %ld, Bytes: %ld\n", argv[2], f2.st_blocks, f2.st_size);

		return 0;
}
