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

       /*
        * cp /boot/kernel.amd64/kernel.gz ~/
        * gunzip kernel.gz
        * nm ~/kernel | grep -w unmapped_buf
        */
	unmapped_buf = *(caddr_t *)0xffffffff83df55c0;

	printf("\n\t\t\t\t\t      ");
}

/*intr**************************************************************************/
adadone:entry
{
	self->done_ccb = args[1];

	this->adadone_bp = (struct bio *)self->done_ccb->ccb_h.periph_priv.entries[1].ptr;
	printf("bio:%p",
		this->adadone_bp
		);
}

bufdone:entry
{
	trace(execname);
}

bufdone_finish:entry
{
	self->bp = args[0];

	printf("bp:%p flags:%x",
		self->bp,
		self->bp->b_flags
		);
}

bdone:entry
{
	/*stack(50);*/
}

/***************************************************************************/
bdone:return
{}

bufdone_finish:return
{}

bufdone:return
{}

adadone:return
{}

/*data copy -- critical**************************************************************************/
uiomove_fromphys:entry
/execname == "main" && ffsread/
{
	self->offset = args[1];
	self->n =args[2];
	self->uio = args[3];
	self->iov_base = self->uio->uio_iov->iov_base;

	printf("offset:%d n:%d",
		self->offset,
		self->n
		);

	printf("\n\t\t\t\t\t      ");
	printf("uio(%p):resid:%d iov_base:%p",
		self->uio,
		self->uio->uio_resid,
		self->iov_base
		);
	/*trace(copyinstr((uintptr_t)self->iov_base));*/
}

pmap_map_io_transient:entry
/execname == "main" && ffsread/
{
	self->ppage = args[0];
	self->pvaddr = args[1];

	printf("page:%p pindex:%d phys_addr:%p",
		self->ppage[0],
		self->ppage[0]->pindex,
		self->ppage[0]->phys_addr
		);
}

g_vfs_strategy:entry
/execname == "main" && ffsread/
{
	self->bp = args[1];

	printf("bp:%p bio:%p pages:%p npages:%d",
		self->bp,
		self->bp->b_bio,
		(uintptr_t *)self->bp->b_pages,
		self->bp->b_npages
		);
}

/***************************************************************************/
g_vfs_strategy:return
/execname == "main" && ffsread/
{
	this->ret_bip = self->bp->b_bio;

	printf("bp:%p b_bio:%p",
		self->bp,
		self->bp->b_bio
		);

	printf("\n\t\t\t\t\t      ");
	printf("bio:ma:%p ma_n:%d",
		this->ret_bip->bio_ma,
		this->ret_bip->bio_ma_n
		);
}

pmap_map_io_transient:return
/execname == "main" && ffsread/
{
	printf("vaddr:%p",
		*self->pvaddr
		);

	/*trace(stringof(*self->pvaddr));*/
}

uiomove_fromphys:return
/execname == "main" && ffsread/
{
	trace(probename);
	/*trace(copyinstr((uintptr_t)self->iov_base));*/
}

/*syscall**************************************************************************/
kern_preadv:entry
/execname == "main"/
{
	self->auio = args[2];

	self->iov_base = self->auio->uio_iov->iov_base;

	printf("auio(%p):resid:%d iov_base:%p",
		self->auio,
		self->auio->uio_resid,
		self->iov_base
		);

	/*trace(copyinstr((uintptr_t)self->auio->uio_iov->iov_base));*/
	/*trace(copyinstr((uintptr_t)self->iov_base));*/
}

/***************************************************************************/
kern_preadv:return
/execname == "main"/
{
	printf("auio(%p):resid:%d iov_base:%p",
		self->auio,
		self->auio->uio_resid,
		self->iov_base
		);

	/*trace(copyinstr((uintptr_t)self->iov_base));*/
}

