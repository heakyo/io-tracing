#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libaio.h>
#include <assert.h>

extern int subdata;
extern int sub_func(int num);

int main(int argc, char *argv[])
{
	int result = sub_func(subdata);

        //getchar();

        return 0;
}
