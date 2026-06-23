#!/usr/sbin/dtrace -s

/*
 * 	./rhw.d -c './main'
 *      dtrace -n 'bdone:entry /execname == "intr"/ {stack();}'
 */

#pragma D option flowindent

BEGIN
{
	dmaplimit = *(vm_paddr_t *)0xffffffff83e15838;
	unmapped_buf = *(caddr_t *)0xffffffff83df55c0;
        proc = "main"
}

/*CAM*************************************************************************************/
adastrategy:entry
/execname == proc/
{}

adastart:entry
/execname == proc/
{
	self->periph = args[0];
	self->start_ccb = args[1];

	this->softc = (struct ada_softc *)self->periph->softc;

	printf("periph:%p start_ccb:%p",
		self->periph,
		self->start_ccb
		);
	printf("\n\t\t\t\t\t      ");

	printf("softc:state:%d",
		this->softc->state
		);
}

cam_iosched_next_bio:entry
/execname == proc/
{}

cam_fill_ataio:entry
/execname == proc/
{
}

/*-----------------------------------------------------------------------------*/

cam_fill_ataio:return
/execname == proc/
{}

cam_iosched_next_bio:return
/execname == proc/
{
	self->ret_biop = args[1];

	printf("biop(%p):cmd:%d flags:0x%x data:%p",
		self->ret_biop,
		self->ret_biop->bio_cmd,
		self->ret_biop->bio_flags,
		self->ret_biop->bio_data
		);
}

adastart:return
/execname == proc/
{}

adastrategy:return
/execname == proc/
{}

/*Dev Driver*************************************************************************************/
ahciaction:entry
/execname == proc/
{
	this->ccb = args[1];

	printf("ccb:func_code:0x%x",
		this->ccb->ccb_h.func_code
		);
}
/*-----------------------------------------------------------------------------*/
ahciaction:return
/execname == proc/
{
}

/*amd64*************************************************************************************/
uiomove_fromphys:entry
/execname == proc/
{
	this->offset = args[1];
	this->n = args[2];
	this->uio = args[3];

	printf("offset:%d n:%d",
		this->offset,
		this->n
		);
	printf("\n\t\t\t\t\t      ");

	printf("uio:resid:%d segflg:%x rw:%d iovcnt:%d",
		this->uio->uio_resid,
		this->uio->uio_segflg,
		this->uio->uio_rw,
		this->uio->uio_iovcnt
		);
	printf("\n\t\t\t\t\t      ");

	this->iov = this->uio->uio_iov;
	this->cnt = this->iov->iov_len;
	printf("iov:cnt:%d",
		this->cnt
		);
}

uiomove_fromphys:return
/execname == proc/
{
/*
	this->iov_val = copyinstr((uintptr_t)this->iov->iov_base-this->cnt);
	trace(this->iov_val);
*/
}

/*pmap*************************************************************************************/
pmap_map_io_transient:entry
/execname == proc/
{
	self->page = args[0];
	self->vaddrp = args[1];

	printf("paddr:%p", self->page[0]->phys_addr);
	printf("\n\t\t\t\t\t      ");

	printf("vaddr:%p",
		self->vaddrp
		);
	printf("\n\t\t\t\t\t      ");

	printf("dmaplimit:0x%x", dmaplimit);
}

pmap_qenter:entry
/execname == proc/
{
}
/*-------------------------------------------------------------------------------*/
pmap_qenter:return
/execname == proc/
{}

pmap_map_io_transient:return
/execname == proc/
{
	this->vaddr = *self->vaddrp;
	this->v = *(char *)this->vaddr;
	printf("vaddr:%x %x",
		this->vaddr,
		this->v
		);
	printf("\n\t\t\t\t\t      ");
}

/*common*************************************************************************************/
vn_io_fault_*move:*
/execname == proc/
{}

bdata2bio:entry
/execname == proc/
{
	self->bp = args[0];
	self->bip = args[1];

	printf("bp:data:%p unmapped_buf:%p",
		self->bp->b_data,
		unmapped_buf
		);
	printf("\n\t\t\t\t\t      ");

	printf("bip(%p):ma:%p ma_n:%d data:%p",
		self->bip,
		self->bip->bio_ma,
		self->bip->bio_ma_n,
		self->bip->bio_data
		);
}