/*vfs_vnops**************************************************************************/
vn_io_fault_doio:entry
/execname == "main"/
{
	self->args = args[0];
	self->uio = args[1];

	printf("args:kind:%d",
		self->args->kind
		);
	printf("\n\t\t\t\t\t      ");

	printf("uio(%p)",
		self->uio
		);

	func((uintptr_t)self->args->args.fop_args.doio);
}
/***************************************************************************/
vn_io_fault_doio:return
/execname == "main"/
{}

/***************************************************************************/
getblkx:*,
getblk_traced:*
/execname == "main"/
{}

adastrategy:entry
/execname == "main"/
{
	self->biop = args[0];

	printf("bio:%p ma:%p ma_n:%d",
		self->biop,
		self->biop->bio_ma,
		self->biop->bio_ma_n
		);

	printf("\n\t\t\t\t\t      ");
	this->page = self->biop->bio_ma[0];
	printf("page:%p pindex:%d phys_addr:%p", this->page, this->page->pindex, this->page->phys_addr);

	printf("\n\t\t\t\t\t      ");
	this->page = self->biop->bio_ma[1];
	printf("page:%p pindex:%d phys_addr:%p", this->page, this->page->pindex, this->page->phys_addr);

	printf("\n\t\t\t\t\t      ");
	this->page = self->biop->bio_ma[2];
	printf("page:%p pindex:%d phys_addr:%p", this->page, this->page->pindex, this->page->phys_addr);

	printf("\n\t\t\t\t\t      ");
	this->page = self->biop->bio_ma[3];
	printf("page:%p pindex:%d phys_addr:%p", this->page, this->page->pindex, this->page->phys_addr);

	/*stack(50);*/
}

adastart:entry
/execname == "main"/
{
	self->done_ccb = args[1];

	this->as_bp = (struct bio *)self->done_ccb->ccb_h.periph_priv.entries[1].ptr;
	printf("bio:%p",
		this->as_bp
		);
}

/***************************************************************************/
adastart:return
/execname == "main"/
{
	this->as_bp = (struct bio *)self->done_ccb->ccb_h.periph_priv.entries[1].ptr;
	printf("bio:%p",
		this->as_bp
		);
}

adastrategy:return
/execname == "main"/
{}

/*buf**************************************************************************/

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

bufstrategy:entry
/execname == "main"/
{
	self->bp = args[1];

	this->vp = self->bp->b_vp;

	printf("vp:%p",
		this->vp
		);
}

bufwait:entry
/execname == "main"/
{
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
	printf("obj:%p size:%d res_pg_cnt:%d handle:%p",
		this->obj,
		this->obj->size,
		this->obj->resident_page_count,
		this->obj->handle
		);
	printf("\n\t\t\t\t\t      ");

	this->un_pager = this->obj->un_pager;
	this->vnp = this->un_pager.vnp;
	printf("vnp:size:%d, writemappings:%d",
		this->vnp.vnp_size,
		this->vnp.writemappings
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

getnewbuf:entry
/execname == "main"/
{
}

/***************************************************************************/
getnewbuf:return
/execname == "main"/
{
	self->gu_ret_bp = args[1];

	printf("bp:%p",
		self->gu_ret_bp
		);
}

gbincore_unlocked:return
/execname == "main"/
{
	self->gu_ret_bp = args[1];

	printf("bp:%p",
		self->gu_ret_bp
		);
	if (self->gu_ret_bp) {

		printf("bp:%p flags:%x",
			self->gu_ret_bp,
			self->gu_ret_bp->b_flags
			);
	}
}

getblk_core:return
/execname == "main"/
{
	this->bp = *self->bpp;

	printf("bp:%p flags:%x data:%p",
		this->bp,
		this->bp->b_flags,
		this->bp->b_data
		);

	printf("\n\t\t\t\t\t      ");
	printf("unmapped_buf:%p",
		unmapped_buf
		);
}

cluster_read:return
/execname == "main" && ffsread/
{}

bufwait:return
/execname == "main"/
{
}

bufstrategy:return
/execname == "main"/
{}

breadn_flags:return
/execname == "main"/
{
}

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

