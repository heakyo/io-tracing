#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <sys/queue.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/proc.h>

#include <sys/turnstile.h>
#include <ddb/ddb.h>

extern volatile int db_pager_quit;

static long addr = 1;

#if 0
#include <linux/module.h>

static long addr = 0;
module_param(addr, long, 0444);
MODULE_PARM_DESC(addr, "Thread Address");
#endif

static void
db_cmd_lockchain(void)
{
	//struct thread *td;

	printf("addr:0x%lx\n", addr);
	printf("db_pager_quit:%d\n", db_pager_quit);

#if 0
	td = db_lookup_thread(addr, true);
	printf("td:0x%p\n", td);
	if (!td) {
		printf("The thread on the addr(0x%lx) is not found!\n", addr);
		return;
	}

	//print_lockchain(td, "");
#endif
}

static void
db_cmd_allchains(void)
{
	struct thread *td;
	struct proc *p;
	int i;

	i = 1;

	printf("%s:\n", __func__);
	FOREACH_PROC_IN_SYSTEM(p) {
		FOREACH_THREAD_IN_PROC(p, td) {
			if ((TD_ON_LOCK(td) && LIST_EMPTY(&td->td_contested))
			    || (TD_IS_INHIBITED(td) && TD_ON_SLEEPQ(td))) {
				db_printf("chain %d:\n", i++);
				print_lockchain(td, " ");
			}
			if (db_pager_quit)
				return;
		}
	}
}

static int
lockchain_modevent(module_t mod, int event, void *arg)
{
	int error = 0;

	switch (event) {
	case MOD_LOAD:
		printf("Hi db_cmd_show_lockchain\n");
		db_cmd_lockchain();
		db_cmd_allchains();
		break;
	case MOD_UNLOAD:
		printf("Bye db_cmd_show_lockchain\n");
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}

	return error;
}

static moduledata_t lockchain_mod = {
	"lockchain",
	lockchain_modevent,
	NULL
};

DECLARE_MODULE(lockchain, lockchain_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
//TUNABLE_INT64("hw.lockchain.addr", &addr);

