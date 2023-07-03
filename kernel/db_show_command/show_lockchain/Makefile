# $FreeBSD$

.PATH: ${SRCTOP}/sys/mma/db_show_command/show_lockchain

KMOD= show_lockchain
SRCS= show_lockchain.c
SRCS+= vnode_if.h opt_ddb.h

CFLAGS+= -I${.CURDIR}/../../compat/linuxkpi/common/include

.include <bsd.kmod.mk>
