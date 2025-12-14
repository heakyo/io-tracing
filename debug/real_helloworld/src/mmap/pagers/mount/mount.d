#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./mount.d -c 'mount /dev/md9 ufsimg'
 */

#pragma D option flowindent

BEGIN
{
	procname = "mount";
	ffsmnt = 0;

	/* @[stack()] = count() */
	printf("-----IO Tracing Start-----");
}

/*User Space*************************************************************************************/
pid$target:mount:mount_fs:entry
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
pid$target:mount:mount_fs:return
/execname == procname/
{
	printf("---User Space---:%s", probename);
}

/*Kernel Space*************************************************************************************/
/*common*************************************************************************************/
fdinit:*
/execname == procname && ffsmnt/
{}

/*entry**************************************************************************************/
sys_nmount:entry
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

ffs_mount:entry
/execname == procname/
{
	trace(probename);

	/*stack();*/

	ffsmnt = 1;
}

namei:entry
/execname == procname && ffsmnt/
{}

namei_handle_root:entry
/execname == procname && ffsmnt/
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

	stack();
}

mountcheckdirs:entry
/execname == procname/
{
}

/*return*************************************************************************************/
mountcheckdirs:return
/execname == procname/
{}

namei_handle_root:return
/execname == procname && ffsmnt/
{}

namei:return
/execname == procname && ffsmnt/
{}

ffs_mount:return
/execname == procname/
{
	trace(probename);

	ffsmnt = 0;
}

sys_nmount:return
/execname == procname/
{}

/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
	printf("\n\t\t\t\t\t      ");
}
