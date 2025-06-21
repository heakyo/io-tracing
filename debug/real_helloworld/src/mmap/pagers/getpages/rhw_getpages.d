#!/usr/sbin/dtrace -s

/*
 *      ./rhw_template.d -c './main'
 */

#pragma D option flowindent

BEGIN
{
        proc = "main";

	sysopenat = 0;
	sysread = 0;
	sysmmap= 0;
	vpgergetpges = 0;
	ffsread = 0;
	printf("\n\t\t\t\t\t      ");
}

/***************************************************************************/
getblkx:*,
getblk_traced:*
/execname == "main"/
{}

adastrategy:entry
/execname == "main"/
{
	stack(50);
}
/***************************************************************************/
adastrategy:return
/execname == "main"/
{}

/*buf**************************************************************************/
bufdone_finish:entry
{
	self->bp = args[0];

	printf("bp:%p flags:%x",
		self->bp,
		self->bp->b_flags
		);

	trace(execname);
}
/***************************************************************************/
bufdone_finish:return
{
	printf("bp:%p flags:%x",
		self->bp,
		self->bp->b_flags
		);

	trace(execname);
}

namei:entry
/execname == "main"/
{
	self->ndp = args[0];

	this->cnp = self->ndp->ni_cnd;

	printf("ndp: dirp:%p %s vp:%p",
		self->ndp->ni_dirp,
		copyinstr((uint_t)self->ndp->ni_dirp),
		self->ndp->ni_vp
		);
	printf("\n\t\t\t\t\t      ");

	this->vp = self->ndp->ni_vp;
	printf("vp:tag:%p(%s)",
		this->vp->v_tag,
		stringof(this->vp->v_tag)
		);
	printf("\n\t\t\t\t\t      ");

	printf("cnp: pnbuf:%p",
		this->cnp.cn_pnbuf
		/*stringof((uint_t)this->cnp.cn_pnbuf)*/
		);
}
/**************************************************************************************/
namei:return
/execname == "main"/
{
	printf("vp:tag:%p %s",
		this->vp->v_tag,
		stringof(this->vp->v_tag)
		);
}

/*file system**************************************************************************/
ffs_read:entry
/execname == "main"/
{
	self->ap = args[0];

	this->vp = self->ap->a_vp;
	this->uio = self->ap->a_uio;
	this->seqcount = self->ap->a_ioflag;
	this->ip = (struct inode *)(this->vp)->v_data;
	this->fs = this->ip->i_ump->um_fs;

	printf("uio:rw:%d offset:%d",
		this->uio->uio_rw,
		this->uio->uio_offset
		);
	printf("\n\t\t\t\t\t      ");

	printf("ip:size:%d",
		this->ip->i_size
		);
	printf("\n\t\t\t\t\t      ");

	printf("fs:bshift:%d",
		this->fs->fs_bshift
		);
	printf("\n\t\t\t\t\t      ");

	printf("ap:ioflag:%x",
		this->seqcount
		);
	printf("\n\t\t\t\t\t      ");

	ffsread = 1;
}

breadn_flags:entry
/execname == "main"/
{
	self->blkno = args[1];
	self->rablkno = args[3];
	self->cnt =args[5];

	printf("blkno:%d",
		self->blkno
		);
	printf("\n\t\t\t\t\t      ");

	printf("rablkno:%p cnt:%d",
		self->rablkno,
		self->cnt
		);
}

cluster_read:entry
/execname == "main" && ffsread/
{
	self->vp = args[0];
	self->lblkno = args[2];

	printf("vp:%p tag:%p %s",
		self->vp,
		self->vp->v_tag,
		stringof(self->vp->v_tag)
		);
	printf("\n\t\t\t\t\t      ");

	printf("lblkno:%d",
		self->lblkno
		);
}

