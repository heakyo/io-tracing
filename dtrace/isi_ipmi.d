#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./isi_ipmi.d -c '/usr/bin/isi_hwtools/isi_fputils -g'
*/

#pragma D option flowindent

BEGIN
{
	proc = "isi_fputil";
	printf("-----IO Tracing Start-----");
}
/*common*************************************************************************************/

/*entry**************************************************************************************/
ipmi_ioctl:entry
/execname == proc/
{
	this->cdev = args[0];
	this->cmd = args[1];

	self->ii_sc = (struct ipmi_softc *)this->cdev->si_drv1;

	this->ii_fp = curthread->td_fpop;
	this->ii_p = this->ii_fp->f_vnun.fvn_cdevpriv;
	this->ii_datap = this->ii_p->cdpd_data;
	this->ii_dev = (struct ipmi_device *)this->ii_datap;

	printf("args:cmd:0x%p",
			this->cmd
		);
	printf("\n\t\t\t\t\t      ");

	printf("ipmi_pending_requests empty:%p", self->ii_sc->ipmi_pending_requests.tqh_first);
	printf("\n\t\t\t\t\t      ");

	printf("dev:%p ipmi_requests:%d",
			this->ii_dev,
			this->ii_dev->ipmi_requests
		);

}

ipmi_polled_enqueue_request:entry
/execname == proc/
{
}

ipmi_dequeue_request:entry
{}


/*return*************************************************************************************/
ipmi_dequeue_request:return
{}

ipmi_polled_enqueue_request:return
/execname == proc/
{}

ipmi_ioctl:return
/execname == proc/
{
	printf("ipmi_pending_requests empty:%p", self->ii_sc->ipmi_pending_requests.tqh_first);
}


/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
}