bdata2bio:return
/execname == proc/
{
	printf("bip:ma:%p ma_n:%d data:%p",
		self->bip->bio_ma,
		self->bip->bio_ma_n,
		self->bip->bio_data
		);
}

g_vfs_strategy:entry
/execname == proc/
{

}

g_vfs_strategy:return
/execname == proc/
{}

ffs_geom_strategy:entry
/execname == proc/
{
}

ffs_geom_strategy:return
/execname == proc/
{}

ufs_strategy:entry
/execname == proc/
{
	self->ap = args[0];
	self->a_vp = self->ap->a_vp;

	self->ufsmount = (struct ufsmount *)self->a_vp->v_mount->mnt_data;
	printf("mount:%p",
		self->a_vp->v_mount
		);

	printf("\n\t\t\t\t\t      ");
	printf("ufs:fstype:%x mountp:%p",
		self->ufsmount->um_fstype,
		self->ufsmount->um_mountp
	);
	func((uintptr_t)self->ufsmount->um_bo->bo_ops);
}

ufs_strategy:return
/execname == proc/
{}

_vn_open:entry,
_vn_open:return
/execname == proc/
{}

_vn_open_cred:entry,
_vn_open_cred:return
/execname == proc/
{}

namei:entry,
namei:return
/execname == proc/
{}

lookup:entry,
lookup:return
/execname == proc/
{}

VOP_LOOKUP_APV:entry,
VOP_LOOKUP_APV:return
/execname == proc/
{}

vfs_cache_lookup:entry,
vfs_cache_lookup:return
/execname == proc/
{}

vn_io_fault_doio:entry
/execname == proc/
{
	this->args = args[0];

	/*
	 * vfs_vnops.c
	 * 0:VN_IO_FAULT_FOP  1:VN_IO_FAULT_VOP
	 */
	printf("args:kind:%d",
		this->args->kind
		);
}

vn_io_fault_doio:return
/execname == proc/
{
}

cluster_read:*
/execname == proc/
{
}

bufwait:*
/execname == proc/
{
}

/*VOP**********************************************************************************************************/
VOP_CACHEDLOOKUP_APV:*
/execname == proc/
{}

VOP_UNLOCKED_READ_APV:entry
/execname == proc/
{
	this->vop = args[0];

	printf("vop(%p):unlocked_read:%p bypass:%p default:%p",
		this->vop,
		this->vop->vop_unlocked_read,
		this->vop->vop_bypass,
		this->vop->vop_default
	);
	func((uintptr_t)this->vop);
	printf("\n\t\t\t\t\t      ");

	this->vop = this->vop->vop_default;
	printf("vop(%p):unlocked_read:%p bypass:%p default:%p",
		this->vop,
		this->vop->vop_unlocked_read,
		this->vop->vop_bypass,
		this->vop->vop_default
	);
	func((uintptr_t)this->vop);
	printf("\n\t\t\t\t\t      ");

	this->vop = this->vop->vop_default;
	printf("vop(%p):unlocked_read:%p bypass:%p default:%p",
		this->vop,
		this->vop->vop_unlocked_read,
		this->vop->vop_bypass,
		this->vop->vop_default
	);
	func((uintptr_t)this->vop);
	printf("\n\t\t\t\t\t      ");
}

VOP_READ_APV:entry
/execname == proc/
{
	this->vop = args[0];

	printf("vop(%p):read:%p bypass:%p default:%p",
		this->vop,
		this->vop->vop_read,
		this->vop->vop_bypass,
		this->vop->vop_default
	);
	func((uintptr_t)this->vop);
	printf("\n\t\t\t\t\t      ");
}

VOP_READ_APV:return
/execname == proc/
{}

VOP_UNLOCKED_READ_APV:return
/execname == proc/
{}

/*entry**************************************************************************************/

sys_openat:entry
/execname == proc/
{
	this->td = args[0];
	this->uap = args[1];

	printf("td:td_name:%s", this->td->td_name);
	printf("\n\t\t\t\t\t      ");

	printf("uap:fd:%d", this->uap->fd);
}

