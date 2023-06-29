#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

static int
hello_modevent(module_t mod, int event, void *arg)
{
	int error = 0;

	switch (event) {
	case MOD_LOAD:
		uprintf("Hello World\n");
		break;
	case MOD_UNLOAD:
		uprintf("Bye bye World\n");
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}

	return error;
}

static moduledata_t hello_mod = {
	"hello",
	hello_modevent,
	NULL
};

DECLARE_MODULE(hello, hello_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
