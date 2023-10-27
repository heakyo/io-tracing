#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	1. ./main.d
 *      2. ./main
 */

#pragma D option flowindent

BEGIN
{
	printf("-----IO Tracing Start-----");

	parent = "zsh";
}
/*common*************************************************************************************/
post_execve:entry,
post_execve:return
/execname == parent/
{}

/*entry**************************************************************************************/
sys_execve:entry
/execname == parent/
{
	this->se_td = args[0];
	this->se_uap = args[1];

	printf("td:name:%s",
		this->se_td->td_name
		);

	printf("\n\t\t\t\t\t      ");
	printf("uap:fname:%s",
		copyinstr((uintptr_t)this->se_uap->fname)
		);

	printf("\n\t\t\t\t\t      ");
}

exec_copyin_args:entry
/execname == parent/
{
}

kern_execve:entry
/execname == parent/
{
}

do_execve:entry
/execname == parent/
{
	this->de_td = args[0];
	this->de_args = args[1];
	this->de_mac_p = args[2];

	printf("args:buf:%s begin_argv:%s fname:%s argc:%d fd:%d fdp:%p",
		stringof(this->de_args->buf),
		stringof(this->de_args->begin_argv),
		stringof(this->de_args->fname),
		this->de_args->argc,
		this->de_args->fd,
		this->de_args->fdp
		);

	printf("\n\t\t\t\t\t      ");
}

exec_map_first_page:entry
/execname == parent/
{
	this->emfp_imgp = args[0];

	printf("args:imgp:%p", this->emfp_imgp);

	printf("\n\t\t\t\t\t      ");
	printf("imgp: execpath:%p",
		this->emfp_imgp->execpath
		);

	printf("\n\t\t\t\t\t      ");
	printf("|--image_header:%p", this->emfp_imgp->image_header);

	printf("\n\t\t\t\t\t      ");
	this->emfp_firstpage = this->emfp_imgp->firstpage;
	printf("|--firstpage:%p", this->emfp_firstpage);


	printf("\n\t\t\t\t\t      ");
	this->emfp_proc = this->emfp_imgp->proc;
	printf("|--proc:%p comm:%s",
		this->emfp_proc,
		this->emfp_proc->p_comm
		);

	printf("\n\t\t\t\t\t      ");
	this->emfp_vp =  this->emfp_imgp->vp;
	printf("|--vp:%p tag:%s type:%d data:%p object:%p op:%p",
		this->emfp_vp,
		stringof(this->emfp_vp->v_tag),
		this->emfp_vp->v_type,
		this->emfp_vp->v_data,
		this->emfp_vp->v_bufobj.bo_object,
		this->emfp_vp->v_op
		);
	func((uintptr_t)this->emfp_vp->v_op);

	printf("\n\t\t\t\t\t      ");
	this->emfp_object =  this->emfp_vp->v_bufobj.bo_object;
	printf("|  |--v_object:%p size:%d resident_page_count:%d",
		this->emfp_object,
		this->emfp_object->size,
		this->emfp_object->resident_page_count
		);
	func((uintptr_t)this->emfp_object->handle);

	printf("\n\t\t\t\t\t      ");
	this->emfp_memq = this->emfp_object->memq;
	this->emfp_vmpg = this->emfp_memq.tqh_first;
	printf("|  |  |--pglist(vm_page:%p): pindex:%d, phys_addr:0x%lx flags:0x%x order:%d isi_fstate:%d object:%p",
		this->emfp_vmpg,
		this->emfp_vmpg->pindex,
		this->emfp_vmpg->phys_addr,
		this->emfp_vmpg->flags,
		this->emfp_vmpg->order,
		this->emfp_vmpg->isi_fstate,
		this->emfp_vmpg->object
		);

			/*********************************pv list begin*********************************/
			this->emfp_md = this->emfp_vmpg->md;
			this->emfp_pv_list = this->emfp_md.pv_list;

			/*printf("\n\t\t\t\t\t      ");*/
			/*this->emfp_pv_entry = this->emfp_pv_list.tqh_first;*/
			/*printf("|  |  |  |  |--pv_list(pv_entry): pv_va:0x%p", this->emfp_pv_entry->pv_va);*/
			/*printf("pv_entry:%p", this->emfp_pv_entry);*/
			/*********************************pv list end*********************************/
			/*printf("pv_entry:%p", this->emfp_vp->v_bufobj.bo_object->memq.tqh_first->md.pv_list.tqh_first);*/
			/*printf("pv_va:%p", this->emfp_vp->v_bufobj.bo_object->memq.tqh_first->md.pv_list.tqh_first->pv_va);*/

	printf("\n\t\t\t\t\t      ");
	this->emfp_vmpg = this->emfp_vmpg->listq.tqe_next;
	printf("|  |  |--pglist(vm_page): pindex:%d, phys_addr:0x%lx flags:0x%x order:%d isi_fstate:%d",
		this->emfp_vmpg->pindex,
		this->emfp_vmpg->phys_addr,
		this->emfp_vmpg->flags,
		this->emfp_vmpg->order,
		this->emfp_vmpg->isi_fstate);

	printf("\n\t\t\t\t\t      ");
	this->emfp_inode = (struct inode *)this->emfp_vp->v_data;
	printf("inode:number:%d size:%d",
		this->emfp_inode->i_number,
		this->emfp_inode->i_size
		);
}

