#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./hello-dtrace.d
 */

#pragma D option flowindent

BEGIN
{
	proc="cat";

	printf("-----IO Tracing Start-----");
}
/*common*************************************************************************************/
/*nullfs_*:**/
/*{}*/

/*entry**************************************************************************************/
nullfs_mount:entry
{
}

nullfs_statfs:entry
{
	stack();
}

nullfs_root:entry
{
	stack();
}

ffs_read:entry
/execname == proc/
{
	stack();
}

/*return*************************************************************************************/
ffs_read:return
/execname == proc/
{
}

nullfs_mount:return
{
}

nullfs_statfs:return
{

}

nullfs_root:return
{}

/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
	printf("\n\t\t\t\t\t      ");
}
