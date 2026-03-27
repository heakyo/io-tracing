#!/usr/sbin/dtrace -s

/*
 * test command:
 *      ./ioflow.d -c 'umount /dev/ada0p20'
 */

#pragma D option flowindent

BEGIN
{
        procname = "umount";

	sysunmount_flag = 0;

        /* @[stack()] = count() */
        printf("-----IO Tracing Start-----");
}

/*Kernel Space*******************************************************************************/
sys_unmount:entry
/execname == procname/
{
	this->uap = args[1];

	printf("uap:flags:%p path:%s",
		this->uap->flags,
		copyinstr((uintptr_t)this->uap->path)
	);

	sysunmount_flag = 1;
}

ffs_unmount:entry
/sysunmount_flag/
{
	this->mp = args[0];

	this->ump = (struct ufsmount *)this->mp->mnt_data;

	printf("ump:fs:0x%p",
		this->ump->um_fs
	);
        printf("\n\t\t\t\t\t      ");

	this->fs = this->ump->um_fs;
	printf("fs:flags:%p",
		this->fs->fs_flags
	);
}

ffs_sbupdate:entry
/sysunmount_flag/
{
	this->ump = args[0];

	this->fs = this->ump->um_fs;
	printf("fs:flags:%p",
		this->fs->fs_flags
	);
}

vfs_freeopts:entry
/sysunmount_flag/
{
	this->opts = args[0];

	this->opt = this->opts->tqh_first;
	printf("opt:%p name:%s",
		this->opt,
		stringof(this->opt->name)
	);
}

/*return*************************************************************************************/
vfs_freeopts:return
/sysunmount_flag/
{}

ffs_sbupdate:return
/sysunmount_flag/
{}

ffs_unmount:return
/sysunmount_flag/
{}

sys_unmount:return
/execname == procname/
{
	trace(probename);
	sysunmount_flag = 0;
}

/********************************************************************************************/
END
{
        printf("-----IO Tracing END-----");
        printf("\n\t\t\t\t\t      ");
}

