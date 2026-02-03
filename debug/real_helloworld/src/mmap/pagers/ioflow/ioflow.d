#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./ioflow.d -c 'cat ../mount/ufsimg/ufstest'
 * 	./ioflow.d -c './main'
 */

#pragma D option flowindent

BEGIN
{
	rootvnodep = (struct vnode **)0xffffffff83df5740;

	/*procname = "cat";*/
	/*procname = "main";*/
	procname = "rm";
	ufsopen = 0;
	nameiflag = 0;
	ffsgetcg_flag = 0;

	/* @[stack()] = count() */
	printf("-----IO Tracing Start-----");
	printf("\n\t\t\t\t\t      ");
	printf("rootvnode:%p", *rootvnodep);
}

/*User Space*************************************************************************************/
pid$target:libc.so.7:open:entry
/execname == procname/
{
	this->td = curthread;

	printf("---User Space---:%s", probename);
	printf("\n\t\t\t\t\t      ");

	printf("td:%p", this->td);
	printf("\n\t\t\t\t\t      ");

	this->pwd = (struct pwd *)this->td->td_proc->p_fd->fd_pwd.__ptr;
        printf("pwd:pwd_rdir:%p", this->pwd->pwd_rdir);
}

/*return*************************************************************************************/
pid$target:libc.so.7:open:return
/execname == procname/
{
	printf("---User Space---:%s", probename);
}

/*Kernel Space*************************************************************************************/
/*common*************************************************************************************/

/*entry**************************************************************************************/
sys_openat:entry
/execname == procname/
{
	this->td = args[0];

	printf("td:td_proc->p_fd->fd_pwd.__ptr:%p",
		this->td->td_proc->p_fd->fd_pwd.__ptr
		);
	printf("\n\t\t\t\t\t      ");

	printf("td:%p curthread:%p", this->td, curthread);
	printf("\n\t\t\t\t\t      ");

	this->pwd = (struct pwd *)this->td->td_proc->p_fd->fd_pwd.__ptr;
	printf("pwd:pwd_rdir:%p", this->pwd->pwd_rdir);
}

isi_kern_unlinkat:entry
/execname == procname/
{}

ufs_open:entry
/execname == procname/
{
	this->ap = args[0];

	printf("ap:vp:%p",
		this->ap->a_vp
		);

	/*stack();*/

	ufsopen = 1;
}

namei:entry
/execname == procname/
{
	this->ndp = args[0];
        printf("ndp:vp:%p, dirp:%s(0x%p)",
                this->ndp->ni_vp,
                copyinstr((uintptr_t)this->ndp->ni_dirp),
                this->ndp->ni_dirp
                );
        printf("\n\t\t\t\t\t      ");

	nameiflag = 1;

	/*stack();*/
}

ufs_lookup:entry
/execname == procname && nameiflag/
{}

breadn_flags:entry
/execname == procname && (nameiflag || ffsgetcg_flag)/
{}

