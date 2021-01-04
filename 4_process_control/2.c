#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

int public_var;
jmp_buf buf;
static int static_var;

int a(jmp_buf buf) {
		static int a_static_var;
		auto int a_auto_var;
		int ret;

		a_static_var = 4;
		a_auto_var = 5;

		printf("a_static_var = %d\na_auto_var = %d\npublic_var = %d\nstatic_var = %d\n\n", 
				a_static_var, a_auto_var, public_var, static_var);

		ret = setjmp(buf);

		printf("setjump(buf) = %d\n", ret);
		printf("a_static_var = %d\na_auto_var = %d\npublic_var = %d\nstatic_var = %d\n\n", 
				a_static_var, a_auto_var, public_var, static_var);

		a_static_var = 104;
		a_auto_var = 105;

		return ret;
}

int b(jmp_buf buf) {
		longjmp(buf, 42);
}


int main(void) {
		auto int auto_var;

		public_var = 1;
		static_var = 2;
		auto_var = 3;

		printf("Before a(buf)\n");
		printf("public_var = %d\nstatic_var = %d\nauto_var = %d\n\n",
				public_var, static_var, auto_var);

		if (a(buf)) {
				exit(0);
		}

		printf("After a(buf), before b(buf)\n");
		printf("public_var = %d\nstatic_var = %d\nauto_var = %d\n\n",
				public_var, static_var, auto_var);

		public_var = 101;
		static_var = 102;
		auto_var = 103;

		b(buf);

		printf("After b(buf)\n");
		printf("public_var = %d\nstatic_var = %d\nauto_var = %d\n\n",
				public_var, static_var, auto_var);

		return 0;
}
