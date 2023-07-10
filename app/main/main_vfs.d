#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./main_vfs.d -c './main -r /mnt/data15'
 *      ./main_vfs.d -c './main -o 1024 -s 8192 -r /mnt/data15'
 */

#pragma D option flowindent

BEGIN
{
	printf("-----IO Tracing Start-----");

	proc = "main";
	unmapped_bufp = (long *)0xffffffff83ea6cd0;

	printf("\n\t\t\t\t\t      ");
	printf("unmapped_buf:0x%p", *unmapped_bufp);
}
/*common*************************************************************************************/

/*entry**************************************************************************************/
ufs_lookup_ino:entry
/execname == proc/
{}

ffs_read:entry
/execname == proc/
{
	this->fr_ap = args[0];

	this->fr_ap_vp = this->fr_ap->a_vp;

	printf("vop_read_args:vp:%p uio:%p",
			this->fr_ap->a_vp,
			this->fr_ap->a_uio
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_a_desc = this->fr_ap->a_gen.a_desc;
	printf("vnodeop_desc:%p:name:%s call:%p",
			this->fr_a_desc,
			stringof(this->fr_a_desc->vdesc_name),
			this->fr_a_desc->vdesc_call
			);
	printf("\n\t\t\t\t\t      ");
	func((uintptr_t)this->fr_a_desc->vdesc_call);
	printf("\n\t\t\t\t\t      ");
	func((uintptr_t)this->fr_a_desc);

	printf("\n\t\t\t\t\t      ");
	this->fr_a_uio = this->fr_ap->a_uio;
	printf("uio:iovcnt:%d offset:%d resid:%d segflg:%d rw:%d td:%p",
			this->fr_a_uio->uio_iovcnt,
			this->fr_a_uio->uio_offset,
			this->fr_a_uio->uio_resid,
			this->fr_a_uio->uio_segflg,
			this->fr_a_uio->uio_rw,
			this->fr_a_uio->uio_td
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_uio_iov = this->fr_a_uio->uio_iov;
	self->fr_iov_base = this->fr_uio_iov->iov_base;
	self->fr_iov_len = this->fr_uio_iov->iov_len;
	printf("iovec:base:%p len:%d",
			self->fr_iov_base,
			self->fr_iov_len
			);

	printf("\n\t\t\t\t\t      ");
	printf("vnode:%p:tag:%s type:%d data:%p op:%p mount:%p bufobj:%p",
			this->fr_ap_vp,
			stringof(this->fr_ap_vp->v_tag),
			this->fr_ap_vp->v_type,
			this->fr_ap_vp->v_data,
			this->fr_ap_vp->v_op,
			this->fr_ap_vp->v_mount,
			&this->fr_ap_vp->v_bufobj
			);

	printf("\n\t\t\t\t\t      ");
	printf("vnode:usecount:%d writecount:%d refcount:%d vnlock:%p v_lock:%p",
			this->fr_ap_vp->v_usecount,
			this->fr_ap_vp->v_writecount,
			this->fr_ap_vp->v_holdcnt,
			this->fr_ap_vp->v_vnlock,
			&this->fr_ap_vp->v_lock
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_v_vnlock = this->fr_ap_vp->v_vnlock;
	printf("lock:object:%p lock:(%p %p)",
			&this->fr_v_vnlock->lock_object,
			this->fr_v_vnlock->lk_lock,
			curthread
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_lo = &this->fr_v_vnlock->lock_object;
	printf("lock_object:name:%s flags:0x%x",
			stringof(this->fr_lo->lo_name),
			this->fr_lo->lo_flags
			);


	printf("\n\t\t\t\t\t      ");
	this->fr_v_bufobj = this->fr_ap_vp->v_bufobj;
	printf("bufobj:ops:%p bsize:%d",\
			this->fr_v_bufobj.bo_ops,
			this->fr_v_bufobj.bo_bsize
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_bo_ops = this->fr_v_bufobj.bo_ops;
	printf("buf_ops:name:%s strategy:%p",\
			stringof(this->fr_bo_ops->bop_name),
			this->fr_bo_ops->bop_strategy
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_bo_object = this->fr_v_bufobj.bo_object;
	printf("vm_object:size:%d type:%d handle:%p",\
			this->fr_bo_object->size,
			this->fr_bo_object->type,
			this->fr_bo_object->handle
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_memq = this->fr_bo_object->memq;
	printf("vm_page:%p",\
			this->fr_memq.tqh_first
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_inode = (struct inode *)this->fr_ap_vp->v_data;
	printf("inode:%p:vnode:%p number:%d size:%d mode:0x%x diroff:%d, din1:%p",\
			this->fr_inode,
			this->fr_inode->i_vnode,
			this->fr_inode->i_number,
			this->fr_inode->i_size,
			this->fr_inode->i_mode,
			this->fr_inode->i_diroff,
			this->fr_inode->dinode_u.din2
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_din2 = this->fr_inode->dinode_u.din2;
	printf("ufs2_dinode:size:%d blksize:%d db:0:%d",\
			this->fr_din2->di_size,
			this->fr_din2->di_blksize,
			this->fr_din2->di_db[0]
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_i_ump = this->fr_inode->i_ump;
	printf("ufsmount:%p devvp(vtag:%s):%p bo:%p",\
			this->fr_i_ump,
			stringof(this->fr_i_ump->um_devvp->v_tag),
			this->fr_i_ump->um_devvp,
			this->fr_i_ump->um_bo
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_um_dev = this->fr_i_ump->um_dev;
	printf("cdev:name:%s",\
			this->fr_um_dev->si_name);

	printf("\n\t\t\t\t\t      ");
	this->fr_um_mountp = this->fr_inode->i_ump->um_mountp;
	printf("mount:%p:vfc:%p op:%p vnodecovered:%p nvnodelistsize:%d data:%p",\
			this->fr_um_mountp,
			this->fr_um_mountp->mnt_vfc,
			this->fr_um_mountp->mnt_op,
			this->fr_um_mountp->mnt_vnodecovered,
			this->fr_um_mountp->mnt_nvnodelistsize,
			this->fr_um_mountp->mnt_data
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_mnt_opt = this->fr_um_mountp->mnt_opt;
	volist0 = this->fr_mnt_opt->tqh_first;
	volist1 = volist0->link.tqe_next;
	volist2 = volist1->link.tqe_next;
	volist3 = volist2->link.tqe_next;
	volist4 = volist3->link.tqe_next;
	printf("vfsoptlist:[0]:(%s:%s) [1]:(%s:%s) [2]:(%s:%s) [3]:(%s:%s) [4]:%p:",\
			stringof(volist0->name), stringof(volist0->value),
			stringof(volist1->name), stringof(volist1->value),
			stringof(volist2->name), stringof(volist2->value),
			stringof(volist3->name), stringof(volist3->value),
			volist4
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_mnt_vnodecovered = this->fr_um_mountp->mnt_vnodecovered;
	printf("mnt_vnodecovered:tag:%s inumber:%d",
			stringof(this->fr_mnt_vnodecovered->v_tag),
			((struct inode *)(this->fr_mnt_vnodecovered->v_data))->i_number
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_mnt_nvnodelist = this->fr_um_mountp->mnt_nvnodelist;
	nvlist0 = this->fr_mnt_nvnodelist.tqh_first;
	nvlist1 = nvlist0->v_nmntvnodes.tqe_next;
	nvlist2 = nvlist1->v_nmntvnodes.tqe_next;
	nvlist3 = nvlist2->v_nmntvnodes.tqe_next;
	nvlist4 = nvlist3->v_nmntvnodes.tqe_next;
	nvlist5 = nvlist4->v_nmntvnodes.tqe_next;
	printf("vnodelst(%d):[0]:%p [1]:%p [2]:%p [3]:%p [4]:%p [5]:%p",\
			this->fr_um_mountp->mnt_nvnodelistsize,
			nvlist0,
			nvlist1,
			nvlist2,
			nvlist3,
			nvlist4,
			nvlist5
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_mnt_activevnodelist = this->fr_um_mountp->mnt_activevnodelist;
	nvactlist0 = this->fr_mnt_activevnodelist.tqh_first;
	nvactlist1 = nvactlist0->v_nmntvnodes.tqe_next;
	nvactlist2 = nvactlist1->v_nmntvnodes.tqe_next;
	nvactlist3 = nvactlist2->v_nmntvnodes.tqe_next;
	printf("actvnodelst(%d):[0]:%p [1]:%p [2]:%p [3]:%p",\
			this->fr_um_mountp->mnt_activevnodelistsize,
			nvactlist0,
			nvactlist1,
			nvactlist2,
			nvactlist3
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_mnt_vfc = this->fr_um_mountp->mnt_vfc;
	printf("vfsconf:name:%s vfsops:%p",\
			this->fr_mnt_vfc->vfc_name,
			this->fr_mnt_vfc->vfc_vfsops
			);

	printf("\n\t\t\t\t\t      ");
	this->fr_mnt_stat = this->fr_um_mountp->mnt_stat;
	printf("stat:fstypename:%s mntfromname:%s mntonname:%s bsize:%d iosize:%d files:%d",\
			this->fr_mnt_stat.f_fstypename,
			this->fr_mnt_stat.f_mntfromname,
			this->fr_mnt_stat.f_mntonname,
			this->fr_mnt_stat.f_bsize,
			this->fr_mnt_stat.f_iosize,
			this->fr_mnt_stat.f_files
			);

}

g_vfs_strategy:entry
/execname == proc/
{
	this->gvs_bo = args[0];
	this->gvs_bp = args[1];

	this->gvs_bo_object = this->gvs_bo->bo_object;

	printf("args:bo:0x%p bp:0x%p",\
			this->gvs_bo,
			this->gvs_bp
			);

	printf("\n\t\t\t\t\t      ");
	printf("vm_object:type:%d",\
			this->gvs_bo_object->type
			);
}

g_disk_start:entry
/execname == proc/
{
        this->as_bp = args[0];

        printf("bio:cmd:%d offset:%d bcount:%d pblkno:%d data:0x%p length:%d from:0x%p to:0x%p", \
                        this->as_bp->bio_cmd,
                        this->as_bp->bio_offset,
                        this->as_bp->bio_bcount,
                        this->as_bp->bio_pblkno,
                        this->as_bp->bio_data,
                        this->as_bp->bio_length,
                        this->as_bp->bio_from,
                        this->as_bp->bio_to
                        );
}

dastrategy:entry
/execname == proc/
{
	this->as_bp = args[0];

	this->as_bio_disk = this->as_bp->bio_disk;
	this->as_d_geom = this->as_bio_disk->d_geom;
	this->as_class = this->as_d_geom->class;
	this->as_bio_ma = this->as_bp->bio_ma;
	this->as_object = this->as_bio_ma[0]->object;

	printf("bio:cmd:%d offset:%d bcount:%d pblkno:%d data:0x%p flags:0x%x ma_n:%d resid:%d length:%d completed:%d ma_offset:0x%d", \
			this->as_bp->bio_cmd,
			this->as_bp->bio_offset,
			this->as_bp->bio_bcount,
			this->as_bp->bio_pblkno,
			this->as_bp->bio_data,
			this->as_bp->bio_flags,
			this->as_bp->bio_ma_n,
			this->as_bp->bio_resid,
			this->as_bp->bio_length,
			this->as_bp->bio_completed,
			this->as_bp->bio_ma_offset
			);

	printf("\n\t\t\t\t\t      ");
	printf("bio:data[0]:0x%p from:0x%p to:0x%p",
			this->as_bp->bio_data,
			this->as_bp->bio_from,
			this->as_bp->bio_to
			);

	printf("\n\t\t\t\t\t      ");

	printf("disk:name:%s unit:%d sectorsize:%d mediasize:%d ident:%s",\
			stringof(this->as_bio_disk->d_name),
			this->as_bio_disk->d_unit,
			this->as_bio_disk->d_sectorsize,
			this->as_bio_disk->d_mediasize,
			this->as_bio_disk->d_ident
                        );


	printf("\n\t\t\t\t\t      ");
	printf("geom:name:%s",\
			stringof(this->as_d_geom->name)
			);

	printf("\n\t\t\t\t\t      ");
	printf("class:name:%s",\
			stringof(this->as_class->name)
			);

	printf("\n\t\t\t\t\t      ");
	printf("vm_page:%p:phys_addr:0x%p order:%d object:%p pindex:%d", \
			this->as_bio_ma[0],
			this->as_bio_ma[0]->phys_addr,
			this->as_bio_ma[0]->order,
			this->as_bio_ma[0]->object,
			this->as_bio_ma[0]->pindex
			);

	printf("\n\t\t\t\t\t      ");
	printf("vm_object:type:%d size:%d",\
			this->as_object->type,
			this->as_object->size
			);

if(0) {
	func((uintptr_t)this->as_bio_disk->d_open);
	func((uintptr_t)this->as_bio_disk->d_close);
	func((uintptr_t)this->as_bio_disk->d_strategy);

	printf("\n\t\t\t\t\t      ");
	printf("data:0x%x", *(char *)this->as_bio_ma[0]->phys_addr);
}

}

mprsas_action:entry
/execname == proc/
{
	/*stack(50);*/
}

/*return*************************************************************************************/
mprsas_action:return
/execname == proc/
{}

dastrategy:return
/execname == proc/
{}

g_disk_start:return
/execname == proc/
{}

g_vfs_strategy:return
/execname == proc/
{}

ffs_read:return
/execname == proc/
{
	this->v = copyinstr((uintptr_t)self->fr_iov_base);
	printf("iovec:base:%p len:%d",
			self->fr_iov_base,
			self->fr_iov_len
			);
	trace(this->v);
}

ufs_lookup_ino:return
/execname == proc/
{}
/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
}
