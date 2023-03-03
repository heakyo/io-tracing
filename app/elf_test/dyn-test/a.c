#include <stdio.h>

static int a1 = 10;

int a2 = 20;

extern int b;

extern void func_b(void);

static void func_a2(void)
{
	printf("in func_a2\n");
}

void func_a3(void)
{
	printf("in func_a3\n");
}

void func_a1(void)
{
	printf("in func_a1\n");

	a1 = 11;
	a2 = 21;

	b = 31;

	func_a2();
	func_a3();

	func_b();
}
