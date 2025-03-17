#!/usr/sbin/dtrace -s

/*
 *      ./rhw_sysread.d -c './main'
 * 	or
 * 	make rhw_sysread
 */

#pragma D option flowindent

BEGIN
{
        proc = "main"
}

/*syscall*************************************************************************************/
sys_read:entry
/execname == proc/
{
}

/*-----------------------------------------------------------------------------*/
sys_read:return
/execname == proc/
{}

/*VFS File System*************************************************************************************/
cluster_read:entry
/execname == proc/
{}

vn_io_fault_pgmove:entry
/execname == proc/
{
}

/*-----------------------------------------------------------------------------*/
vn_io_fault_pgmove:return
/execname == proc/
{}

cluster_read:return
/execname == proc/
{}


/*FFS File System*************************************************************************************/
ffs_read:entry
/execname == proc/
{
	self->ap = args[0];

	/************************* uio *************************/
	self->uio = self->ap->a_uio;
	printf("uio:offset:%d resid:%d ioflag:%p",
		self->uio->uio_offset,
		self->uio->uio_resid,
		self->ap->a_ioflag
		);
	printf("\n\t\t\t\t\t      ");

	/************************* vnode *************************/
	self->vp = self->ap->a_vp;
	printf("vnode(%p):tag:%s type:%d",
		self->vp,
		stringof(self->vp->v_tag),
		self->vp->v_type
		);
	printf("\n\t\t\t\t\t      ");

	/************************* mount *************************/
	self->mnt = self->vp->v_mount;
	printf("mount:flag:%x",
		self->mnt->mnt_flag
		);
	printf("\n\t\t\t\t\t      ");

	/************************* inode *************************/
	self->ip = (struct inode *)self->vp->v_data;
	printf("inode:number:%d size:%d",
		self->ip->i_number,
		self->ip->i_size
		);
	printf("\n\t\t\t\t\t      ");

	/************************* bufobj *************************/
	self->bufobj = self->vp->v_bufobj;
	printf("bufobj:bsize:%d",
		self->bufobj.bo_bsize
		);
	printf("\n\t\t\t\t\t      ");

	/************************* vm_object *************************/
	self->vmobj = self->bufobj.bo_object;
	printf("vmobj(%p):type:%d size:%d handle:%p resident_page_count:%d",
		self->vmobj,
		self->vmobj->type,
		self->vmobj->size,
		self->vmobj->handle,
		self->vmobj->resident_page_count
		);
	printf("\n\t\t\t\t\t      ");

	/************************* vm_page *************************/
	self->memq = self->vmobj->memq;
	self->vmpg0 = self->memq.tqh_first;
	self->vmpg1 = self->vmpg0->listq.tqe_next;

	printf("vmpg0:pindex:%d phys_addr:%p flags:%p object:%p",
		self->vmpg0->pindex,
		self->vmpg0->phys_addr,
		self->vmpg0->flags,
		self->vmpg0->object
		);
	printf("\n\t\t\t\t\t      ");

	printf("vmpg1:pindex:%d phys_addr:%p flags:%p object:%p",
		self->vmpg1->pindex,
		self->vmpg1->phys_addr,
		self->vmpg1->flags,
		self->vmpg1->object
		);
}

/*-----------------------------------------------------------------------------*/
ffs_read:return
/execname == proc/
{}

/*HBA Driver(ahci)*************************************************************************************/
ahciaction:entry
/execname == proc/
{}

/*-----------------------------------------------------------------------------*/
ahciaction:return
/execname == proc/
{}