getblkx:entry
/execname == procname && (nameiflag || ffsgetcg_flag)/
{
	self->gbxvp = args[0];
	this->blkno = args[1];
	this->size = args[2];
	this->flags = args[5];
	self->bpp = args[6];

	printf("blkno:%d size:%d flags:0x%p",
		this->blkno,
		this->size,
		this->flags
		);
        printf("\n\t\t\t\t\t      ");

	printf("vp:%p type:%d tag:%s vmobj:0x%p",
		self->gbxvp,
		self->gbxvp->v_type,
		stringof(self->gbxvp->v_tag),
		self->gbxvp->v_bufobj.bo_object
		);
	func((uintptr_t)self->gbxvp->v_op);
        printf("\n\t\t\t\t\t      ");

	this->vmobj = self->gbxvp->v_bufobj.bo_object;
	printf("vmobj:size:%d",
		this->vmobj->size
		);
        printf("\n\t\t\t\t\t      ");

	this->v_rdev = self->gbxvp->v_rdev;
	if (this->v_rdev) {
		printf("rdev:name:%s",
			stringof(this->v_rdev->si_name)
			);
	}
        printf("\n\t\t\t\t\t      ");

printf("***************VFS*************************");
	printf("\n\t\t\t\t\t      ");

	this->consumer = (struct g_consumer *)self->gbxvp->v_bufobj.bo_private;
	this->geom = this->consumer->geom;
	this->provider = this->consumer->provider;
	printf("provider:%p geom:%p consumer:%p (lower)provider:%p",
		this->geom->provider.lh_first,
		this->geom,
		this->consumer,
		this->provider
		);
	printf("\n\t\t\t\t\t      ");

	printf("consumer:%p geom:%p",
		this->geom->consumer.lh_first,
		this->geom->consumer.lh_first->geom
		);
	printf("\n\t\t\t\t\t      ");

	printf("geom: name:%s cls_name:%s consumer:%p provider:%p",
		stringof(this->geom->name),
		stringof(this->geom->class->name),
		this->geom->consumer.lh_first,
		this->geom->provider.lh_first
		);
	printf("\n\t\t\t\t\t      ");

	printf("provider:%p", this->geom->provider.lh_first);
	printf("\n\t\t\t\t\t      ");

printf("***************PART*************************");
	printf("\n\t\t\t\t\t      ");

	this->provider = this->consumer->provider;
	this->geom = this->provider->geom;
	this->consumer = this->geom->consumer.lh_first;
	printf("provider:%p geom:%p consumer:%p (lower)provider:%p",
		this->provider,
		this->geom,
		this->consumer,
		this->geom->consumer.lh_first->provider /*dig*/
		);
	printf("\n\t\t\t\t\t      ");

	printf("consumer:%p geom:%p",
		this->consumer,
		this->geom->consumer.lh_first->geom
		);
	printf("\n\t\t\t\t\t      ");

	printf("geom: name:%s cls_name:%s consumer:%p provider:%p",
		stringof(this->geom->name),
		stringof(this->geom->class->name),
		this->geom->consumer.lh_first,
		this->geom->provider.lh_first
		);
	printf("\n\t\t\t\t\t      ");

	printf("provider: name:%s cls_name:%s geom:%p",
		stringof(this->provider->name),
		stringof(this->geom->class->name),
		this->provider->geom
		);
	printf("\n\t\t\t\t\t      ");
	printf("privider consumers:%p",
		this->provider->consumers.lh_first
		);
	printf("\n\t\t\t\t\t      ");

printf("***************MD*************************");
	printf("\n\t\t\t\t\t      ");

	this->provider = this->consumer->provider;
	this->geom = this->provider->geom;
	this->consumer = this->geom->consumer.lh_first;
	printf("provider:%p geom:%p consumer:%p",
		this->provider,
		this->geom,
		this->consumer
		);
	printf("\n\t\t\t\t\t      ");

	printf("provider:%p name:%s cls_name:%s consumer:%p provider:%p ",
		this->provider,
		stringof(this->provider->name),
		stringof(this->geom->class->name),
		this->geom->consumer.lh_first,
		this->geom->provider.lh_first
		);
	printf("\n\t\t\t\t\t      ");

	printf("geom:%p name:%s cls_name:%s consumer:%p provider:%p",
		this->geom,
		stringof(this->geom->name),
		stringof(this->geom->class->name),
		this->geom->consumer.lh_first,
		this->geom->provider.lh_first
		);
	printf("\n\t\t\t\t\t      ");

	printf("bp:*bpp:0x%p",
		*self->bpp
		);
}

getnewbuf:entry
/execname == procname && nameiflag/
{}

bstrategy:entry
/execname == procname && nameiflag/
{}

ufs_strategy:entry
/execname == procname && nameiflag/
{
	this->ufs_s_ap = args[0];

	self->bp = this->ufs_s_ap->a_bp;

	printf("bp:iooffset:0x%x offset:0x%x",
		self->bp->b_iooffset,
		self->bp->b_offset
		);
}

ufs_bmaparray:entry
/execname == procname && nameiflag/
{
	this->vp = args[0];
	self->blknop = args[2];
	this->bp = args[3];

/*****************************vnode*************************************/
	printf("vp(0x%p):tag:%s type:%d",
		this->vp,
		stringof(this->vp->v_tag),
		this->vp->v_type
		);
	printf("\n\t\t\t\t\t      ");

/*****************************inode*************************************/
	this->inode = (struct inode*)this->vp->v_data;
	printf("inode:number:%d",
		this->inode->i_number
		);

	printf("\n\t\t\t\t\t      ");

/*****************************buf*************************************/
	printf("bp(0x%p): blkno:0x%x lblkno:0x%x",
		this->bp,
		this->bp->b_blkno,
		this->bp->b_lblkno
		);
}

