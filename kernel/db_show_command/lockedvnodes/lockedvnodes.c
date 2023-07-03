#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <sys/queue.h>
#include <sys/vnode.h>
#include <sys/mount.h>

static void
db_cmd_lockedvnods(void)
{
	struct mount *mp;
	struct vnode *vp;

	unsigned int mnt_cnt;

	mnt_cnt = 0;

	printf("%s\n", __func__);
	TAILQ_FOREACH(mp, &mountlist, mnt_list) {
		TAILQ_FOREACH(vp, &mp->mnt_nvnodelist, v_nmntvnodes) {
			if (vp->v_type != VMARKER && VOP_ISLOCKED(vp)) {
				vn_printf(vp, "vnode ");
				printf("lock status:%d\n", lockstatus(vp->v_vnlock));
			}
		}
	}
}

static int
lockednodes_modevent(module_t mod, int event, void *arg)
{
	int error = 0;

	switch (event) {
	case MOD_LOAD:
		printf("Hi db_cmd_lockedvnods\n");
		db_cmd_lockedvnods();
		break;
	case MOD_UNLOAD:
		printf("Bye db_cmd_lockedvnods\n");
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}

	return error;
}

static moduledata_t lockednodes_mod = {
	"lockednodes",
	lockednodes_modevent,
	NULL
};

DECLARE_MODULE(lockednodes, lockednodes_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
