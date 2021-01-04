#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define BUF_SIZE 1024
typedef struct {
		int fp;
		int offset;
		int chars_read;
		char buf[BUF_SIZE];
}read_buf;

void get_read_buf(read_buf *b, int fp) {
		b->fp = fp;
		b->offset = 0;
		b->chars_read = -1;
}

void destroy_read_buf(read_buf *b) {
		close(b->fp);
		b->offset = b->fp = 0;
		b->chars_read = -1;
}

int getchar_buffered(read_buf *b) {
		if (b->chars_read < 0 || b->offset == b->chars_read) {
				if ((b->chars_read = read(b->fp, b->buf, BUF_SIZE)) == -1) {
						perror("read");
						exit(errno);
				}

				if (!b->chars_read) {
						/* this is the end of file */
						return EOF;
				}

				/* assert: buffer has nonzero number of bytes */
				b->offset = 0;
		}

		return b->buf[++(b->offset)];
}

typedef struct {
		char *filepath;
		char *word;
		int count;
}search_params;


#define MAX_WORD 512 
#define IN 101
#define OUT 102
void *search_word(void *arg) {
		search_params *sp;
		sp = (search_params *)arg;

		int c;
		int fp;
		read_buf rb;

		int lineno = 1;

		int state = OUT;
		char word[MAX_WORD + 1];
		int wordlen = 0;

		if ((fp = open(sp->filepath, O_RDONLY)) == -1) {
				perror("open");
				exit(errno);
		}

		get_read_buf(&rb, fp);

		while ((c = getchar_buffered(&rb)) != EOF) {
				if (c == '\n') {
						lineno++;
				}

				if (state == OUT) {
						if (isalpha(c) || c == '_') {
								state = IN;
								wordlen = 0;
								word[wordlen++] = c;
						}
				}
				else if (state == IN) {
						if (!(isalpha(c) || (c == '_'))) {
								word[wordlen] = '\0';
								state = OUT;
								wordlen = 0;
								/* printf("%s\n", word); */
								if (strcmp(word, sp->word) == 0) {
										/* word found */
										printf("%s: Found word '%s' in line %d\n", sp->filepath, sp->word, lineno);
										++sp->count;
								}
						}
						else {
								word[wordlen++] = c;
						}
				}
				else {
						fprintf(stderr, "Invalid State\n");
						exit(1);
				}
		}

		destroy_read_buf(&rb);

		/* pthread_exit(NULL); */
		return &(sp->count);
}

int main(int argc, char *argv[]) {

		/* search word file1 file2 .. */
		if (argc < 3) {
				fprintf(stderr, "usage: ./search <word> <file1> [<file2> ... <file-n>]\n");
				return EINVAL;
		}

		int i; int n_files = (argc - 2); pthread_t *threads; search_params *parameters;

		if ((threads = (pthread_t *)malloc(sizeof(pthread_t) * n_files)) == NULL) {
				fprintf(stderr, "malloc failed\n");
				return 1;
		}

		if ((parameters = (search_params *)malloc(sizeof(search_params) * n_files)) == NULL) {
				fprintf(stderr, "malloc failed\n");
				return 1;
		}

		for (i = 0; i < n_files; i++) {

			/* add file path to parameters */
			if ((parameters[i].filepath = (char *)malloc(sizeof(char) * (strlen(argv[2]) + 1))) == NULL) {
					fprintf(stderr, "malloc failed\n");
					return 1;
			}
			strcpy(parameters[i].filepath, argv[2 + i]);

			/* add search pattern to parameters */
			if ((parameters[i].word = (char *)malloc(sizeof(char) * (strlen(argv[1]) + 1))) == NULL) {
					fprintf(stderr, "malloc failed\n");
					return 1;
			}
			strcpy(parameters[i].word, argv[1]);

			/* initialize count to 0 */
			parameters[i].count = 0;

			/* dispatch thread for searching file */
			pthread_create(&threads[i],
						   NULL,
						   search_word,
						   (void *)&parameters[i]);
		}

		int total_count = 0;

		for (i = 0; i < n_files; i++) {
				if (pthread_join(threads[i], NULL)) {
						fprintf(stderr, "Unable to join thread\n");
				}
				free(parameters[i].word);
				free(parameters[i].filepath);

				total_count += parameters[i].count;
		}

		printf("Found %d occurences of '%s' during search\n", total_count, argv[1]);
		
		return 0;
}