ffs_geom_strategy:entry
/execname == procname && nameiflag/
{
	self->bp = args[1];

	printf("bp:iooffset:0x%x offset:0x%x",
		self->bp->b_iooffset,
		self->bp->b_offset
	);
}

namei_handle_root:entry
/execname == procname/
{
	this->ndp = args[0];

	this->cnp = &this->ndp->ni_cnd;

	printf("cnp:nameptr:%s", stringof(this->cnp->cn_nameptr));
	printf("\n\t\t\t\t\t      ");

	this->rootdir = this->ndp->ni_rootdir;
	printf("ndp:rootdir:%p", this->rootdir);
	printf("\n\t\t\t\t\t      ");

/*****************************vnode*************************************/
	printf("rootdir:tag:%s type:%d",
		stringof(this->rootdir->v_tag),
		this->rootdir->v_type
		);
	printf("\n\t\t\t\t\t      ");

/*****************************inode*************************************/
	this->inode = (struct inode*)this->rootdir->v_data;
	printf("inode:number:%d",
		this->inode->i_number
		);

	printf("\n\t\t\t\t\t      ");

/*****************************mount*************************************/

	printf("td:td_proc->p_fd->fd_pwd.__ptr:%p",
		curthread->td_proc->p_fd->fd_pwd.__ptr
		);
	printf("\n\t\t\t\t\t      ");

	this->pwd = (struct pwd *)curthread->td_proc->p_fd->fd_pwd.__ptr;
	printf("pwd:pwd_rdir:%p", this->pwd->pwd_rdir);
}

/*remove a file*/
isi_kern_unlinkat:entry
/execname == procname/
{}

ffs_truncate:entry,
vtruncbuf:entry,
ufs_remove:entry,
ufs_inactive:entry
/execname == procname/
{}

bufobj_wwait:entry
/execname == procname/
{
	this->bo = args[0];

	printf("bo:0x%p numoutput:%d",
		this->bo,
		this->bo->bo_numoutput
		);
}

ufs_create:entry
/execname == procname/
{}

ufs_makeinode:entry
/execname == procname/
{}

ffs_valloc:entry
/execname == procname/
{}

ffs_getcg:entry
/execname == procname/
{
	printf("-----%s-----", probename);
	ffsgetcg_flag = 1;
}

ffs_vgetf:entry
/execname == procname/
{
	this->mp = args[0];
	this->ump = (struct ufsmount *)this->mp->mnt_data;

	this->devvp = this->ump->um_devvp;
	printf("devvp:0x%p",
		this->devvp
		);
}

ffs_load_inode:entry
/execname == procname/
{
	this->bp = args[0];
	this->ip = args[1];
	this->ino = args[3];

	printf("ino:%d",
		this->ino
		);
	printf("\n\t\t\t\t\t      ");

	printf("bp:blkno:%d iooffset:0x%p offset:0x%p bufsize:%d vp:0x%p qindex:%d",
		this->bp->b_blkno,
		this->bp->b_iooffset,
		this->bp->b_offset,
		this->bp->b_bufsize,
		this->bp->b_vp,
		this->bp->b_qindex
		);
	printf("\n\t\t\t\t\t      ");
	printf("bp:data:0x%p bcount:%d kvabase:0x%p kvasize:%d",
		this->bp->b_data,
		this->bp->b_bcount,
		this->bp->b_kvabase,
		this->bp->b_kvasize
		);
	printf("\n\t\t\t\t\t      ");

	this->bufobj =  this->bp->b_bufobj;
	printf("bufobf:ops:%p, bsize:%d",
		this->bufobj->bo_ops,
		this->bufobj->bo_bsize
		);
	printf("\n\t\t\t\t\t      ");
	func((uintptr_t)this->bufobj->bo_ops);
	printf("\n\t\t\t\t\t      ");

	this->bufv_clean = this->bufobj->bo_clean;
	printf("bufv_clean:cnt:%d",
		this->bufv_clean.bv_cnt
		);
	printf("\n\t\t\t\t\t      ");
	this->clean_bv_hd = this->bufv_clean.bv_hd;
	printf("\n\t\t\t\t\t      ");

	this->bufv_dirty = this->bufobj->bo_dirty;
	printf("bufv_dirty:cnt:%d",
		this->bufv_dirty.bv_cnt
		);
	printf("\n\t\t\t\t\t      ");

	this->din2 = ((struct ufs2_dinode *)this->bp->b_data + this->ino);
	printf("din2:size:%d",
		this->din2->di_size
		);

}

