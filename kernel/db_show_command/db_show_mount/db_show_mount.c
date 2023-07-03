#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <sys/queue.h>
#include <sys/vnode.h>
#include <sys/mount.h>

static void
mma_db_show_mount(void)
{
	struct mount *mp;
	unsigned int mnt_cnt;

	mnt_cnt = 0;

	printf("%s:\n", __func__);
	TAILQ_FOREACH(mp, &mountlist, mnt_list) {
		mnt_cnt++;
		printf("%p %s on %s (%s)\n", mp,
		    mp->mnt_stat.f_mntfromname,
		    mp->mnt_stat.f_mntonname,
		    mp->mnt_stat.f_fstypename);
	}

	printf("Total mounted FS count:%d\n", mnt_cnt);
}

static int
mma_db_show_mount_modevent(module_t mod, int event, void *arg)
{
	int error = 0;

	switch (event) {
	case MOD_LOAD:
		printf("Hi, db_show_mount\n");
		mma_db_show_mount();
		break;
	case MOD_UNLOAD:
		printf("Bye, db_show_mount\n");
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}

	return error;
}

static moduledata_t mma_db_show_mount_mod = {
	"mma_db_show_mount",
	mma_db_show_mount_modevent,
	NULL
};

DECLARE_MODULE(mma_db_show_mount, mma_db_show_mount_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
