#!/usr/sbin/dtrace -s

/*
 *      ./rhw_cam.d -c './main'
 */

#pragma D option flowindent

BEGIN
{
        proc = "main";
	sysread = 0;
}

/*sys*************************************************************************************/
/* probemod:probefunc:probename */
kernel:sys_read:entry
/execname == proc/
{
	self->td = args[0];
	self->uap = args[1];

	trace(probename);
	printf("\n\t\t\t\t\t      ");

	sysread = 1;

	printf("uap:buf:%p nbyte:%d",
		self->uap->buf,
		self->uap->nbyte
		);
}


/*FFS*************************************************************************************/
ffs_read:entry
/execname == proc && sysread/
{
	self->ap = args[0];

	this->vp = self->ap->a_vp;
	this->uio = self->ap->a_uio;
	this->ip = (struct inode *)this->vp->v_data;
	this->fs = this->ip->i_ump->um_fs;

	printf("uio:offset:%d",
		this->uio->uio_offset
		);
	printf("\n\t\t\t\t\t      ");

	/************************inode************************/
	printf("ip:size:%d",
		this->ip->i_size
		);
	printf("\n\t\t\t\t\t      ");

	/************************inode************************/
	printf("fs:bshift:%d qbmask:%p",
		this->fs->fs_bshift,
		this->fs->fs_qbmask
		);
	printf("\n\t\t\t\t\t      ");

}

breadn_flags:entry
/execname == proc && sysread/
{
	self->vp = args[0];
	self->blkno = args[1];
	self->size = args[2];

	printf("lblkno:%d size:%d",
		self->blkno,
		self->size
		);
}

cluster_read:entry
/execname == proc && sysread/
{
	self->vp = args[0];
	self->filesize = args[1];
	self->lblkno = args[2];
	self->size = args[3];

	printf("lblkno:%d size:%d filesize:%d",
		self->lblkno,
		self->size,
		self->filesize
		);
}

getblkx:entry
/execname == proc && sysread/
{
        self->bpp = args[6];

        printf("bpp:%p",
                self->bpp
                );
}

bufstrategy:entry
/execname == proc && sysread/
{}

/*-----------------------------------------------------------------------------*/
bufstrategy:return
/execname == proc && sysread/
{}

getblkx:return
/execname == proc && sysread/
{
        self->ret_bp = *self->bpp;

        printf("ret:%d", args[1]);
        printf("\n\t\t\t\t\t      ");
        printf("bp:%p data:%p flags:%x",
                self->ret_bp,
                self->ret_bp->b_data,
                self->ret_bp->b_flags
                );
        func((uintptr_t)self->ret_bp->b_bufobj->bo_ops->bop_strategy);
}

cluster_read:return
/execname == proc && sysread/
{}

breadn_flags:return
/execname == proc && sysread/
{}

ffs_read:return
/execname == proc && sysread/
{}

kernel:sys_read:return
/execname == proc/
{
	trace(probename);
	printf("\n\t\t\t\t\t      ");

	sysread = 0;
}

