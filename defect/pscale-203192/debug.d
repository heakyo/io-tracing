#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./debug.d
 */

#pragma D option flowindent

BEGIN
{
	printf("-----IO Tracing Start-----");
}
/*common*************************************************************************************/

/*entry**************************************************************************************/
_linux_pci_register_driver:entry
{
	this->lprd_pdrv = args[0];
	this->lprd_dc = args[1];

	printf("pdrv:name:%s",
		stringof(this->lprd_pdrv->name)
		);

	printf("\n\t\t\t\t\t      ");
	printf("dc:name:%s",
		stringof(this->lprd_dc->name)
		);
}

devclass_add_driver:entry
{}

devclass_driver_added:entry
{}

pci_driver_added:entry
{
	this->pda_dev = args[0];
	this->pda_driver = args[1];

	printf("dev:nameunit:%p",
		this->pda_dev->type->name
		);
}

device_attach:entry
{}

device_get_children:entry
{}

device_attach:entry
{}

/*return*************************************************************************************/
device_attach:return
{}

device_get_children:return
{}

device_attach:entry
{}

pci_driver_added:return
{}

devclass_driver_added:return
{}

devclass_add_driver:return
{}

_linux_pci_register_driver:return
{}

/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
	printf("\n\t\t\t\t\t      ");
}
