#!/usr/sbin/dtrace -s

/*
 * test command:
 *      ./ioflow.d -c 'umount /dev/ada0p20'
 */

#pragma D option flowindent

BEGIN
{
        procname = "umount";

        /* @[stack()] = count() */
        printf("-----IO Tracing Start-----");
}

/*Kernel Space*******************************************************************************/
sys_unmount:entry
/execname == procname/
{}

/*return*************************************************************************************/
sys_unmount:return
/execname == procname/
{}

/********************************************************************************************/
END
{
        printf("-----IO Tracing END-----");
        printf("\n\t\t\t\t\t      ");
}

