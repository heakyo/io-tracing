#include <stdio.h>

void print_hello(void);
void print_bye(void);

void print_hello(void)
{
	printf("Hello world\n");
}

void print_bye(void)
{
	printf("Goodbye world\n");
}

int main(int argc, char *argv[])
{
	print_hello();
	print_bye();

	return 0;
}