isi_kern_openat:entry
/execname == proc/
{
	this->td = args[0];
	this->fd = args[1];
	this->path = args[2];

	printf("fd:%d", this->fd);
	printf("\n\t\t\t\t\t      ");

	printf("path:%s", copyinstr((uintptr_t)this->path));
	printf("\n\t\t\t\t\t      ");

}

cache_lookup:entry
/execname == proc/
{
	this->dvp = args[0];

	this->inode = (struct inode *)this->dvp->v_data;

	printf("dvp(%p):v_tag:%s v_type:%d", this->dvp,
		stringof(this->dvp->v_tag),
		this->dvp->v_type
		);
	printf("\n\t\t\t\t\t      ");

	printf("inode:i_vnode:%p i_number:%d",
		this->inode->i_vnode,
		this->inode->i_number
		);
	printf("\n\t\t\t\t\t      ");

	this->din2 = this->inode->dinode_u.din2;
	printf("din2:di_blksize:%d",
		this->din2->di_size
               );
        printf("\n\t\t\t\t\t      ");

/************************************************************************/
	this->iump = this->inode->i_ump;
	this->umcp = this->iump->um_cp;
	this->geom = this->umcp->geom;
/*
 * Note: cp->pp->geom->cp
 * pp = cp->provider
 * gp = pp->geom
 * cp = LIST_FIRST(&gp->consumer)
 */
	printf("umcp:%p",
		this->umcp
		);
	printf("\n\t\t\t\t\t      ");

	/* VFS */
	this->vfs_csm = this->umcp;
	printf("csm:geom:%s \tclass:%s \tpvd:%s",
		stringof(this->geom->name),
		stringof(this->vfs_csm->geom->class->name),
		stringof(this->vfs_csm->provider->name)
	);
	printf("\n\t\t\t\t\t      ");

/************************************************************************/
	/* MIRROR */
	printf("------------------------------------------------------------------");
	printf("\n\t\t\t\t\t      ");
	this->mirror_pvd = this->umcp->provider;
	printf("pvd(%p):geom:%s \t\tclass:%s \t\t\tflags:0x%x gp:%p",
		this->mirror_pvd,
		stringof(this->mirror_pvd->geom->name),
		stringof(this->mirror_pvd->geom->class->name),
		this->mirror_pvd->flags,
		this->mirror_pvd->geom
	);
	printf("\n\t\t\t\t\t      ");

	this->mirror_csm = this->mirror_pvd->geom->consumer.lh_first;
	printf("csm(%p):geom:%s \t\tclass:%s \tpvd:%s \tflags:0x%x gp:%p",
		this->mirror_csm,
		stringof(this->mirror_csm->geom->name),
		stringof(this->mirror_csm->geom->class->name),
		stringof(this->mirror_csm->provider->name),
		this->mirror_csm->flags,
		this->mirror_csm->geom
	);
	printf("\n\t\t\t\t\t      ");

	this->mirror_geom = this->mirror_pvd->geom;
	printf("geom:%p \t\t\tclass:%s \tpvdp:%p \tcsmp:%p",
		this->mirror_geom,
		stringof(this->mirror_geom->class->name),
		this->mirror_geom->provider.lh_first,
		this->mirror_geom->consumer.lh_first
		);
	printf("\n\t\t\t\t\t      ");
	printf("------------------------------------------------------------------");
	printf("\n\t\t\t\t\t      ");

/************************************************************************/
	/* PART */
	/* md10p1 */
	this->mirror_csm2 = this->mirror_csm->consumer.le_next;
	this->part_pvd = this->mirror_csm->provider;
	printf("pvd:geom:%s \t\tclass:%s \tflags:0x%x",
		stringof(this->part_pvd->geom->name),
		stringof(this->part_pvd->geom->class->name),
		this->part_pvd->flags
	);
	printf("\n\t\t\t\t\t      ");

/************************************************************************/
	/* CDEV */
	this->umdev=this->iump->um_dev;
	this->dev_csm = ((struct g_consumer *)this->umdev->si_drv2);
	printf("csm:geom:%s \tclass:%s \tpvd:%s \tdrv2:%p(csm)->%p(pvd)",
		stringof(this->dev_csm->geom->name),
		stringof(this->dev_csm->geom->class->name),
		stringof(this->dev_csm->provider->name),
		this->dev_csm,
		this->dev_csm->provider
		);

/************************************************************************/
	/* md9p1 */
	if (0) {
		this->part_pvd2 = this->mirror_csm2->provider;
		printf("geom:%s \t\t\tclass:%s \tprovider:%s \tflags:0x%x",
			stringof(this->part_pvd2->geom->name),
			stringof(this->part_pvd2->geom->class->name),
			stringof(this->part_pvd2->name),
			this->part_pvd2->flags
		);
		printf("\n\t\t\t\t\t      ");
	}

/************************************************************************/
}

