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
ffs_read:entry
/execname == proc/
{
	self->ap = args[0];

	/************************* vnode *************************/
	self->vp = self->ap->a_vp;
	printf("vnode(%p):",
		self->vp
		);
	printf("\n\t\t\t\t\t      ");

	/************************* inode *************************/
	self->ip = (struct inode *)self->vp->v_data;
	printf("inode:number:%d",
		self->ip->i_number
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
	printf("vmobj(%p):type:%d size:%d handle:%p",
		self->vmobj,
		self->vmobj->type,
		self->vmobj->size,
		self->vmobj->handle
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

breadn_flags:entry
/execname == proc/
{
}

/*-----------------------------------------------------------------------------*/
breadn_flags:return
/execname == proc/
{
}

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
