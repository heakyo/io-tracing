#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>

extern int a2;
extern void func_a1();

int main(int argc, char *argv[])
{
	printf("in main\n");

	void *handle = dlopen(0, RTLD_NOW);
	if (!handle) {
		printf("dlopen failed!\n");
		return -1;
	}

	printf("\n----------main----------\n");
	void *addr_main = dlsym(handle, "main");
	printf("addr_main = %p\n", addr_main);

	printf("\n----------liba.so----------\n");
	void *addr_a1 = dlsym(handle, "a1");
	void *addr_a2 = dlsym(handle, "a2");
	void *addr_func_a1 = dlsym(handle, "func_a1");
	void *addr_func_a2 = dlsym(handle, "func_a2");
	void *addr_func_a3 = dlsym(handle, "func_a3");

	printf("addr_a1 = %p\n", addr_a1);
	printf("addr_a2 = %p\n", addr_a2);
	printf("addr_func_a1 = %p\n", addr_func_a1);
	printf("addr_func_a2 = %p\n", addr_func_a2);
	printf("addr_func_a3 = %p\n", addr_func_a3);

	printf("\n----------libb.so----------\n");
	void *addr_b = dlsym(handle, "b");
	void *addr_func_b = dlsym(handle, "func_b");

	printf("addr_b = %p\n", addr_b);
	printf("addr_func_b = %p\n", addr_func_b);

	dlclose(handle);

	a2 = 100;

	func_a1();

        getchar();

        return 0;
}
