#!/usr/sbin/dtrace -s

/*
 * test command:
 *      ./ioflow.d -c 'cat ../mount/ufsimg/ufstest'
 *      ./ioflow.d -c './main'
 */

#pragma D option flowindent

BEGIN
{
        rootvnodep = (struct vnode **)0xffffffff83df5740;
	unmapped_buf = *(caddr_t *)0xffffffff83df55c0;

        procname = "main";
	kprdflag = 0;
	gcflag = 0;

        /* @[stack()] = count() */
        printf("-----IO Tracing Start-----");
        printf("\n\t\t\t\t\t      ");
        printf("rootvnode:%p", *rootvnodep);
}

/*Page Fault*/
vnode_pager_getpages:entry
/execname == procname/
{}
/*return*************************************************************************************/
vnode_pager_getpages:return
/execname == procname/
{}

/*Kernel Space*******************************************************************************/
kern_pread:entry
/execname == procname/
{
	trace(probename);
	kprdflag = 1;
}

getblk_core:entry
/kprdflag/
{
        self->vp = args[0];
        self->bpp = args[8];

	/*****vnode*****/
        printf("vp:%p tag:%p %s",
                self->vp,
                self->vp->v_tag,
                stringof(self->vp->v_tag)
                );
        printf("\n\t\t\t\t\t      ");

	/*****inode*****/
	this->inode = (struct inode *)self->vp->v_data;
	printf("inode: number:%d",
		this->inode->i_number
		);
        printf("\n\t\t\t\t\t      ");

	/*****bufobj*****/
	this->bufobj = self->vp->v_bufobj;
	printf("bufobj:vmobj:%p",
		this->bufobj.bo_object
		);
        printf("\n\t\t\t\t\t      ");

	this->bufv_clean = this->bufobj.bo_clean;
	this->bufv_dirty = this->bufobj.bo_dirty;
	printf("bufv clean:cnt:%d",
		this->bufv_clean.bv_cnt
		);
        printf("\n\t\t\t\t\t      ");
	printf("bufv dirty:cnt:%d",
		this->bufv_dirty.bv_cnt
		);
        printf("\n\t\t\t\t\t      ");

	/*stack();*/

	gcflag = 1;
}

getnewbuf:entry
/kprdflag/
{}

/*set vp*/
bgetvp:entry
/kprdflag/
{

	this->bp = args[1];
	/*****buf*****/
        printf("bp:%p bufobj:%p vp:%p",
		this->bp,
		this->bp->b_bufobj,
		this->bp->b_vp
                );
}

allocbuf_flags:entry
/kprdflag/
{
	this->bp = args[0];
	this->size = args[1];

	printf("size:%d", this->size);
        printf("\n\t\t\t\t\t      ");

	/*****buf*****/
        printf("bp:%p flags:%p bcount:%d",
		this->bp,
		this->bp->b_flags,
		this->bp->b_bcount
                );
        printf("\n\t\t\t\t\t      ");

if (0) {
	printf("buf pages: pg0:%p pg1:%p pg2:%p",
		this->bp->b_pages[0],
		this->bp->b_pages[1],
		this->bp->b_pages[2]
		);
}

}

vfs_vmio_extend:entry
/kprdflag/
{
	this->bp = args[0];
	this->desiredpages = args[1];
	this->size = args[2];

	printf("size:%d desiredpages:%d",
		this->size, this->desiredpages);
        printf("\n\t\t\t\t\t      ");

	/*****buf*****/
        printf("bp:%p flags:%p bcount:%d b_npages:%d",
		this->bp,
		this->bp->b_flags,
		this->bp->b_bcount,
		this->bp->b_npages
                );
        printf("\n\t\t\t\t\t      ");

	printf("buf pages: pg0:%p pg1:%p pg2:%p",
		this->bp->b_pages[0],
		this->bp->b_pages[1],
		this->bp->b_pages[2]
		);
}

vm_page_grab_pages_unlocked_tracked:entry
/kprdflag/
{
	this->allocflags = args[3];

	printf("allocflags:%p", this->allocflags);
}

vm_page_acquire_unlocked:entry
/kprdflag/
{
	this->mp = args[3];

	printf("mp:%p", *this->mp);
}

vm_page_grab_pages_tracked:entry
/kprdflag/
{}

vm_page_alloc_after:entry
/kprdflag && gcflag/
{
	this->object = args[0];
	this->pindex = args[1];

	printf("object:%p pindex:%d",
		this->object, this->pindex);
}

uma_zalloc:entry
/kprdflag && gcflag/
{}

vm_page_insert_after:entry
/kprdflag && gcflag/
{
	this->mpred = args[3];
	printf("mpred:%p", this->mpred);
}

