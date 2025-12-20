#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./nullfs-test.d -c 'mymount_nullfs /usr/src /mnt/src' && umount /mnt/src
 */

#pragma D option flowindent

BEGIN
{
	proc="cat";

	vfsdomount_flag = 0;

	printf("-----IO Tracing Start-----");
        printf("\n\t\t\t\t\t      ");
}
/*common*************************************************************************************/
/*nullfs_*:**/
/*{}*/

/*User Space*************************************************************************************/
pid$target:libc.so.7:nmount:entry
{
	this->iov = (struct iovec *)arg0;
	this->iovlen = arg1;

	printf("iov:0x%p len:%d",
		this->iov,
		this->iovlen
		);
}

/*return*************************************************************************************/
pid$target:libc.so.7:nmount:return
{
}

/*entry**************************************************************************************/
sys_nmount:entry
{
	this->uap = args[1];

	printf("uap:iovp:0x%p iovcnt:%d flags:0x%x",
		this->uap->iovp,
		this->uap->iovcnt,
		this->uap->flags
		);
	printf("\n\t\t\t\t\t      ");
}

vfs_donmount:entry
{
	this->fsoptions = args[2];

	printf("fsoptions(auio):0x%p iov:0x%p, iovcnt:%d, offset:%d resid:%d",
		this->fsoptions,
		this->fsoptions->uio_iov,
		this->fsoptions->uio_iovcnt,
		this->fsoptions->uio_offset,
		this->fsoptions->uio_resid
		);
	printf("\n\t\t\t\t\t      ");

	printf("iovec::%s:%s",
		copyinstr((uintptr_t)this->fsoptions->uio_iov[0].iov_base),
		copyinstr((uintptr_t)this->fsoptions->uio_iov[1].iov_base)
		);

	vfsdomount_flags = 1;
}

vfs_domount_first:entry
{
}

vfs_mount_alloc:entry
{
	this->vma_vp = args[0];
	this->vma_vfsp = args[1];
	this->vma_fspath = args[2];

	printf("args:vp:0x%p vfsp:0x%p fspath:%s",
		this->vma_vp,
		this->vma_vfsp,
		stringof(this->vma_fspath)
		);
	printf("\n\t\t\t\t\t      ");

	printf("vfsconf:name:%s vfsops:0x%p",
		this->vma_vfsp->vfc_name,
		this->vma_vfsp->vfc_vfsops
		);
}

namei:entry
/vfsdomount_flags/
{
	this->ndp = args[0];

	printf("ndp:vp:%p, dirp:%s",
		this->ndp->ni_vp,
		stringof(this->ndp->ni_dirp)
		);
	printf("\n\t\t\t\t\t      ");
}

nullfs_mount:entry
{
}

null_nodeget:entry
{
}

getnewvnode:entry
{

}

nullfs_statfs:entry
{
}

nullfs_root:entry
{
	stack();
}

ufs_open:entry
/execname == proc/
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
{}

ufs_open:return
/execname == proc/
{}

nullfs_root:return
{}

nullfs_statfs:return
{}

getnewvnode:return
{}

null_nodeget:return
{}

nullfs_mount:return
{}

namei:return
/vfsdomount_flags/
{
	printf("%s---------", probename);
	printf("\n\t\t\t\t\t      ");

	printf("ndp:ni_vp:%p",
		this->ndp->ni_vp
		);

	printf("\n\t\t\t\t\t      ");
	this->ret_ni_vp = this->ndp->ni_vp;
	printf("ret_ni_vp:");

/*****************************vnode*************************************/
        printf("ni_vp:tag:%s type:%d",
                stringof(this->ret_ni_vp->v_tag),
                this->ret_ni_vp->v_type
                );
        printf("\n\t\t\t\t\t      ");

/*****************************inode*************************************/
        this->inode = (struct inode*)this->ret_ni_vp->v_data;
        printf("inode:number:%d",
                this->inode->i_number
                );

        printf("\n\t\t\t\t\t      ");

}

vfs_mount_alloc:return
{
	this->ret_vma_mp = args[1];

	printf("mount:vnodecoverd:0x%p",
		this->ret_vma_mp->mnt_vnodecovered
		);
}

vfs_domount_first:return
{
}

vfs_donmount:return
{
	printf("\n\t\t\t\t\t      ");
	vfsdomount_flags = 0;
}

sys_nmount:return
{
}
/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
	printf("\n\t\t\t\t\t      ");
}
