#!/usr/sbin/dtrace -s

/*
 * 	./rhw.d -c './main'
 */

#pragma D option flowindent

BEGIN
{
        proc = "main"
}

/*common*************************************************************************************/
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

VOP_CACHEDLOOKUP_APV:*
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

       /* VFS */
       printf("geom:%s \tclass:%s",
               stringof(this->geom->name),
               stringof(this->geom->class->name)
               );
       printf("\n\t\t\t\t\t      ");

/************************************************************************/
       /* MIRROR */
       this->mirror_pvd = this->umcp->provider;
       printf("geom:%s \t\t\tclass:%s \tprovider:%s \tflags:0x%x",
               stringof(this->mirror_pvd->geom->name),
               stringof(this->mirror_pvd->geom->class->name),
               stringof(this->mirror_pvd->name),
               this->mirror_pvd->flags
               );
       printf("\n\t\t\t\t\t      ");

       this->mirror_csm = this->mirror_pvd->geom->consumer.lh_first;
       printf("geom:%s \t\t\tclass:%s \tflags:0x%x",
               stringof(this->mirror_csm->geom->name),
               stringof(this->mirror_csm->geom->class->name),
               this->mirror_csm->flags
               );
       printf("\n\t\t\t\t\t      ");

       this->mirror_csm2 = this->mirror_csm->consumer.le_next;

/************************************************************************/
       /* PART */
       /* md10p1 */
       this->part_pvd = this->mirror_csm->provider;
       printf("geom:%s \t\tclass:%s \tprovider:%s \tflags:0x%x",
               stringof(this->part_pvd->geom->name),
               stringof(this->part_pvd->geom->class->name),
               stringof(this->part_pvd->name),
               this->part_pvd->flags
               );
       printf("\n\t\t\t\t\t      ");


       /* md9p1 */
       this->part_pvd2 = this->mirror_csm2->provider;
if (this->part_pvd2) {
       printf("geom:%s \t\t\tclass:%s \tprovider:%s \tflags:0x%x",
               stringof(this->part_pvd2->geom->name),
               stringof(this->part_pvd2->geom->class->name),
               stringof(this->part_pvd2->name),
               this->part_pvd2->flags
		);
       printf("\n\t\t\t\t\t      ");

}
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
}


/*return*************************************************************************************/

breadn_flags:return
/execname == proc/
{}

ffs_blkatoff:return
/execname == proc/
{
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
}

ufs_lookup_ino:return
/execname == proc/
{
	printf("dd_ino:%p", this->dd_ino);
	printf("\n\t\t\t\t\t      ");
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