ufs_lookup_ino:entry
/execname == proc/
{
	this->vdp = args[0];
	this->vpp = args[1];
	this->cnp = args[2];
	this->dd_ino = args[3];

	printf("dd_ino:%p", this->dd_ino);
	printf("\n\t\t\t\t\t      ");

	printf("cnp:cn_nameptr:%s cn_nameptr:%s",
		stringof(this->cnp->cn_pnbuf),
		stringof(this->cnp->cn_nameptr)
		);
}

ffs_blkatoff:entry
/execname == proc/
{
	this->bpp = args[3];

	printf("bpp:0x%p(v:%p)", this->bpp, *this->bpp);
}

breada:entry
/execname == proc/
{}

bstrategy:entry
/execname == proc/
{
}

breadn_flags:entry
/execname == proc/
{
	this->vp = args[0];
	this->blkno = args[1];
	this->size = args[2];

	printf("vp:%p blkno:%d size:%d",
		this->vp,
		this->blkno,
		this->size
		);
	printf("\n\t\t\t\t\t      ");

	printf("inode:number:%d",
		((struct inode *)this->vp->v_data)->i_number);
	printf("\n\t\t\t\t\t      ");

/********************************* vm_page *************************************/
	this->vm_page0 = this->vp->v_bufobj.bo_object->memq.tqh_first;
	printf("vm_page(%p):vmobj:%p pindex:%d phys_addr:%p",
		this->vm_page0,
		this->vm_page0->object,
		this->vm_page0->pindex,
		this->vm_page0->phys_addr
		);
	printf("\n\t\t\t\t\t      ");

	printf("vm_page:astate:flags:0x%x queue:%d",
		this->vm_page0->a.flags,
		this->vm_page0->a.queue
		);
	printf("\n\t\t\t\t\t      ");

/********************************* ufsmount *************************************/
	this->ufsmount = (struct ufsmount *)this->vp->v_mount->mnt_data;
	printf("ufs:fstype:%x", this->ufsmount->um_fstype);
	/*func((uintptr_t)this->ufsmount->um_bo->bo_ops);*/

	/*stack();*/
}

getblkx:entry
/execname == proc/
{
	this->bpp = args[6];

	printf("bpp:%p",
		this->bpp
		);
}

getblk_core:entry
/execname == proc/
{
}

gbincore_unlocked:entry
/execname == proc/
{
}

getnewbuf:*,
allocbuf_flags:*
/execname == proc/
{
}

/*------------------------------------------------------------------------------------------*/
sys_read:entry
/execname == proc/
{
}