/*return*************************************************************************************/
vm_page_insert_after:return
/kprdflag && gcflag/
{}

uma_zalloc:return
/kprdflag && gcflag/
{}

vm_page_alloc_after:return
/kprdflag && gcflag/
{}

vm_page_grab_pages_tracked:return
/kprdflag/
{
	this->ret = args[1];
	printf("ret:%d", this->ret);
}

vm_page_acquire_unlocked:return
/kprdflag/
{
	this->ret = args[1];

	printf("mp:%p", *this->mp);
        printf("\n\t\t\t\t\t      ");

	printf("ret:%d", this->ret);
}

vm_page_grab_pages_unlocked_tracked:return
/kprdflag/
{
}

vfs_vmio_extend:return
/kprdflag/
{
	/*****buf*****/
        printf("bp:%p flags:%p bcount:%d b_npages:%d",
		this->bp,
		this->bp->b_flags,
		this->bp->b_bcount,
		this->bp->b_npages
                );
        printf("\n\t\t\t\t\t      ");

	printf("buf pages: pg0:%p pg1:%p pg2:%p",
		this->bp->b_pages[0],
		this->bp->b_pages[1],
		this->bp->b_pages[2]
		);
}

allocbuf_flags:return
/kprdflag/
{
	printf("buf pages: pg0:%p pg1:%p pg2:%p",
		this->bp->b_pages[0],
		this->bp->b_pages[1],
		this->bp->b_pages[2]
		);
}

bgetvp:return
/kprdflag/
{
	/*****buf*****/
        printf("bp:%p bufobj:%p vp:%p",
		this->bp,
		this->bp->b_bufobj,
		this->bp->b_vp
                );
        printf("\n\t\t\t\t\t      ");

	this->bufv_clean = this->bp->b_bufobj->bo_clean;
	this->bufv_dirty = this->bp->b_bufobj->bo_dirty;
	printf("bufv clean:cnt:%d", this->bufv_clean.bv_cnt);
        printf("\n\t\t\t\t\t      ");
	printf("bufv dirty:cnt:%d", this->bufv_dirty.bv_cnt);
        printf("\n\t\t\t\t\t      ");
}

getnewbuf:return
/kprdflag/
{
	this->bp = args[1];
	/*****buf*****/
        printf("bp:%p bcount:%d bufsize:%d flags:%x data:%p",
                this->bp,
		this->bp->b_bcount,
		this->bp->b_bufsize,
                this->bp->b_flags,
                this->bp->b_data
                );
        printf("\n\t\t\t\t\t      ");
        printf("bp: bufobj:%p vp:%p kvabase:%p",
		this->bp->b_bufobj,
		this->bp->b_vp,
		this->bp->b_kvabase
                );
}

getblk_core:return
/kprdflag/
{
        this->bp = *self->bpp;

	/*****buf*****/
        printf("bp:%p offset:%d bcount:%d bufsize:%d flags:%x data:%p",
                this->bp,
		this->bp->b_offset,
		this->bp->b_bcount,
		this->bp->b_bufsize,
                this->bp->b_flags,
                this->bp->b_data
                );
        printf("\n\t\t\t\t\t      ");
        printf("bp: bufobj:%p vp:%p kvabase:%p",
		this->bp->b_bufobj,
		this->bp->b_vp,
		this->bp->b_kvabase
                );
        printf("\n\t\t\t\t\t      ");

	printf("buf pages: pg0:%p pg1:%p pg2:%p",
		this->bp->b_pages[0],
		this->bp->b_pages[1],
		this->bp->b_pages[2]
		);
        printf("\n\t\t\t\t\t      ");

	/*****bufobj*****/
	this->bufobj = self->vp->v_bufobj;
	printf("bufobj:bsize:%d",
		this->bufobj.bo_bsize
		);
        printf("\n\t\t\t\t\t      ");

	this->bufv_clean = this->bufobj.bo_clean;
	this->bufv_dirty = this->bufobj.bo_dirty;
	printf("bufv clean:cnt:%d",
		this->bufv_clean.bv_cnt);
        printf("\n\t\t\t\t\t      ");
	printf("bufv dirty:cnt:%d",
		this->bufv_dirty.bv_cnt);
        printf("\n\t\t\t\t\t      ");

        printf("unmapped_buf:%p",
                unmapped_buf
                );

	gcflag = 0;
}

kern_pread:return
/execname == procname/
{
	trace(probename);
	kprdflag = 0;
}

/********************************************************************************************/
END
{
        printf("-----IO Tracing END-----");
        printf("\n\t\t\t\t\t      ");
}

