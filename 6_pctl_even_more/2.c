#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>

#define OWN_FILE "2_own_file.txt"
#define SCORES_FILE "2_scores_file.txt"

#define BUFSIZE 100

int dice(int n) {
		return rand() % n;
}

static char buf[BUFSIZE];

int main(void) {

		/* On exec:
		 * RUID   EUID  SSUID
		 * player owner owner */

		int n, i, score, roll, chars_printed;
		int fp_own, fp_scores;
		uid_t player, owner;

		/* program knows that it has owner perms due to sticky bit */

		player = getuid(); /* player is real user id */
		
		owner = geteuid(); /* owner is effective user id */

		scanf("%d", &n);

		score = 0;

		for (i = 0; i < n; i++) {

				roll = dice(6);

				score += roll;

				chars_printed = sprintf(buf, "Iteration: %d\tRolled: %d\tScore: %d\n", i, roll, score);

				/* when entering loop:
		 		 * RUID   EUID  SSUID
		 		 * player owner owner */

				if ((fp_scores = open(SCORES_FILE, O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR)) == -1) {
						perror(SCORES_FILE);
						return errno;
				}

				if (write(fp_scores, buf, chars_printed) != chars_printed) {
						perror("writing score to scores file");
						return errno;
				}

				if (setuid(player) == -1) {
						perror("switching to player");
						return errno;
				}

				/* now
				 * RUID		EUID	SSUID
				 * player	player	owner */

				if ((fp_own = open(OWN_FILE, O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR)) == -1) {
						perror(OWN_FILE);
						return errno;
				}

				if (write(fp_own, buf, chars_printed) != chars_printed) {
						perror("writings score to player file");
						return errno;
				}

				if (setuid(owner) == -1) {
						perror("switching to owner");
						return errno;
				}

				/* now
				 * RUID		EUID	SSUID
				 * player	owner	player*/
		}
}
