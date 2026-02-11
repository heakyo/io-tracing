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

        procname = "main";
	kwflag = 0;

        /* @[stack()] = count() */
        printf("-----IO Tracing Start-----");
        printf("\n\t\t\t\t\t      ");
        printf("rootvnode:%p", *rootvnodep);
}

/*Kernel Space*******************************************************************************/
kern_writev:entry
/execname == procname/
{
	trace(probename);
	kwflag = 1;
}

vm_page_grab_pages_unlocked_tracked:entry
/kwflag/
{}

vm_page_insert_radixdone:entry
/kwflag/
{
	this->m = args[0];

	printf("pg:busy_lock:%d",
		this->m->busy_lock
		);

	/*stack();*/
}

vm_page_sunbusy:entry
/kwflag/
{
	self->m = args[0];

	printf("pg:busy_lock:%d",
		self->m->busy_lock
		);

	/*stack();*/
}

vm_page_trysbusy:entry
/kwflag/
{
	self->m = args[0];

	printf("pg:busy_lock:%d",
		self->m->busy_lock
		);
}

/*return*************************************************************************************/
vm_page_trysbusy:return
/kwflag/
{
	printf("pg:busy_lock:%d",
		self->m->busy_lock
		);
}

vm_page_sunbusy:return
/kwflag/
{
	printf("pg:busy_lock:%d",
		self->m->busy_lock
		);
	/*stack();*/
}

vm_page_insert_radixdone:return
/kwflag/
{}

vm_page_grab_pages_unlocked_tracked:return
/kwflag/
{}

kern_writev:return
/execname == procname/
{
	trace(probename);
	kwflag = 0;
}

/********************************************************************************************/
END
{
        printf("-----IO Tracing END-----");
        printf("\n\t\t\t\t\t      ");
}

