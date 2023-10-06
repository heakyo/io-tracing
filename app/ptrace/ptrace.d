#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./ptrace.d -c './main'
 * 	./ptrace.d -c './ptrace_attach_example pid'
 *
 * #define PT_TRACE_ME  0
 * #define PT_CONTINUE  7
 * #define PT_ATTACH    10
 * #define PT_DETACH    11
 *
 * ./ptrace_attach_example:
 *     1. run example first
 *     2. ./ptrace_attach_example `pgrep example`
 */

#pragma D option flowindent

BEGIN
{
	printf("-----IO Tracing Start-----");

	/*proc = "main";*/
	proc = "ptrace_attach_examp";

	/*g_pf_p = (struct proc *)0;*/
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
}

kern_ptrace:entry
/execname == proc/
{
	this->kp_td = args[0];
	this->kp_req = args[1];

	this->kp_p = this->kp_td->td_proc;

	printf("req:%d",
		this->kp_req
		);

	printf("\n\t\t\t\t\t      ");

	printf("proc:%p: comm:%s flag:%p flag2:%p",
		this->kp_p,
		this->kp_p->p_comm,
		this->kp_p->p_flag,
		this->kp_p->p_flag2
		);

}

pfind:entry
/execname == proc/
{
	this->pf_pid = args[0];

	printf("pid:%d",
		this->pf_pid
		);
}

proc_set_traced:entry
/execname == proc/
{
	this->pst_p = args[0];
	this->pst_stop = args[1];

	printf("p:%p stop:%d",
		this->pst_p,
		this->pst_stop
		);

	printf("\n\t\t\t\t\t      ");
	printf("proc: comm:%s flag:%p flag2:%p",
		this->pst_p->p_comm,
		this->pst_p->p_flag,
		this->pst_p->p_flag2
	);
}

kern_psignal:entry
/execname == proc/
{
	this->kpe_p = args[0];
	this->kpe_sig = args[1];

	printf("p:%p sig:%d",
		this->kpe_p,
		this->kpe_sig
	);
}

/*return*************************************************************************************/

kern_psignal:return
/execname == proc/
{}

proc_set_traced:return
/execname == proc/
{
	printf("proc: comm:%s flag:%p flag2:%p pid:%d",
		this->pst_p->p_comm,
		this->pst_p->p_flag,
		this->pst_p->p_flag2,
		this->pst_p->p_pid
		);

	printf("\n\t\t\t\t\t      ");
	this->pst_pptr = this->pst_p->p_pptr;
	printf("pp: comm:%s pid:%d",
		this->pst_pptr->p_comm,
		this->pst_pptr->p_pid
		);
}

pfind:return
/execname == proc/
{
	this->pf_p = (struct proc *)args[1];

	printf("ret:proc:%p comm:%s flag:%p flag2:%p",
		this->pf_p,
		this->pf_p->p_comm,
		this->pf_p->p_flag,
		this->pf_p->p_flag2
		);
}

kern_ptrace:return
/execname == proc/
{
	printf("proc:%p: comm:%s flag:%p flag2:%p",
		this->kp_p,
		this->kp_p->p_comm,
		this->kp_p->p_flag,
		this->kp_p->p_flag2
		);

	printf("\n\t\t\t\t\t      ");
	printf("proc:%p comm:%s",
		this->pf_p,
		this->pf_p->p_comm
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