kern_readv:entry
/execname == proc/
{
/*https://man.freebsd.org/cgi/man.cgi?query=vnode&apropos=0&sektion=0&manpath=FreeBSD+14.2-RELEASE+and+Ports&arch=default&format=html*/
	this->td = args[0];

	printf("td:%p name:%s proc:%p",
		this->td,
		this->td->td_name,
		this->td->td_proc
		);
	printf("\n\t\t\t\t\t      ");

	this->proc = this->td->td_proc;
	printf("proc:name:%s pid:%d",
		this->proc->p_comm,
		this->proc->p_pid
		);
	printf("\n\t\t\t\t\t      ");

	this->ptextvp = this->proc->p_textvp;
	this->pvmspace = this->proc->p_vmspace;

/************************************* vnode ***************************************/
	printf("textvp(%p):tag:%s type:%d",
		this->ptextvp,
		stringof(this->ptextvp->v_tag),
		this->ptextvp->v_type
		);
	printf("\n\t\t\t\t\t      ");

/************************************* inode ***************************************/
	this->inode = (struct inode*)this->ptextvp->v_data;
	printf("inode:number:%d vnode:%p",
		this->inode->i_number,
		this->inode->i_vnode
		);
	printf("\n\t\t\t\t\t      ");

/************************************* ufsmount ***************************************/
	this->iump = this->inode->i_ump;
	this->um_bo = this->iump->um_bo;
	printf("ufsmount:bo:%p bo_ops:%p goem name:%s",
		this->iump->um_bo,
		this->iump->um_bo->bo_ops,
		stringof(((struct g_consumer*)this->iump->um_bo->bo_private)->geom->name)
		);
	printf("\n\t\t\t\t\t      ");

/************************************* bufobj ***************************************/
	this->vbufobj = this->ptextvp->v_bufobj;

	printf("bufobj:private:%p(vp) bsize:%d ops:%p bv_cnt:(d:%d c:%d)",
		this->vbufobj.bo_private,
		this->vbufobj.bo_bsize,
		this->vbufobj.bo_ops,
		this->vbufobj.bo_dirty.bv_cnt,
		this->vbufobj.bo_clean.bv_cnt
		);
	printf("\n\t\t\t\t\t      ");

/************************************* bufv ***************************************/
	this->bodirty =  this->vbufobj.bo_dirty;
	this->boclean =  this->vbufobj.bo_clean;
/*

	this->buf0 = this->bodirty.bv_hd.tqh_first;
	this->buf1 = this->buf0->b_bobufs.tqe_next;
	this->buf2 = this->buf1->b_bobufs.tqe_next;

	printf("dirty bufv:cnt:%d buf0:%p buf1:%p buf2:%p",
		this->bodirty.bv_cnt,
		this->buf0,
		this->buf1,
		this->buf2
		);
	printf("\n\t\t\t\t\t      ");

	printf("clean bufv:cnt:%d",
		this->boclean.bv_cnt
		);
	printf("\n\t\t\t\t\t      ");
*/

/************************************* buf0 ***************************************/
/*
	printf("buf0:bufobj:%p qindex:%d count:%d offset:%d blkno:%d npages:%d",
		this->buf0->b_bufobj,
		this->buf0->b_qindex,
		this->buf0->b_bcount,
		this->buf0->b_offset,
		this->buf0->b_blkno,
		this->buf0->b_npages
		);
	printf("\n\t\t\t\t\t      ");
	printf("buf0:data:%p",
		this->buf0->b_data
		);
	printf("\n\t\t\t\t\t      ");
*/
	/*trace(copyinstr((uintptr_t)this->buf0->b_data));*/

/************************************* buf1 ***************************************/
/*
	printf("buf1:bufobj:%p qindex:%d count:%d offset:%d blkno:%d npages:%d",
		this->buf1->b_bufobj,
		this->buf1->b_qindex,
		this->buf1->b_bcount,
		this->buf1->b_offset,
		this->buf1->b_blkno,
		this->buf0->b_npages
		);
	printf("\n\t\t\t\t\t      ");
*/

/************************************* vm_obj ***************************************/
	this->bobject = this->vbufobj.bo_object;

	printf("vmobject:size:%d",
		this->bobject->size
		);
	printf("\n\t\t\t\t\t      ");

	this->memq = this->bobject->memq;
	this->vmpg0 = this->memq.tqh_first;
	this->vmpg1 = this->vmpg0->listq.tqe_next;
	this->vmpg2 = this->vmpg1->listq.tqe_next;
	this->vmpg3 = this->vmpg2->listq.tqe_next;
	this->vmpg4 = this->vmpg3->listq.tqe_next;
	this->vmpg5 = this->vmpg4->listq.tqe_next;
	this->vmpg6 = this->vmpg5->listq.tqe_next;
	this->vmpg7 = this->vmpg6->listq.tqe_next;

	printf("memq:vmpg:0:%p 1:%p 2:%p 3:%p",
		this->vmpg0,
		this->vmpg1,
		this->vmpg2,
		this->vmpg3
		);
	printf("\n\t\t\t\t\t      ");

	printf("memq:vmpg:4:%p 5:%p 6:%p 7:%p",
		this->vmpg4,
		this->vmpg5,
		this->vmpg6,
		this->vmpg7
		);
	printf("\n\t\t\t\t\t      ");

	printf("vmpg0:pindex:%d flags:%x",
		this->vmpg0->pindex,
		this->vmpg0->flags
		);
	printf("\n\t\t\t\t\t      ");

	printf("vmpg1:pindex:%d flags:%x",
		this->vmpg1->pindex,
		this->vmpg1->flags
		);
	printf("\n\t\t\t\t\t      ");

}