ffs_update:entry
/execname == procname/
{
	this->vp = args[0];
	this->waitfor = args[1];

	printf("vp:0x%p waitfor:%d",
		this->vp,
		this->waitfor
		);
}

bwrite:entry
/execname == procname/
{}

g_vfs_strategy:entry
/execname == procname/
{
	this->bo = args[0];
	this->bp = args[1];

	printf("bo:0x%p bp:0x%p",
		this->bo,
		this->bp
		);
	printf("\n\t\t\t\t\t      ");

	printf("bp:bufobj:0x%p iocmd:%d iooffset:0x%p npages:%d",
		this->bp->b_bufobj,
		this->bp->b_iocmd,
		this->bp->b_iooffset,
		this->bp->b_npages
		);
	printf("\n\t\t\t\t\t      ");

	this->bp->b_pages;
	printf("pages[0]:0x%p object:0x%p pindex:0x%x phys_addr:0x%p",
		this->bp->b_pages[0],
		this->bp->b_pages[0]->object,
		this->bp->b_pages[0]->pindex,
		this->bp->b_pages[0]->phys_addr
		);
	printf("\n\t\t\t\t\t      ");

	if (this->bp->b_pages[1]) {
		printf("pages[1]:0x%p object:0x%p pindex:0x%x phys_addr:0x%p",
			this->bp->b_pages[1],
			this->bp->b_pages[1]->object,
			this->bp->b_pages[1]->pindex,
			this->bp->b_pages[1]->phys_addr
		);
	}

	this->vmobj = this->bp->b_pages[0]->object;
	printf("vmobj:size:0x%p ",
		this->vmobj->size
		);
	printf("\n\t\t\t\t\t      ");

	/**************************memq****************************/
	this->memq = this->vmobj->memq;
	this->pg0 = this->memq.tqh_first;
	printf("pg0:0x%p object:0x%p pindex:0x%x",
		this->pg0,
		this->pg0->object,
		this->pg0->pindex
		);
}

vnode_pager_setsize:entry
/execname == procname/
{}

vm_object_page_remove:entry
/execname == procname/
{}

vm_page_tryxbusy:entry
/execname == procname/
{}

vm_page_xunbusy_hard_tail:entry
/execname == procname/
{}

vinactive:entry
/execname == procname/
{
	this->vp = args[0];

	this->inode = (struct inode *)this->vp->v_data;
	printf("inode: number:%d",
		this->inode->i_number
		);
	printf("\n\t\t\t\t\t      ");

	this->vmobj = this->vp->v_bufobj.bo_object;
	printf("vmobj:size:%d",
		this->vmobj->size
		);
	printf("\n\t\t\t\t\t      ");

	this->pg0 = this->vmobj->memq.tqh_first;
	printf("pg0: pindex:%d busy_lock:0x%p",
		this->pg0->pindex,
		this->pg0->busy_lock
		);
	printf("\n\t\t\t\t\t      ");
}

/*return*************************************************************************************/
vinactive:return
/execname == procname/
{}

vm_page_xunbusy_hard_tail:return
/execname == procname/
{}

vm_page_tryxbusy:return
/execname == procname/
{
	this->ret = args[1];

	printf("ret:%d", this->ret);
}

vm_object_page_remove:return
/execname == procname/
{}

vnode_pager_setsize:return
/execname == procname/
{}

g_vfs_strategy:return
/execname == procname/
{}

ffs_getcg:return
/execname == procname/
{
	printf("-----%s-----", probename);
	ffsgetcg_flag = 0;
}

