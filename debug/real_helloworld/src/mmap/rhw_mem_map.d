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
	vmsfork = 0;
	vmocflag = 0;
	vmoref = 0;
	vmorefl = 0;
}


/*pager*************************************************************************************/
vm_pager_has_page:*
/0&&execname == proc/
{}

default_pager_*:entry
/0&&execname == proc/
{
	stack();
}

/******************************************************************************************/
default_pager_*:return
/0&&execname == proc/
{}

pmap_pvh_remove:*
/0&&execname == proc && sysmunmap/
{
	@[stack()]=count();
}

vm_object_clear_flag:entry
/1&&execname == proc && args[0]->size == 4 && vmsfork/
{
	self->vocf_obj = args[0];
	vmocflag = 1;

	printf("obj:%p size:%d ref_count:%d",
		self->vocf_obj,
		self->vocf_obj->size,
		self->vocf_obj->ref_count
		);
	printf("\n\t\t\t\t\t      ");

	/*stack();*/
}

vm_object_reference:entry
/0&&execname == proc && args[0]->size == 4/
{
	self->vor_obj = args[0];
	vmoref = 1;

	printf("obj:%p size:%d ref_count:%d",
		self->vor_obj,
		self->vor_obj->size,
		self->vor_obj->ref_count
		);
	printf("\n\t\t\t\t\t      ");

	/*stack();*/
}

vm_object_reference_locked:entry
/0&&execname == proc && args[0]->size == 4 && vmsfork/
{
	self->vorl_obj = args[0];
	vmorefl = 1;

	printf("obj:%p size:%d ref_count:%d",
		self->vorl_obj,
		self->vorl_obj->size,
		self->vorl_obj->ref_count
		);
	printf("\n\t\t\t\t\t      ");

	stack();
}

/******************************************************************************************/

vm_object_reference_locked:return
/0&&execname == proc && vmorefl && vmsfork/
{
	vmorefl = 0;
	trace(probename);
}

vm_object_reference:return
/0&&execname == proc && vmoref/
{
	vmoref = 0;
	trace(probename);
}

vm_object_clear_flag:return
/1&&execname == proc && vmocflag && vmsfork/
{
	vmocflag = 0;
	trace(probename);
}

/*vm_map*************************************************************************************/
vmspace_fork:entry
/1&&execname == proc/
{
	vmsfork = 1;
	trace(probename);
}

vm_map_copy_entry:entry
/0&&execname == proc && vmsfork/
{
	self->src_entry = args[2];
	self->dst_entry = args[3];

	printf("src_entry:%p:start:%x end:%x obj:%p",
		self->src_entry,
		self->src_entry->start,
		self->src_entry->end,
		self->src_entry->object.vm_object
		);
	printf("\n\t\t\t\t\t      ");

	printf("dst_entry:%p:start:%x end:%x obj:%p",
		self->dst_entry,
		self->dst_entry->start,
		self->dst_entry->end,
		self->dst_entry->object.vm_object
		);
}

vm_map_copy_swap_object:entry
/0&&execname == proc && vmsfork/
{}

vm_object_shadow:entry
/0&&execname == proc/
{
	self->vos_object = args[0];
	self->offset = (vm_ooffset_t *)args[1];

	printf("offset:%d",
		*self->offset
		);
}

vm_object_allocate_anon:entry
/0&&execname == proc/
{
	stack();
}

vm_object_backing_insert_ref:entry
/0&&execname == proc/
{
	self->vobir_obj = args[0];
	self->vobir_backing_obj = args[1];

	printf("obj:%p shadow_count:%d backing_obj:%p shadow_count:%d",
		self->vobir_obj,
		self->vobir_obj->shadow_count,
		self->vobir_backing_obj,
		self->vobir_backing_obj->shadow_count
		);
}

/******************************************************************************************/
vm_object_backing_insert_ref:return
/0&&execname == proc/
{}

vm_object_allocate_anon:return
/0&&execname == proc/
{}

vm_object_shadow:return
/0&&execname == proc/
{
	printf("obj:%p offset:%d",
		*self->vos_object,
		*self->offset
		);
}

vm_map_copy_swap_object:return
/0&&execname == proc && vmsfork/
{}

vm_map_copy_entry:return
/0&&execname == proc && vmsfork/
{}