vn_read:entry
/execname == proc/
{
	this->fp = args[0];

	printf("fp:type:%d vnode:%p",
		this->fp->f_type,
		this->fp->f_vnode
		);
}

/*sys/ufs/ffs/ffs_vnops.c*/
ffs_read:entry
/execname == proc/
{
	this->ap = args[0];

	this->vp = this->ap->a_vp;

/************************************* inode ***************************************/
	this->inode = (struct inode*)this->vp->v_data;
	printf("inode:number:%d vnode:%p",
		this->inode->i_number,
		this->inode->i_vnode
		);
	printf("\n\t\t\t\t\t      ");

/************************************* bufobj ***************************************/
	this->vbufobj = this->vp->v_bufobj;
	printf("bufobj:private:%p(vp) bsize:%d ops:%p bv_cnt:(d:%d c:%d)",
		this->vbufobj.bo_private,
		this->vbufobj.bo_bsize,
		this->vbufobj.bo_ops,
		this->vbufobj.bo_dirty.bv_cnt,
		this->vbufobj.bo_clean.bv_cnt
		);
	printf("\n\t\t\t\t\t      ");

/************************************* bufv ***************************************/
	this->bodirty =  this->vbufobj.bo_dirty;
	this->boclean =  this->vbufobj.bo_clean;

/*
	this->buf0 = this->bodirty.bv_hd.tqh_first;
	this->buf1 = this->buf0->b_bobufs.tqe_next;

	printf("dirty bufv:cnt:%d buf0:%p buf1:%p",
		this->bodirty.bv_cnt,
		this->buf0,
		this->buf1
		);
	printf("\n\t\t\t\t\t      ");

	printf("clean bufv:cnt:%d",
		this->boclean.bv_cnt
		);
	printf("\n\t\t\t\t\t      ");
*/

/************************************* buf0 ***************************************/
/*
	printf("buf0:bufobj:%p qindex:%d count:%d offset:%d blkno:%d npages:%d",
		this->buf0->b_bufobj,
		this->buf0->b_qindex,
		this->buf0->b_bcount,
		this->buf0->b_offset,
		this->buf0->b_blkno,
		this->buf0->b_npages
		);
	printf("\n\t\t\t\t\t      ");
*/
}

/*return*************************************************************************************/
ffs_read:return
/execname == proc/
{
/*
	printf("buf0:data:%p",
		this->buf0->b_data
		);
	printf("\n\t\t\t\t\t      ");
*/
}

vn_read:return
/execname == proc/
{
}

kern_readv:return
/execname == proc/
{
}

sys_read:return
/execname == proc/
{
}

/*------------------------------------------------------------------------------------------*/
gbincore_unlocked:return
/execname == proc/
{
	this->ret_bp = args[1];

	printf("ret:%p", this->ret_bp);
	printf("\n\t\t\t\t\t      ");

	printf("bp:%p data:%p flags:%x",
		this->ret_bp,
		this->ret_bp->b_data,
		this->ret_bp->b_flags
		);
	printf("\n\t\t\t\t\t      ");
}

getblk_core:return
/execname == proc/
{
}

getblkx:return
/execname == proc/
{
	this->ret_bp = *this->bpp;

	printf("ret:%d", args[1]);
	printf("\n\t\t\t\t\t      ");
	printf("bp:%p data:%p flags:%x",
		this->ret_bp,
		this->ret_bp->b_data,
		this->ret_bp->b_flags
		);
	func((uintptr_t)this->ret_bp->b_bufobj->bo_ops->bop_strategy);
	printf("\n\t\t\t\t\t      ");
}

