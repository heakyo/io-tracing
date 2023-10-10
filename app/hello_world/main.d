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
	printf("imgp:entry_addr:%p interpreter_name:%p sysent:%p",
		this->eei_imgp->entry_addr,
		this->eei_imgp->interpreter_name,
		this->eei_imgp->sysent
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
