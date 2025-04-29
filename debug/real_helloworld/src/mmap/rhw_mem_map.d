#!/usr/sbin/dtrace -s

/*
 *      ./rhw_cam.d -c './main'
 */

/* probemod:probefunc:probename */

#pragma D option flowindent

BEGIN
{
        proc = "main";
	sysmunmap = 0;
}

/*sys*************************************************************************************/
kernel:sys_munmap:entry
/execname == proc/
{
	trace(probename);
	printf("\n\t\t\t\t\t      ");

	sysmunmap = 1;
}

vm_map_entry_delete:entry
/execname == proc && sysmunmap/
{
	self->map = args[0];
	self->entry = args[1];

	self->object = self->entry->object.vm_object;

	printf("obj:%p size:%d flags:0x%x type:%d ref_count:%d",
		self->object,
		self->object->size,
		self->object->flags,
		self->object->type,
		self->object->ref_count
		);
}

vm_object_collapse:entry
/execname == proc && sysmunmap/
{
	self->object = args[0];

	printf("obj:%p backing_object:%p",
		self->object,
		self->object->backing_object
		);
	printf("\n\t\t\t\t\t      ");
}

vm_object_backing_collapse_wait:entry
/execname == proc && sysmunmap/
{
	self->object = args[0];

	self->backing_object = self->object->backing_object;

	printf("backing_object:%p",
		self->backing_object
		);
	if (self->backing_object) {
		printf("backing_object:%p size:%d flags:0x%x type:%d ref_count:%d",
			self->backing_object,
			self->backing_object->size,
			self->backing_object->flags,
			self->backing_object->type,
			self->backing_object->ref_count
			);
		printf("\n\t\t\t\t\t      ");

		self->p=self->backing_object->memq.tqh_first;
		printf("vm_page:obj:%p ref_count:0x%x",
			self->p->object,
			self->p->ref_count
			);
		printf("\n\t\t\t\t\t      ");

		self->pv_ent = self->p->md.pv_list.tqh_first;
		printf("pv_ent0:%p pv_va:%p",
			self->pv_ent,
			self->pv_ent->pv_va
			);
	}

}

vm_object_collapse_scan:entry
/execname == proc && sysmunmap/
{
}

/*****************************************************************************************/
vm_object_collapse_scan:return
/execname == proc && sysmunmap/
{
}

vm_object_backing_collapse_wait:return
/execname == proc && sysmunmap/
{
	self->ret_backing_object = args[1];

	printf("ret:backing_object:%p",
		self->ret_backing_object
		);
}

vm_object_collapse:return
/execname == proc && sysmunmap/
{

}

vm_map_entry_delete:return
/execname == proc && sysmunmap/
{}

kernel:sys_munmap:return
/execname == proc/
{
	trace(probename);
	printf("\n\t\t\t\t\t      ");

	sysmunmap = 0;
}