breadn_flags:return
/execname == proc/
{
/*
	printf("vm_page(%p):vmobj:%p pindex:%d phys_addr:%p",
		this->vm_page0,
		this->vm_page0->object,
		this->vm_page0->pindex,
		this->vm_page0->phys_addr
		);
	printf("\n\t\t\t\t\t      ");

	printf("vm_page:astate:flags:0x%x queue:%d",
		this->vm_page0->a.flags,
		this->vm_page0->a.queue
		);
	printf("\n\t\t\t\t\t      ");
*/
}

breada:return
/execname == proc/
{}

bstrategy:return
/execname == proc/
{}

ffs_blkatoff:return
/execname == proc/
{
/*
	printf("ret:%d", args[1]);
	printf("\n\t\t\t\t\t      ");
	printf("bpp:0x%p(v:0x%p)", this->bpp, *this->bpp);
	printf("\n\t\t\t\t\t      ");

	this->bp = *this->bpp;
	this->ep = (struct direct *)((char *)this->bp->b_data + 0x4C);
	printf("ep:d_ino:%d d_reclen:%d d_namelen:%d, d_name:%s",
		this->ep->d_ino,
		this->ep->d_reclen,
		this->ep->d_namlen,
		this->ep->d_name);
*/
}

ufs_lookup_ino:return
/execname == proc/
{
	this->ret = args[1];
	this->dd_vp = *this->vpp;
	this->dd_ip = (struct inode *)this->dd_vp->v_data;

	printf("ret:%d", this->ret);
	printf("\n\t\t\t\t\t      ");

/********************************* data vnode *************************************/
	printf("data vnodep:%p tag:%s type:%d cachedd:%p",
		this->dd_vp,
		stringof(this->dd_vp->v_tag),
		this->dd_vp->v_type,
		this->dd_vp->v_cache_dd
		);
	printf("\n\t\t\t\t\t      ");

/********************************* bufobj *************************************/
	this->dd_v_bufobj = this->dd_vp->v_bufobj;
	this->dd_bo_clean = this->dd_v_bufobj.bo_clean;
	this->dd_bo_dirty = this->dd_v_bufobj.bo_dirty;

	printf("bufobj:bufv:clean:cnt:%d dirty:cnt:%d",
		this->dd_bo_clean.bv_cnt,
		this->dd_bo_dirty.bv_cnt
		);
	printf("\n\t\t\t\t\t      ");

/********************************* buf *************************************/
	this->dd_buf = this->dd_bo_dirty.bv_hd.tqh_first;
	printf("buf: bcount:%d bdata:%p iocmd:%d blkno:%d offset:%d qindex:%d",
		this->dd_buf->b_bcount,
		this->dd_buf->b_data,
		this->dd_buf->b_iocmd,
		this->dd_buf->b_blkno,
		this->dd_buf->b_offset,
		this->dd_buf->b_qindex
		);
	printf("\n\t\t\t\t\t      ");

/********************************* vm_object *************************************/
	this->dd_bo_object = this->dd_v_bufobj.bo_object;
	printf("vm_object:%p:size:%d rpc:%d",
		this->dd_bo_object,
		this->dd_bo_object->size,
		this->dd_bo_object->resident_page_count
		);
	printf("\n\t\t\t\t\t      ");

/********************************* vm_page *************************************/
	this->vm_page0 = this->dd_bo_object->memq.tqh_first;
	printf("vm_page(%p):vmobj:%p pindex:%d phys_addr:%p",
		this->vm_page0,
		this->vm_page0->object,
		this->vm_page0->pindex,
		this->vm_page0->phys_addr
		);
	printf("\n\t\t\t\t\t      ");

	printf("vm_page:astate:flags:0x%x queue:%d",
		this->vm_page0->a.flags,
		this->vm_page0->a.queue
		);
	printf("\n\t\t\t\t\t      ");

/********************************* data inode *************************************/
	printf("data inodep:%p vnodep:%p",
		this->dd_ip,
		this->dd_ip->i_vnode
		);
	printf("\n\t\t\t\t\t      ");

	printf("number:%d",
		this->dd_ip->i_number
		);
/*********************************************************************************/
}

cache_lookup:return
/execname == proc/
{
	printf("ret:%d", args[1]);
}

isi_kern_openat:return
/execname == proc/
{}

sys_openat:return
/execname == proc/
{}
