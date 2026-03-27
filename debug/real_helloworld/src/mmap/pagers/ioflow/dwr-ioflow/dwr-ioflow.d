#!/usr/sbin/dtrace -s

/*
 * test command:
 *      ./ioflow.d -c 'cat ../mount/ufsimg/ufstest'
 *      ./ioflow.d -c './main'
 */

#pragma D option flowindent

BEGIN
{
        rootvnodep = (struct vnode **)0xffffffff83df5740;

        procname = "umount";
	kwflag = 0;
	syncv_flag = 0;

        /* @[stack()] = count() */
        printf("-----IO Tracing Start-----");
        printf("\n\t\t\t\t\t      ");
        printf("rootvnode:%p", *rootvnodep);
}

/*Common*******************************************************************************/

/*Kernel Space*******************************************************************************/
sys_unmount:entry
/execname == procname/
{}

ffs_sync:entry
/execname == procname/
{
}

ffs_syncvnode:entry
/execname == procname/
{
        this->vp = args[0];
	this->waitfor = args[1];
	this->flags = args[2];

	printf("waitfor:%d flags:%p",
		this->waitfor,
		this->flags
	);
        printf("\n\t\t\t\t\t      ");

        /***** vnode *****/
        printf("vp:%p tag:%s type:%d mnt_data:%p op:",
                this->vp,
                stringof(this->vp->v_tag),
                this->vp->v_type,
                this->vp->v_mount->mnt_data
        );
        func((uintptr_t)this->vp->v_op);
        printf("\n\t\t\t\t\t      ");

        /***** inode *****/
        this->inode = (struct inode *)this->vp->v_data;
        printf("inode:%p flag:%p number:%p size:%d ump:%p",
                this->inode,
                this->inode->i_flag,
                this->inode->i_number,
                this->inode->i_size,
                this->inode->i_ump
        );
        printf("\n\t\t\t\t\t      ");

	syncv_flag = 1;
}

ffs_update:entry
/execname == procname/
{}

vfs_bio_awrite:entry
/syncv_flag/
{}

bwrite:entry
/syncv_flag/
{}

ffs_unmount:entry
/execname == procname/
{
}

/*return*************************************************************************************/
ffs_unmount:return
/execname == procname/
{
}

bwrite:return
/syncv_flag/
{}

vfs_bio_awrite:return
/syncv_flag/
{}

ffs_update:return
/execname == procname/
{}

ffs_syncvnode:return
/execname == procname/
{
	trace(probename);
	syncv_flag = 0;
}

ffs_sync:return
/execname == procname/
{}

sys_unmount:return
/execname == procname/
{}

/********************************************************************************************/
END
{
        printf("-----IO Tracing END-----");
        printf("\n\t\t\t\t\t      ");
}

