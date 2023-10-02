#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./ptrace.d -c './main'
 *
 * #define PT_TRACE_ME 0
 * #define PT_CONTINUE 7
 * #define PT_ATTACH 10
 *
 */

#pragma D option flowindent

BEGIN
{
	printf("-----IO Tracing Start-----");

	proc = "main";
}
/*common*************************************************************************************/
syscall::ptrace:entry,
syscall::ptrace:return
/execname == proc && 0/
{}


/*entry**************************************************************************************/
sys_ptrace:entry
/execname == proc/
{
	this->td = args[0];
	self->uap = args[1];

	printf("ptrace_args:req:%d",
		self->uap->req
		);

	printf("\n\t\t\t\t\t      ");
}

kern_ptrace:entry
/execname == proc/
{
	this->kp_td = args[0];

	this->kp_p = this->kp_td->td_proc;

	printf("proc:%p: comm:%s flag:%p flag2:%p",
		this->kp_p,
		this->kp_p->p_comm,
		this->kp_p->p_flag,
		this->kp_p->p_flag2
		);
}

proc_set_traced:entry
/execname == proc/
{}

/*return*************************************************************************************/

proc_set_traced:return
/execname == proc/
{}

kern_ptrace:return
/execname == proc/
{
	printf("proc:%p: comm:%s flag:%p flag2:%p",
		this->kp_p,
		this->kp_p->p_comm,
		this->kp_p->p_flag,
		this->kp_p->p_flag2
		);
}

sys_ptrace:return
/execname == proc/
{

}

/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
}
