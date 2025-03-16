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
}

/*-----------------------------------------------------------------------------*/

adadone:return
/execname == proc/
{}

