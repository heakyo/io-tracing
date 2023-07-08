#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./hello-dtrace.d
*/

BEGIN
{
	printf("-----IO Tracing Start-----");

	/*mountlist = (struct mntlist *)(*(long *)0xffffffff825fbba0);*/
	mountlist = *(struct mntlist *)0xffffffff825fbba0;

	printf("\n\t\t\t\t    ");
/*
	printf("mountlist:0x%p\n",
			mountlist);
*/

	mp = mountlist.tqh_first;
	mp0 = mountlist.tqh_first;
	mp1 = mp0->mnt_list.tqe_next;
	mp2 = mp1->mnt_list.tqe_next;
	mp3 = mp2->mnt_list.tqe_next;
	mp4 = mp3->mnt_list.tqe_next;
	mp5 = mp4->mnt_list.tqe_next;
	mp6 = mp5->mnt_list.tqe_next;

	printf("\n\t\t\t\t    ");
	mp0_mnt_opt = mp0->mnt_opt;
	printf("mountlist:[0]:%p %s on %s (%s)",\
			mp0, mp0->mnt_stat.f_mntfromname, mp0->mnt_stat.f_mntonname, mp0->mnt_stat.f_fstypename
			);

	exit(0);
}
/*common*************************************************************************************/

/*entry**************************************************************************************/

/*return*************************************************************************************/


/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
}
