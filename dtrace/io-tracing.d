#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./io-tracing.d -p $(ps -aux | grep "lw-container nfs" | head -1 | awk '{print $2}') -o io-tracing.log
*/

#pragma D option flowindent

struct _NFS_EXEC_CONTEXT {
	void *pDriverData;
	void *pContext;

	struct LW_PROMISE_GROUP_ *pGroup;
	struct svc_req *pRpcSvcReq;
	SVCXPRT* pRpcTransport;

	unsigned int RpcProgram;
	unsigned int RpcVersion;
	unsigned int RpcProcedure;
};

BEGIN
{
	printf("-----IO Tracing Start-----");
}

/*common*************************************************************************************/
pid$target::NfsIoWrite:entry,
pid$target::NfsIoWrite:return,

pid$target::IoWriteVectorFile:entry,
pid$target::IoWriteVectorFile:return,

pid$target::IopReadWriteVectorFile:entry,
pid$target::IopReadWriteVectorFile:return,

pid$target::IopIrpDispatch:entry,
pid$target::IopIrpDispatch:return,

pid$target::IoFmIrpDispatchContinue:entry,
/*pid$target::IoFmIrpDispatchContinue:return,*/

pid$target::IopIrpCompleteInternal:entry,
pid$target::IopIrpCompleteInternal:return,

pid$target::IopFmIrpStateDispatchFsdExec:entry,
pid$target::IopFmIrpStateDispatchFsdExec:return,

pid$target::OnefsDriverDispatch:entry,
pid$target::OnefsDriverDispatch:return,

pid$target::OnefsAsyncStart:entry,
pid$target::OnefsAsyncStart:return,

pid$target::OnefsIrpWork:entry,
pid$target::OnefsIrpWork:return,

pid$target::OnefsIrpSpark:entry,
pid$target::OnefsIrpSpark:return,

pid$target::OnefsWriteInternal:entry,
pid$target::OnefsWriteInternal:return,

pid$target::OnefsSysWriteVec:entry,
pid$target::OnefsSysWriteVec:return,

pid$target::OneFS_lwext_write:entry,
pid$target::OneFS_lwext_write:return,

syscall:freebsd:lwextsvc_write:entry,
syscall:freebsd:lwextsvc_write:return
{
	/*printf("pname:%s(%d) tname:%s(%d)", execname, pid, curthread->td_name, tid);*/
	/*printf("timestamp:%d", timestamp);*/
}

/*entry**************************************************************************************/
pid$target:nfs.so:NfsSocketProcessTask:entry
{}

pid$target:nfs.so:NfsProtoNfs3CallDispatch:entry
{
	printf("pname:%s(%d) tname:%s(%d)", execname, pid, curthread->td_name, tid);
}

pid$target:nfs.so:NfsProtoNfs3Dispatch:entry
{
	this->pExecContext = (struct _NFS_EXEC_CONTEXT *)arg0;

	printf("pname:%s(%d) tname:%s(%d) ", execname, pid, curthread->td_name, tid);
	printf("pExecContext:%p ", this->pExecContext);
	/*printf("\t\t\t\t\t\t|->\n");*/
	/*printf("NfsProc:%d", this->pExecContext->RpcProcedure);*/
}

pid$target:nfs.so:NfsStatsOpBegin:entry
/arg3 == 7/
{
	/* arg3<-->operation: WRITE. REF: _NFS3_OP_RECORD_STAT */

	self->pExecContext = arg0;
	this->stats_op_delta = arg1;
	this->protocol = arg2;
	this->operation = arg3;
	/*@[execname, pid, stack()] = count();*/

	printf("pExecContext:%p ", self->pExecContext);
	printf("delta:%p ", this->stats_op_delta);
	printf("protocol:%d ", this->protocol);
	printf("operation:%d ", this->operation);
	printf("timestamp:%d", timestamp);

	if (0) {
		ustack();
	}
}

ISP_OP_END:entry
{
	this->d = args[0];

	printf("enabled:%x ", this->d->enabled);
}

isp_op_end_:entry
{
	this->delta = args[0];

	printf("exec:%s delta:%p ", execname, this->delta);
	printf("enabled:%x ", this->delta->enabled);
	printf("timestamp:%d begin:%d ", timestamp, this->delta->begin);
}

/*return*************************************************************************************/

isp_op_end_:return
{
}

ISP_OP_END:return
{}

pid$target:nfs.so:NfsStatsOpBegin:return
/this->operation == 7/
{
}

pid$target::IoFmIrpDispatchContinue:return
{
	printf("status:%x", arg1);
}

pid$target:nfs.so:NfsProtoNfs3Dispatch:return
{}

pid$target:nfs.so:NfsProtoNfs3CallDispatch:return
{}

pid$target:nfs.so:NfsSocketProcessTask:return
{}

END
{
	printf("-----IO Tracing END-----");
}
