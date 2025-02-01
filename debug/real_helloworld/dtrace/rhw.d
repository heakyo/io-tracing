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

	printf("dvp:v_tag:%s v_type:%d",
		stringof(this->dvp->v_tag),
		this->dvp->v_type);
}

ufs_lookup_ino:entry
/execname == proc/
{

}

ffs_blkatoff:entry
/execname == proc/
{
	this->bpp = args[3];

	printf("bpp:0x%p(v:%p)", this->bpp, *this->bpp);
}

/*return*************************************************************************************/

ffs_blkatoff:return
/execname == proc/
{
	printf("ret:%d", args[1]);
	printf("\n\t\t\t\t\t      ");
	printf("bpp:0x%p(v:0x%p)", this->bpp, *this->bpp);
	printf("\n\t\t\t\t\t      ");

	this->bp = *this->bpp;
	this->ep = (struct direct *)((char *)this->bp->b_data + 24);
	printf("ep:d_ino:%d d_reclen:%d d_namelen:%d, d_name:%s",
		this->ep->d_ino,
		this->ep->d_reclen,
		this->ep->d_namlen,
		this->ep->d_name);
}

ufs_lookup_ino:return
/execname == proc/
{

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
