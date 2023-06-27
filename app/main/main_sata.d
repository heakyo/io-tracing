#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./main_sata.d -c './main -r /dev/ada0p6'
*/

#pragma D option flowindent

BEGIN
{
	printf("-----IO Tracing Start-----");

	proc = "main";
	unmapped_bufp = (long *)0xffffffff83ea6cd0;

	printf("\n\t\t\t\t\t      ");
	printf("unmapped_buf:0x%p", *unmapped_bufp);
}
/*common*************************************************************************************/

/*entry**************************************************************************************/
physio:entry
/execname == proc/
{
	this->physio_dev = args[0];
	this->physio_uio = args[1];
	this->physio_ioflag = args[2];

	this->physio_si_devsw = this->physio_dev->si_devsw;

	printf("args:ioflag:0x%x", this->physio_ioflag);

	printf("\n\t\t\t\t\t      ");
	printf("cdev:name:%s flags:0x%x iosize_max:%dKB devsw:0x%p",\
			this->physio_dev->si_name,
			this->physio_dev->si_flags,
			(this->physio_dev->si_iosize_max/1024),
			this->physio_dev->si_devsw
			);

	printf("\n\t\t\t\t\t      ");
	printf("cdevsw:name:%s",\
			stringof(this->physio_si_devsw->d_name)
			);

	printf("\n\t\t\t\t\t      ");
	printf("uio:[0][base:0x%p len:%d], iovcnt:%d resid:%d offset:%d segflg:%d rw:%d",\
			this->physio_uio->uio_iov[0].iov_base,
			this->physio_uio->uio_iov[0].iov_len,
			this->physio_uio->uio_iovcnt,
			this->physio_uio->uio_resid,
			this->physio_uio->uio_offset,
			this->physio_uio->uio_segflg,
			this->physio_uio->uio_rw
			);
}

vm_fault_quick_hold_pages:entry
/execname == proc/
{}

g_dev_strategy:entry,
g_disk_start:entry
/execname == proc/
{
	this->as_bp = args[0];

	printf("bio:cmd:%d offset:%d bcount:%d pblkno:%d data:0x%p length:%d from:0x%p to:0x%p", \
			this->as_bp->bio_cmd,
			this->as_bp->bio_offset,
			this->as_bp->bio_bcount,
			this->as_bp->bio_pblkno,
			this->as_bp->bio_data,
			this->as_bp->bio_length,
			this->as_bp->bio_from,
			this->as_bp->bio_to
			);
}

dastrategy:entry
/execname == proc/
{
	this->as_bp = args[0];

	this->as_bio_disk = this->as_bp->bio_disk;
	this->as_d_geom = this->as_bio_disk->d_geom;
	this->as_class = this->as_d_geom->class;
	this->as_bio_ma = this->as_bp->bio_ma;
	this->as_object = this->as_bio_ma[0]->object;

	printf("bio:cmd:%d offset:%d bcount:%d pblkno:%d data:0x%p flags:0x%x ma_n:%d resid:%d length:%d completed:%d ma_offset:0x%d from:0x%p to:0x%p", \
			this->as_bp->bio_cmd,
			this->as_bp->bio_offset,
			this->as_bp->bio_bcount,
			this->as_bp->bio_pblkno,
			this->as_bp->bio_data,
			this->as_bp->bio_flags,
			this->as_bp->bio_ma_n,
			this->as_bp->bio_resid,
			this->as_bp->bio_length,
			this->as_bp->bio_completed,
			this->as_bp->bio_ma_offset,
			this->as_bp->bio_from,
			this->as_bp->bio_to
			);

	printf("\n\t\t\t\t\t      ");
	printf("bio:data[0]:0x%p", this->as_bp->bio_data);

	printf("\n\t\t\t\t\t      ");

	printf("disk:name:%s unit:%d sectorsize:%d mediasize:%d ident:%s",\
			stringof(this->as_bio_disk->d_name),
			this->as_bio_disk->d_unit,
			this->as_bio_disk->d_sectorsize,
			this->as_bio_disk->d_mediasize,
			this->as_bio_disk->d_ident
                        );


	printf("\n\t\t\t\t\t      ");
	printf("geom:name:%s",\
			stringof(this->as_d_geom->name)
			);

	printf("\n\t\t\t\t\t      ");
	printf("class:name:%s",\
			stringof(this->as_class->name)
			);

	printf("\n\t\t\t\t\t      ");
	printf("vm_page:%p:phys_addr:0x%p order:%d object:%p pindex:%d", \
			this->as_bio_ma[0],
			this->as_bio_ma[0]->phys_addr,
			this->as_bio_ma[0]->order,
			this->as_bio_ma[0]->object,
			this->as_bio_ma[0]->pindex
			);

	printf("\n\t\t\t\t\t      ");
	printf("vm_object:type:%d size:%d",\
			this->as_object->type,
			this->as_object->size
			);

if(0) {

	func((uintptr_t)this->as_bio_disk->d_open);
	func((uintptr_t)this->as_bio_disk->d_close);
	func((uintptr_t)this->as_bio_disk->d_strategy);

	printf("\n\t\t\t\t\t      ");
	printf("data:0x%x", *(char *)this->as_bio_ma[0]->phys_addr);
}

}

/*return*************************************************************************************/
dastrategy:return
/execname == proc/
{}

g_dev_strategy:return,
g_disk_start:return
/execname == proc/
{}

vm_fault_quick_hold_pages:return
/execname == proc/
{}

physio:return
/execname == proc/
{}


/********************************************************************************************/
END
{
	printf("-----IO Tracing END-----");
}
