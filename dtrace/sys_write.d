#!/usr/sbin/dtrace -s

/*
 * mount /dev/nvd1
 * test command:
 * 	/root/repo/io-tracing/dtrace/sys_write.d -c 'main -p 0xa5 -w /root/tmp/nvd1-mnt/data'
 *
 * optional commands:
 * // write API arguments: (fd, buf, nbytes)
 * dtrace -n 'pid$target::write:entry {printf("fd:%d buf:0x%p nbytes:%d", arg0, arg1, arg2);}' -c 'main -p 0xa5 -w /root/tmp/nvd1-mnt/data'
 *
 * // the stack of the main
 * dtrace -n 'nvme_ctrlr_submit_io_request:entry /execname == "main"/ {@[stack()]=count();}' -c 'main -p 0xa5 -w /root/tmp/nvd1-mnt/data'
 *
*/

#pragma D option flowindent

BEGIN
{
	printf("-----IO Tracing(sys_write) Start-----");

	proc = "main";
}

/*common*************************************************************************************/

/*entry**************************************************************************************/
sys_write:entry
/execname == proc/
{
	this->td = args[0];
	this->uap = args[1];

	printf("td--name:%s", this->td->td_name);

	printf("\n\t\t\t\t\t\t");
	printf("uap--fd:%d buf:0x%p nbyte:%d",
		this->uap->fd,
		this->uap->buf,
		this->uap->nbyte);
}

kern_writev:entry
/execname == proc/
{}

vn_write:entry
/execname == proc/
{
	this->fp = args[0];
	this->uio = args[1];
	this->active_cred = args[2];
	this->flags = args[3];
	this->td = args[4];

	this->vp = this->fp->f_vnode;

	printf("vp--type:0x%x", this->vp->v_type);

	printf("\n\t\t\t\t\t\t");
	printf("uio:0x%p", this->uio);
}

ffs_write:entry
/execname == proc/
{
	this->vp = args[0]->a_vp;
	this->uio = args[0]->a_uio;
	this->ioflag = args[0]->a_ioflag;
	this->ip = (struct inode *)this->vp->v_data;

	printf("uio:0x%p rw:%d offset:%d resid:%d",
			this->uio,
			this->uio->uio_rw,
			this->uio->uio_offset,
			this->uio->uio_resid);

	printf("\n\t\t\t\t\t\t");
	printf("ioflag:0x%x", this->ioflag);

	printf("\n\t\t\t\t\t\t");
	printf("ip:size:%d", this->ip->i_size);

}

bwrite:entry
/execname == proc/
{
}

/*return**************************************************************************************/

bwrite:return
/execname == proc/
{}

ffs_write:return
/execname == proc/
{
}

vn_write:return
/execname == proc/
{}

kern_writev:return
/execname == proc/
{}

sys_write:return
/execname == proc/
{}

END
{
	printf("-----IO Tracing(sys_write) END-----");
}
