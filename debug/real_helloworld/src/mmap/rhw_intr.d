#!/usr/sbin/dtrace -s

/*
 *      ./rhw.d -c './main'
 *      dtrace -n 'bdone:entry /execname == "intr"/ {stack();}'
 */

#pragma D option flowindent

BEGIN
{
        proc = "intr"
}

/*CAM*************************************************************************************/
adadone:entry
/execname == proc/
{
	this->periph = args[0];
	this->done_ccb = args[1];

	printf("periph:%p done_ccb:%p",
		this->periph,
		this->done_ccb
		);
	printf("\n\t\t\t\t\t      ");

	/********************** cam_periph ************************/
	printf("periph:name:%s unit_number:%d",
		stringof(this->periph->periph_name),
		this->periph->unit_number
		);
	printf("\n\t\t\t\t\t      ");

	/********************** cam_path ************************/
	this->path = this->periph->path;
	this->ccbh_path = this->done_ccb->ccb_h.path;

	printf("path:%p ccbh_path:%p",
		this->path,
		this->ccbh_path
		);
	printf("\n\t\t\t\t\t      ");

	/********************** cam_eb ************************/
	this->bus = this->path->bus;
	printf("bus:id:%d",
		this->bus->path_id
		);
	printf("\n\t\t\t\t\t      ");

	/********************** cam_et ************************/
	this->target = this->path->target;
	printf("target:id:%d refcount:%d",
		this->target->target_id,
		this->target->refcount
		);
	printf("\n\t\t\t\t\t      ");

	/********************** cam_ed ************************/
	this->device = this->path->device;
	printf("dev:serial_num:%s lun_id:%d",
		stringof(this->device->serial_num),
		this->device->lun_id
		);
}

/*-----------------------------------------------------------------------------*/

adadone:return
/execname == proc/
{}

