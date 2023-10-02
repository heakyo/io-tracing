#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./exec.d -c './main'
 */

#pragma D option flowindent

BEGIN
{
	printf("-----IO Tracing Start-----");

	proc = "main";
}
/*common*************************************************************************************/
syscall::ptrace:entry,
syscall::ptrace:return
/execname == proc && 0/
{}


/*entry**************************************************************************************/
sys_execve:entry
/execname == proc/
{}

do_execve:entry
/execname == proc/
{
}

/*return*************************************************************************************/

do_execve:return
/execname == proc/
{}

sys_execve:return
/execname == proc/
{}

/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
}
