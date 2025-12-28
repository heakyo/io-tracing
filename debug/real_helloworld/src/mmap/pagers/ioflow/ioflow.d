#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./ioflow.d -c 'cat ../mount/ufsimg/ufstest'
 */

#pragma D option flowindent

BEGIN
{
	rootvnodep = (struct vnode **)0xffffffff83df5740;

	procname = "cat";
	ufsopen = 0;

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

	/*stack();*/
}

ufs_strategy:entry
/execname == procname/
{}

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

/*return*************************************************************************************/

namei_handle_root:return
/execname == procname/
{}

ufs_strategy:entry
/execname == procname/
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

/*****************************vnode*************************************/
        this->ret_ni_vp = this->ndp->ni_vp;
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
}

ufs_open:return
/execname == procname/
{
	trace(probename);

	ufsopen = 0;
}

sys_openat:return
/execname == procname/
{}

/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
	printf("\n\t\t\t\t\t      ");
}
