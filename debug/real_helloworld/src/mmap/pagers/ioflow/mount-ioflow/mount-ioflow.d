#!/usr/sbin/dtrace -s

/*
 * test command:
 *      ./ioflow.d -c 'mount /dev/ada0p20'
 */

#pragma D option flowindent

BEGIN
{
        procname = "umount";

	sysnmount_flag = 0;

        /* @[stack()] = count() */
        printf("-----IO Tracing Start-----");
}

/*Kernel Space*******************************************************************************/
ffs_mountfs:entry
{
	this->devvp = args[0];

	this->dev = this->devvp->v_rdev;

	printf("dev:%p name:%s iosize_max:%p",
		this->dev,
		stringof(this->dev->si_name),
		this->dev->si_iosize_max
		);
}

/*return*************************************************************************************/
ffs_mountfs:return
{}

/********************************************************************************************/
END
{
        printf("-----IO Tracing END-----");
        printf("\n\t\t\t\t\t      ");
}