getblk_core:entry
/execname == "main"/
{
	self->vp = args[0];
	self->bpp = args[8];

	printf("vp:%p tag:%p %s",
		self->vp,
		self->vp->v_tag,
		stringof(self->vp->v_tag)
		);
	printf("\n\t\t\t\t\t      ");

	this->obj = self->vp->v_bufobj.bo_object;
	printf("obj:%p size:%d res_pg_cnt:%d",
		this->obj,
		this->obj->size,
		this->obj->resident_page_count
		);
	/*stack();*/
}

gbincore_unlocked:entry
/execname == "main"/
{
	self->bo = args[0];
	self->lblkno = args[1];

	printf("bo:%p lblkno:%d",
		self->bo,
		self->lblkno
		);
	printf("\n\t\t\t\t\t      ");

	this->bo_clean = self->bo->bo_clean;
	printf("bo_clean:cnt:%d",
		this->bo_clean.bv_cnt
		);
	printf("\n\t\t\t\t\t      ");

	this->bo_dirty = self->bo->bo_dirty;
	printf("bo_dirty:cnt:%d",
		this->bo_dirty.bv_cnt
		);
}

/***************************************************************************/
gbincore_unlocked:return
/execname == "main"/
{
	self->gu_ret_bp = args[1];

	if (self->gu_ret_bp) {

		printf("bp:%p flags:%x",
			self->gu_ret_bp,
			self->gu_ret_bp->b_flags
			);
	}
	trace(probename);
}

getblk_core:return
/execname == "main"/
{
	this->bp = *self->bpp;

	printf("bp:%p flags:%x",
		this->bp,
		this->bp->b_flags
		);
}

breadn_flags:return
/execname == "main"/
{
}

cluster_read:return
/execname == "main" && ffsread/
{}

ffs_read:return
/execname == "main"/
{
	trace(probename);
	ffsread = 0;
}

/*syscall**************************************************************************/
sys_openat:entry,
sys_close:entry
/execname == "main"/
{
	trace(probename);

	if(probefunc == "sys_openat") {
		sysopenat = 1;
	}
}

sys_read:entry,
sys_mmap:entry
/execname == "main" && sysopenat/
{
	trace(probename);

	if (probefunc == "sys_read") {
		sysread = 1;
	}

	if(probefunc == "sys_mmap") {
		sysmmap = 1;
	}
}

/***************************************************************************/
sys_read:return,
sys_mmap:return
/execname == "main" && sysopenat/
{
	trace(probename);

	if (probefunc == "sys_read") {
		sysread = 0;
	}

	if(probefunc == "sys_mmap") {
		sysmmap = 0;
	}
}

sys_openat:return,
sys_close:return
/execname == "main" && sysopenat/
{
	trace(probename);

	if(probefunc == "sys_close") {
		sysopenat = 0;
	}
}

/*Intr**************************************************************************/
vm_fault:entry
/execname == "main" && sysopenat/
{
	printf("td:name:%s pflags:0x%x",
		curthread->td_name,
		curthread->td_pflags
		);
}

vm_fault_lookup:entry
/execname == "main" && sysopenat/
{
	self->fs = args[0];

	printf("fs:%p:fault_type:%d",
		self->fs,
		self->fs->fault_type
		);
}

vm_fault_getpages:entry
/execname == "main" && sysopenat/
{
	trace(execname);
}

/******* Important ********/
vnode_pager_getpages:entry
/execname == "main" && sysopenat/
{
	vpgergetpges = 1;
	trace(execname);
}

/*-----------------------------------------------------------------------------*/
vnode_pager_getpages:return
/execname == "main" && sysopenat/
{
	vpgergetpges = 0;
	trace(execname);
}

vm_fault_getpages:return
/execname == "main" && sysopenat/
{}

vm_fault_lookup:return
/execname == "main" && sysopenat/
{
	printf("fs:%p:first_object:%p, m:%p",
		self->fs,
		self->fs->first_object,
		self->fs->m
		);
	printf("\n\t\t\t\t\t      ");

	this->fsobj = self->fs->first_object;
	printf("fsobj:type:%d size:%d",
		this->fsobj->type,
		this->fsobj->size
		);
}

vm_fault:return
/execname == "main" && sysopenat/
{
	printf("ret:%d", args[1]);
}

