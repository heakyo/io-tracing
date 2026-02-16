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

        procname = "main";
	kwflag = 0;

        /* @[stack()] = count() */
        printf("-----IO Tracing Start-----");
        printf("\n\t\t\t\t\t      ");
        printf("rootvnode:%p", *rootvnodep);
}

/*Common*******************************************************************************/
bstrategy:*
/kwflag/
{}

/*Kernel Space*******************************************************************************/
kern_pwritev:entry
/execname == procname/
{
	trace(probename);
	kwflag = 1;
}

ffs_write:entry
/kwflag/
{
	this->ap = args[0];
	this->vp = this->ap->a_vp;

	printf("vp:%p",
		this->vp
	);
        printf("\n\t\t\t\t\t      ");
}

ffs_balloc_ufs2:entry
/kwflag/
{
	this->vp = args[0];
	this->bpp = args[5];

	printf("vp:%p", this->vp);
}

vn_io_fault_pgmove:entry
/kwflag/
{}

bwrite:entry
/kwflag/
{
	printf("-----------------------------------------------------");
}

ffs_bufwrite:entry
/kwflag/
{}

bufwrite:entry
/kwflag/
{
	this->bp = args[0];
	this->vp = this->bp->b_vp;

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

	/***** buf *****/
	printf("bp:%p",
		this->bp
	);
        printf("\n\t\t\t\t\t      ");

	/***** bufobj *****/
	this->bufobj = this->bp->b_bufobj;
	printf("bufobj:%p private:%p bop_name:%s ops:",
		this->bufobj,
		this->bufobj->bo_private,
		stringof(this->bufobj->bo_ops->bop_name)
	);
	func((uintptr_t)this->bufobj->bo_ops);
        printf("\n\t\t\t\t\t      ");
	/*func((uintptr_t)this->bufobj->bo_ops->bop_write);*/

	/*stack();*/
}

bufstrategy:entry
/kwflag/
{}

ufs_strategy:entry
/kwflag/
{}

ffs_update:entry
/kwflag/
{
	this->vp = args[0];

	/***** vnode *****/
	printf("vp:%p tag:%s",
		this->vp,
		stringof(this->vp->v_tag)
	);
        printf("\n\t\t\t\t\t      ");
}

ffs_geom_strategy:entry
/kwflag/
{
	this->bufobj = args[0];
	printf("bufobj:%p private:%p bop_name:%s ops:",
		this->bufobj,
		this->bufobj->bo_private,
		stringof(this->bufobj->bo_ops->bop_name)
	);
	func((uintptr_t)this->bufobj->bo_ops);
        printf("\n\t\t\t\t\t      ");
}

adastrategy:entry
/0&&kwflag/
{}

bdone:entry
/0&&kwflag/
{}

/*return*************************************************************************************/
bdone:return
/0&&kwflag/
{}

adastrategy:return
/0&&kwflag/
{}

ffs_geom_strategy:return
/kwflag/
{}

ffs_update:return
/kwflag/
{
}

ufs_strategy:return
/kwflag/
{}

bufstrategy:return
/kwflag/
{}

bufwrite:return
/kwflag/
{}

ffs_bufwrite:return
/kwflag/
{}

bwrite:return
/kwflag/
{}

vn_io_fault_pgmove:return
/kwflag/
{}

ffs_balloc_ufs2:return
/kwflag/
{
	this->bp = *this->bpp;
	printf("bp:%p", this->bp);
        printf("\n\t\t\t\t\t      ");

	/***** bufobj *****/
	this->bufobj = this->bp->b_bufobj;
	printf("bufobj:%p private:%p bop_name:%s ops:",
		this->bufobj,
		this->bufobj->bo_private,
		stringof(this->bufobj->bo_ops->bop_name)
	);
	func((uintptr_t)this->bufobj->bo_ops);
        printf("\n\t\t\t\t\t      ");
	func((uintptr_t)this->bufobj->bo_ops->bop_write);
}

ffs_write:return
/kwflag/
{}

kern_pwritev:return
/execname == procname/
{
	trace(probename);
	kwflag = 0;
}

/********************************************************************************************/
END
{
        printf("-----IO Tracing END-----");
        printf("\n\t\t\t\t\t      ");
}

