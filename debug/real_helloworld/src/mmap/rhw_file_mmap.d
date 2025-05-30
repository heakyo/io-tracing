#!/usr/sbin/dtrace -s

/*
 *      ./rhw_file_mmap.d -c './main'
 */

/* probemod:probefunc:probename */

#pragma D option flowindent

BEGIN
{
        proc = "main";
	filesz = 32*1024;

	sysmmap = 0;
}

vnode_pager_getpages:*,
vnode_create_vobject:*,
VOP_OPEN:*,
sys_openat:*
/execname == proc/
{
}

/*Int**********************************************************************/
vm_fault:entry
/0&&execname == proc/
{
	trace(execname);
}

vm_pager_get_pages:entry
{}
/***********************************************************************/
vm_pager_get_pages:return
{
}

vm_fault:return
/0&&execname == proc/
{
}



vnode_pager_alloc:entry
/execname == proc/
{
	this->handle = args[0];

	self->vp = (struct vnode *)this->handle;
	this->v_object = self->vp->v_bufobj.bo_object;

	printf("handle:%p v_object:%p",
		this->handle,
		this->v_object
		);
}

vnode_pager_alloc:return
/execname == proc/
{
	this->v_object = self->vp->v_bufobj.bo_object;
	printf("ret:vp:%p v_object:%p",
		self->vp,
		this->v_object
		);
}

namei:entry
/execname == proc/
{
	this->ndp = args[0];
	printf("ndp:ni_vp:%p",
		this->ndp->ni_vp
		);
}

namei:return
/execname == proc/
{
	printf("ndp:ni_vp:%p",
		this->ndp->ni_vp
		);
}

isi_kern_openat:entry
/execname == proc/
{
	this->fd = args[1];
	this->path = args[2];
	this->fhp = args[3];

	printf("fd:%d path:%s fph:%p",
		this->fd,
		copyinstr((uintptr_t)this->path),
		this->fhp
		);
}

_vn_open:entry
/execname == proc/
{
	this->fp = args[6];

	trace(probename);
}

ufs_open:entry
/execname == proc/
{
	this->ap = args[0];
	this->vp = this->ap->a_vp;

	printf("vp:%p object:%p",
		this->vp,
		this->vp->v_bufobj.bo_object
		);
}


/************************************************************************************************/
ufs_open:return
/execname == proc/
{}

_vn_open:return
/execname == proc/
{
}

isi_kern_openat:return
/execname == proc/
{}

/*systemcall*************************************************************************************/
sys_mmap:entry
/execname == proc && args[1]->len == filesz/
{
	this->uap = args[1];

	printf("uap:prot:%x len:%d",
		this->uap->prot,
		this->uap->len
		);

	sysmmap = 1;
}

fo_mmap:entry
/execname == proc && sysmmap/
{
	this->fp = args[0];

	this->vp = this->fp->f_vnode;

	printf("fp:vp:%p",
		this->vp);
	printf("\n\t\t\t\t\t      ");

	printf("vp:object:%p",
		this->vp->v_bufobj.bo_object
		);

	func((uintptr_t)this->fp->f_ops->fo_mmap);
}

vm_mmap_vnode:entry
/execname == proc && sysmmap/
{
	this->vp = args[5];

	this->vmobj = this->vp->v_bufobj.bo_object;

	printf("vp: type:%d",
		this->vp->v_type
		);
	printf("\n\t\t\t\t\t      ");

	printf("vmobj:%p type:%d",
		this->vmobj,
		this->vmobj->type
		);
}

vm_mmap_object:entry
/execname == proc && sysmmap/
{}

vm_object_allocate:entry
/execname == proc/
{
	this->type = args[0];
	this->size = args[1];

	printf("type:%d size:%d",
		this->type,
		this->size
		);
	/*stack();*/
}

/******************************************************************************************/
vm_object_allocate:return
/execname == proc/
{}

vm_mmap_object:return
/execname == proc && sysmmap/
{}

vm_mmap_vnode:return
/execname == proc && sysmmap/
{}

fo_mmap:return
/execname == proc && sysmmap/
{}

sys_mmap:return
/execname == proc && sysmmap/
{
	sysmmap = 0;
	trace(probename);
}
