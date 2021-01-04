#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#define ZOMBIE 1
#define ORPHAN 2

#define BUFSIZE 100
#define MAXLEN 12
#define N 100
#define MOD 1000

static int32_t buf[BUFSIZE];
static int32_t arr[BUFSIZE];

/* merges a[left:mid], a[mid:right], using temp */
void merge(int32_t *a, int32_t left, int32_t right, int32_t *buf) {
		int32_t mid;
		int32_t size = left;
		int32_t lp, rp;

		mid = (left + right) / 2;

		lp = left;
		rp = mid;

		while (lp < mid && rp < right) {
				if (a[lp] <= a[rp]) {
						buf[size++] = a[lp++];
				}
				else {
						buf[size++] = a[rp++];
				}
		}

		int32_t start, end;
		if (lp == mid) {
				start = rp;
				end = right;
		}
		else {
				start = lp;
				end = mid;
		}

		while (start < end) {
				buf[size++] = a[start++];
		}


		memcpy(a + left, buf + left, sizeof(int32_t) * (right - left));
}


void mergesort_serial(int32_t *a, int32_t left, int32_t right, int32_t *buf) {
		int32_t mid = (left + right) / 2;

		if ((right - left) <= 1) {
				/* already sorted */
				return;
		}

		mergesort_serial(a, left, mid, buf);
		mergesort_serial(a, mid, right, buf);
		merge(a, left, right, buf);

		return;
}

void print_arr(int32_t *arr, int32_t n) {
		int32_t i;
		for (i = 0; i < n; i++) {
				printf("%d ", arr[i]);
		}
		printf("\n");
}

void read_array(int32_t *arr, int32_t n) {
		int32_t i;
		for (i = 0; i < n; i++) {
				arr[i] = 0;
				scanf("%d", &arr[i]);
		}
}



int32_t main(int32_t argc, char *argv[]) {

		extern char **environ;

		int32_t n, i;

		scanf("%d", &n);

		read_array(arr, n);

		mergesort_serial(arr, 0, n, buf);

		char **sorted_array = (char **)malloc(sizeof(char *) * (n + 1));
		if (!sorted_array) {
				fprintf(stderr, "unable to allocate sufficient memory\n");
				return 1;
		}
		for (i = 0; i < n; i++) {
				sorted_array[i] = (char *)malloc(sizeof(char) * MAXLEN);
				if (!sorted_array[i]) {
						fprintf(stderr, "malloc failed while allocating string\n");
				}
				sprintf(sorted_array[i], "%d", arr[i]);
		}
		sorted_array[n] = NULL;

		/* this trickery works because the size of int32_t is 4 bytes
		 * and the (char *) pointers of argv will either be 4 byte or 
		 * 8 byte (on 32 and 64 bit machines respectively, never less */

		if (!fork()) {
				/* first child */
				execve("./1_child1", sorted_array, environ);
		}
		
		sleep(5);
		printf("First child Zombied\n");
		system("ps -o pid,ppid,stat,comm");

		if (!fork()) {
				/* second child */
				execve("./1_child2", sorted_array, environ);
		}
		
		/* for first child to truly become zombie, the parent should remain in
		 * while loop forever to ensure that child exits first.
		 * However, we also want the second child to become an orphan, which
		 * means that the parent needs to exit.
		 * HENCE, sleep() calls have been inserted appropriately so that the
		 * 1. first child exists first (thus becoming a zombie for some time)
		 * 2. Then, the parent exits  (sleeps for 5)
		 * 3. Finally, the second thild exits (sleeps for 10)
		 * 
		 * A while loop will not be an appropriate solution here
		 * The parent is not going to reap either of the children 
		 * */

}



