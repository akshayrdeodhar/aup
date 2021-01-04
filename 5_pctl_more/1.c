#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define ZOMBIE 1
#define ORPHAN 2

#define BUFSIZE 100
#define N 100
#define MOD 1000
static int buf[BUFSIZE];
static int arr[BUFSIZE];

void random_array(int *arr, int n, int limit) {
		int i;
#pragma omp parallel for private(i)
		for (i = 0; i < n; i++) {
				arr[i] = rand() % limit;
		}
}

int bin_search(int *arr, int n, int x) {
		int left, right, mid;
		int mid_elem;
		left = 0;
		right = n - 1;
		while (left <= right) {
				mid = (left + right) / 2;
				mid_elem = arr[mid];
				if (x < mid_elem) {
						right = mid - 1;
				}
				else if (x > mid_elem) {
						left = mid + 1;
				}
				else {
						return mid;
				}
		}
		return -1;
}


/* merges a[left:mid], a[mid:right], using temp */
void merge(int *a, int left, int right, int *buf) {
		int mid;
		int size = left;
		int lp, rp;

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

		int start, end;
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


		memcpy(a + left, buf + left, sizeof(int) * (right - left));
}


void mergesort_serial(int *a, int left, int right, int *buf) {
		int mid = (left + right) / 2;

		if ((right - left) <= 1) {
				/* already sorted */
				return;
		}

		mergesort_serial(a, left, mid, buf);
		mergesort_serial(a, mid, right, buf);
		merge(a, left, right, buf);

		return;
}

void print_arr(int *arr, int n) {
		int i;
		for (i = 0; i < n; i++) {
				printf("%d ", arr[i]);
		}
		printf("\n");
}


int main(int argc, char *argv[]) {

		int elem, indx;
		int n = N;

		random_array(arr, n, MOD);
		mergesort_serial(arr, 0, n, buf);

		fflush(stdin);
		if (fork()) {
				/* parent */
#if SCENARIO == ZOMBIE
				sleep(10);
				exit(0);
#elif SCENARIO == ORPHAN
				exit(0);
#endif
		}
		else {
				/* child */
				print_arr(arr, n);
				printf("\nElement: ");
				fflush(stdin);
				scanf("%d", &elem);
				indx = bin_search(arr, n, elem);
				if (indx >= 0) {
						printf("Found %d at %d\n", elem, indx);
				}
				else {
						printf("Element %d not found\n", elem);
				}
#if SCENARIO == ORPHAN
				printf("Child = %d, Parent = %d\n", getpid(), getppid());
#endif
		}

}