vmspace_fork:return
/1&&execname == proc/
{
	vmsfork = 0;
	trace(probename);
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

	printf("entry:%p:start:%x end:%x",
		self->entry,
		self->entry->start,
		self->entry->end
		);
	printf("\n\t\t\t\t\t      ");

	printf("obj:%p size:%d flags:0x%x type:%d ref_count:%d backing_object:%p shadow_count:%d res:%d busy:%d",
		self->object,
		self->object->size,
		self->object->flags,
		self->object->type,
		self->object->ref_count,
		self->object->backing_object,
		self->object->shadow_count,
		self->object->resident_page_count,
		self->object->busy.__count
		);

	if (self->object->backing_object) {
		self->backing_object = self->object->backing_object;
		printf("backing_obj:%p size:%d flags:0x%x type:%d ref_count:%d backing_object:%p shadow_count:%d res:%d busy:%d",
			self->backing_object,
			self->backing_object->size,
			self->backing_object->flags,
			self->backing_object->type,
			self->backing_object->ref_count,
			self->backing_object->backing_object,
			self->object->shadow_count,
			self->object->resident_page_count,
			self->object->busy.__count
			);
		printf("\n\t\t\t\t\t      ");

		/*shadow chain*/
		self->shadow_head = self->backing_object->shadow_head;
		printf("sh:lh_first:%p",
			self->shadow_head.lh_first
			);

		if (self->backing_object->backing_object) {

			self->backing_object = self->backing_object->backing_object;
			printf("backing_obj:%p size:%d flags:0x%x type:%d ref_count:%d backing_object:%p shadow_count:%d res:%d busy:%d",
				self->backing_object,
				self->backing_object->size,
				self->backing_object->flags,
				self->backing_object->type,
				self->backing_object->ref_count,
				self->backing_object->backing_object,
				self->object->shadow_count,
				self->object->resident_page_count,
				self->object->busy.__count
				);
		}
	}
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

	printf("obj:%p size:%d flags:0x%x type:%d ref_count:%d backing_object:%p shadow_count:%d res:%d busy:%d",
		self->object,
		self->object->size,
		self->object->flags,
		self->object->type,
		self->object->ref_count,
		self->object->backing_object,
		self->object->shadow_count,
		self->object->resident_page_count,
		self->object->busy.__count
		);

	if (self->object->backing_object) {
		self->backing_object = self->object->backing_object;
		printf("backing_obj:%p size:%d flags:0x%x type:%d ref_count:%d backing_object:%p shadow_count:%d res:%d busy:%d",
			self->backing_object,
			self->backing_object->size,
			self->backing_object->flags,
			self->backing_object->type,
			self->backing_object->ref_count,
			self->backing_object->backing_object,
			self->object->shadow_count,
			self->object->resident_page_count,
			self->object->busy.__count
			);
	}
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
		printf("backing_obj:%p size:%d flags:0x%x type:%d ref_count:%d backing_object:%p",
			self->backing_object,
			self->backing_object->size,
			self->backing_object->flags,
			self->backing_object->type,
			self->backing_object->ref_count,
			self->backing_object->backing_object
			);
		printf("\n\t\t\t\t\t      ");

if(0) {
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

}

vm_object_collapse_scan:entry
/execname == proc && sysmunmap/
{
	self->object = args[0];

	self->backing_obj = self->object->backing_object;

	this->backing_offset_index = self->object->backing_object_offset;

	printf("obj:%p backing_obj:%p",
		self->object,
		self->backing_obj
		);
	printf("\n\t\t\t\t\t      ");

	this->b_p0 = self->backing_obj->memq.tqh_first;
	printf("backing_obj:%p backing_offset_index:%x b_p0:%p",
		self->backing_obj,
		this->backing_offset_index,
		this->b_p0
		);
	printf("\n\t\t\t\t\t      ");

	this->p0 = self->object->memq.tqh_first;
	printf("backing_obj:%p p0:%p",
		self->object,
		this->p0
		);
	printf("\n\t\t\t\t\t      ");

	/*stack();*/
}

vm_page_tryxbusy:entry
/execname == proc && sysmunmap/
{
	self->m = args[0];

	printf("m:%p pindex:%d",
		self->m,
		self->m->pindex
		);
}

vm_object_collapse_scan_wait:entry
/execname == proc && sysmunmap/
{}

vm_page_remove:*
/execname == proc && sysmunmap/
{}

vm_page_lookup:entry
/execname == proc && sysmunmap/
{
	self->object = args[0];
	self->pindex = 0;

	printf("obj:%p pindex:%d",
		self->object,
		self->pindex
		);
}

/*****************************************************************************************/
vm_page_lookup:return
/execname == proc && sysmunmap/
{
	self->pp = args[1];

	printf("pp:%p",
		self->pp
		);
}

vm_object_collapse_scan_wait:return
/execname == proc && sysmunmap/
{}

vm_page_tryxbusy:return
/execname == proc && sysmunmap/
{}

vm_object_collapse_scan:return
/execname == proc && sysmunmap/
{
}

vm_object_backing_collapse_wait:return
/execname == proc && sysmunmap/
{
	self->ret_backing_object = args[1];

	printf("backing_object:%p",
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

