#include <stdio.h>
#include "rt.h"

int foo2(int a, int b);
int foo(int a, int b);
int foo_T_to_A(int a, int b);

int recurse(int depth);
int _recurse(int depth) {
	if (!depth) {
		return 42;
	}

	return recurse(depth - 1);
}

void test() __attribute__ ( (section (".text_A")) );
void test(){
	printf("test good \n");
}

int main() {
	RT_init();
	NaCl_init();
	test();
	foo(2, 2);
	foo_T_to_A(2,2);
	printf("foo2() -> %d (should be %d)\n", foo2(2, 2), foo(2, 2));
	printf("cascade() -> %d (should be %d)\n", recurse(9), _recurse(0));
	printf("foo_T_to_A() -> %d (should be %d)\n", foo_T_to_A(2, 2), foo_T_to_A(2, 2));
	test();
	return 0;
}
