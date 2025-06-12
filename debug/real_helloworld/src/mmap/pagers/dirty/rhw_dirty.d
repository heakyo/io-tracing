#!/usr/sbin/dtrace -s

/*
 *      ./rhw_template.d -c './main'
 */

#pragma D option flowindent

BEGIN
{
        proc = "main";
	sysmsync = 0;
	vpsv = 0;
}

sys_msync:entry
/execname == "main"/
{
	trace(probename);

	sysmsync = 1;
}

vm_page_set_validclean:entry
/sysmsync && execname == "main" && args[0]->dirty == 255/
{
	self->m = args[0];
	self->base = args[1];
	self->size = args[2];

	printf("dirty:%d base:%d size:%d",
		self->m->dirty,
		self->base,
		self->size
		);

	stack();

	vpsv = 1;
}

/*-----------------------------------------------------------------------------*/
vm_page_set_validclean:return
/sysmsync && execname == "main" && vpsv/
{
	printf("dirty:%d",
		self->m->dirty
		);

	vpsv = 0;
}

sys_msync:return
/execname == "main"/
{
	trace(probename);

	sysmsync = 0;
}