vm_page_grab_valid_unlocked:entry
/execname == parent/
{
	this->sba_m = args[0];

	printf("args:m:%p", this->sba_m);
}


/*
 * file: sys\kern\imgact_elf.c
 * function: __CONCAT(exec_, __elfN(imgact))(struct image_params *imgp)
 * line: 1124
 */
exec_elf64_imgact:entry
/execname == parent/
{
	this->eei_imgp = args[0];

	printf("args:imgp:%p", this->eei_imgp);

	printf("\n\t\t\t\t\t      ");
	printf("imgp:entry_addr:%p image_header:%p",
		this->eei_imgp->entry_addr,
		this->eei_imgp->image_header
		);

	printf("\n\t\t\t\t\t      ");
	this->eei_hdr = (const Elf64_Ehdr *)this->eei_imgp->image_header;
	printf("hdr:ident:%s",stringof(this->eei_hdr->e_ident));

	printf("\n\t\t\t\t\t      ");
	printf("hdr:entry:%p type:%d phoff:%d phnum:%d phentsize:%d",
		this->eei_hdr->e_entry,
		this->eei_hdr->e_type,
		this->eei_hdr->e_phoff,
		this->eei_hdr->e_phnum,
		this->eei_hdr->e_phentsize
		);

	/*stack();*/
}

elf64_get_interp:entry
/execname == parent/
{
}

elf64_enforce_limits:entry
/execname == parent/
{
	this->eel_imgp = args[0];
	this->eel_et_dyn_addr = args[3];

	printf("args:et_dyn_addr:%p",
		this->eel_et_dyn_addr);
}

elf64_load_interp:entry
/execname == parent/
{
	this->eli_interp = args[2];
	this->eli_addr = args[3];
	this->eli_entry = args[4];

	printf("args:interp:%p addr:%p entry:0x%x",
		this->eli_interp,
		this->eli_addr,
		*this->eli_entry
		);
}

elf64_load_file:entry
/execname == parent/
{}

/*return*************************************************************************************/

elf64_load_file:return
/execname == parent/
{}

elf64_load_interp:return
/execname == parent/
{
	printf("Return------------------------------------------------");
	printf("\n\t\t\t\t\t      ");
	printf("args:interp:%p addr:%p entry:0x%x",
		this->eli_interp,
		this->eli_addr,
		*this->eli_entry
		);
}

elf64_enforce_limits:return
/execname == parent/
{
}

elf64_get_interp:return
/execname == parent/
{
}

exec_elf64_imgact:return
/execname == parent/
{
	printf("Return------------------------------------------------");
	printf("\n\t\t\t\t\t      ");
	printf("args:imgp:%p", this->eei_imgp);

	printf("\n\t\t\t\t\t      ");
	printf("imgp:entry_addr:%p int/erpreter_name:%p sysent:%p firstpage:%p",
		this->eei_imgp->entry/_addr,
		this->eei_imgp->interpreter_name,
		this->eei_imgp->sysent,
		this->eei_imgp->firstpage
		);

	printf("\n\t\t\t\t\t      ");
	printf("imgp:execpath:%s",
		stringof(this->eei_imgp->execpath)
		);

	printf("\n\t\t\t\t\t      ");
	this->eei_ret_proc = this->eei_imgp->proc;
	printf("proc:comm:%s",
		this->eei_ret_proc->p_comm
		);
}

vm_page_grab_valid_unlocked:return
/execname == parent/
{}

exec_map_first_page:return
/execname == parent/
{
	printf("Return------------------------------------------------");
	printf("\n\t\t\t\t\t      ");

	printf("args:imgp:%p", this->emfp_imgp);
	printf("\n\t\t\t\t\t      ");

	printf("imgp: execpath:%p",
		this->emfp_imgp->execpath
		);

	printf("\n\t\t\t\t\t      ");
	printf("|--image_header:%p", this->emfp_imgp->image_header);

	printf("\n\t\t\t\t\t      ");
	this->emfp_firstpage = this->emfp_imgp->firstpage;
	printf("|--firstpage:%p", this->emfp_firstpage);

	printf("\n\t\t\t\t\t      ");
	printf("|  |--sf_buf:m:%p",
		this->emfp_firstpage); /*sf_buf.h: struct sf_buf;*/

	printf("\n\t\t\t\t\t      ");
	printf("|  |--phys_addr:%p object:%p",
		((struct vm_page *)this->emfp_firstpage)->phys_addr,
		((struct vm_page *)this->emfp_firstpage)->object
		);

	printf("\n\t\t\t\t\t      ");
	this->emfp_hdr = (const Elf64_Ehdr *)this->emfp_imgp->image_header;
	printf("hdr:ident:%s",stringof(this->emfp_hdr->e_ident));

	printf("\n\t\t\t\t\t      ");
	printf("hdr:entry:%p type:%d phoff:%d phnum:%d phentsize:%d",
		this->emfp_hdr->e_entry,
		this->emfp_hdr->e_type,
		this->emfp_hdr->e_phoff,
		this->emfp_hdr->e_phnum,
		this->emfp_hdr->e_phentsize
		);
}

do_execve:return
/execname == parent/
{
}

kern_execve:return
/execname == parent/
{
}

exec_copyin_args:return
/execname == parent/
{
}

sys_execve:return
/execname == parent/
{}


/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
}