bwrite:return,
ffs_update:return,
ffs_load_inode:return,
ffs_vgetf:return,
ffs_valloc:return,
ufs_makeinode:return,
ufs_create:return
/execname == procname/
{}

bufobj_wwait:return
/execname == procname/
{}

ffs_truncate:return,
vtruncbuf:return,
ufs_remove:return,
ufs_inactive:return
/execname == procname/
{}

isi_kern_unlinkat:return
/execname == procname/
{}

namei_handle_root:return
/execname == procname/
{}

ffs_geom_strategy:return
/execname == procname && nameiflag/
{
	printf("bp:iooffset:0x%x offset:0x%x",
		self->bp->b_iooffset,
		self->bp->b_offset
	);
}

ufs_bmaparray:return
/execname == procname && nameiflag/
{
        printf("%s---------", probename);
        printf("\n\t\t\t\t\t      ");

	printf("blkno:0x%x",
		*self->blknop
		);
}

ufs_strategy:return
/execname == procname && nameiflag/
{
        printf("%s---------", probename);
        printf("\n\t\t\t\t\t      ");

	printf("bp:iooffset:0x%x offset:0x%x",
		self->bp->b_iooffset,
		self->bp->b_offset
		);
}

bstrategy:return
/execname == procname && nameiflag/
{}

getnewbuf:return
/execname == procname && nameiflag/
{}

getblkx:return
/execname == procname && (nameiflag || ffsgetcg_flag)/
{

	this->bp = *self->bpp;

        printf("%s---------", probename);
        printf("\n\t\t\t\t\t      ");

	printf("bp:0x%p flags:0x%x",
		this->bp,
		this->bp->b_flags
		);
        printf("\n\t\t\t\t\t      ");

	this->gtxbufobj = self->gbxvp->v_bufobj;
	this->gtxvmobj = this->gtxbufobj.bo_object;
	printf("vmobj:size:%d",
		this->gtxvmobj->size
		);
}

breadn_flags:return
/execname == procname && (nameiflag || ffsgetcg_flag)/
{
}

ufs_lookup:return
/execname == procname && nameiflag/
{}

namei:return
/execname == procname/
{
        printf("%s---------", probename);
        printf("\n\t\t\t\t\t      ");

        printf("ndp:vp:%p, dirp:%s 0x%p",
                this->ndp->ni_vp,
                copyinstr((uintptr_t)this->ndp->ni_dirp),
                this->ndp->ni_dirp
                );
        printf("\n\t\t\t\t\t      ");

        this->ret_ni_vp = this->ndp->ni_vp;

	printf("ni_vp:%p", this->ret_ni_vp);
if (this->ret_ni_vp) {

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

/*****************************mount*************************************/
        this->mount = this->ret_ni_vp->v_mount;
        printf("mount:0x%p vnodecovered:0x%p",
                this->mount,
                this->mount->mnt_vnodecovered
        );
        printf("\n\t\t\t\t\t      ");

        this->mnt_data = (struct ufsmount*)this->mount->mnt_data;
        this->ufsmnt = this->mnt_data;
        printf("ufsmnt:mountp:0x%p",
                this->ufsmnt->um_mountp
        );
        printf("\n\t\t\t\t\t      ");

        this->um_dev = this->ufsmnt->um_dev;
        printf("cdev:si_name:%s",
                this->um_dev->si_name
                );
        printf("\n\t\t\t\t\t      ");

        this->um_devvp = this->ufsmnt->um_devvp;
        printf("um_devvp:tag:%s type:%d",
                stringof(this->um_devvp->v_tag),
                this->um_devvp->v_type
                );
        printf("\n\t\t\t\t\t      ");

/*****************************vnodecovered*************************************/
        this->vnodecovered = this->mount->mnt_vnodecovered;
        printf("vpcovered:tag:%s type:%d",
                stringof(this->vnodecovered->v_tag),
                this->vnodecovered->v_type
                );
        printf("\n\t\t\t\t\t      ");

} /* this->ret_ni_bp */

	nameiflag = 0;
}

ufs_open:return
/execname == procname/
{
	trace(probename);

	ufsopen = 0;
}

isi_kern_unlinkat:return
/execname == procname/
{}

sys_openat:return
/execname == procname/
{}

/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
	printf("\n\t\t\t\t\t      ");
}
