#!/usr/sbin/dtrace -s

/*
 *      ./rhw_cam.d -c './main'
 */

#pragma D option flowindent

BEGIN
{
        proc = "main";
	sysread = 0;
}

/*sys*************************************************************************************/
/* probemod:probefunc:probename */
kernel:sys_read:*
/execname == proc/
{
	trace(probename);

	if (probename == "entry") {
		sysread = 1;
	} else if (probename == "return") {
		sysread = 0;
	}
}


/*CAM*************************************************************************************/
adastrategy:entry
/execname == proc && sysread/
{
	this->bp = args[0];

	self->periph = (struct cam_periph *)this->bp->bio_disk->d_drv1;
	self->softc = (struct ada_softc *)self->periph->softc;

	printf("periph:name:%s%d",
		stringof(self->periph->periph_name),
		self->periph->unit_number
		);
	printf("\n\t\t\t\t\t      ");

	printf("ada_softc:disk:%p",
		self->softc->disk
		);
	printf("\n\t\t\t\t\t      ");

	/*************************** disk ********************************/
	self->disk = self->softc->disk;
	printf("disk:ident:%s",
		self->disk->d_ident
		);
	printf("\n\t\t\t\t\t      ");

	/*************************** disk_params ********************************/
	self->disk_params = self->softc->params;
	printf("disk_params:heads:%d cylinders:%d",
		self->disk_params.heads,
		self->disk_params.cylinders
		);
}

/*-----------------------------------------------------------------------------*/
adastrategy:return
/execname == proc && sysread/
{}

