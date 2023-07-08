#!/usr/sbin/dtrace -s

/*
 * test command:
 * 	./main.d -c './main -r /dev/ada0p6'
*/

#pragma D option flowindent

BEGIN
{
	printf("-----IO Tracing Start-----");

	proc = "main";
}
/*common*************************************************************************************/

/*entry**************************************************************************************/
physio:entry
/execname == proc/
{
	this->physio_cdev = args[0];
	this->physio_uio = args[1];
	this->physio_ioflag = args[2];

	printf("cdev:si_name:%s",\
			this->physio_cdev->si_name);

	printf("uio:iov[0].iov_base:0x%p",\
			this->physio_uio->uio_iov[0].iov_base);
}

adastrategy:entry
/execname == proc/
{
	this->as_bp = args[0];

	this->as_bio_disk = this->as_bp->bio_disk;
	this->as_d_geom = this->as_bio_disk->d_geom;
	this->as_class = this->as_d_geom->class;

	printf("bio:cmd:%d offset:%d bcount:%d pblkno:%d data:0x%p",\
			this->as_bp->bio_cmd,
			this->as_bp->bio_offset,
			this->as_bp->bio_bcount,
			this->as_bp->bio_pblkno,
			this->as_bp->bio_data
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

if(0) {

	func((uintptr_t)this->as_bio_disk->d_open);
	func((uintptr_t)this->as_bio_disk->d_close);
	func((uintptr_t)this->as_bio_disk->d_strategy);
}

}

/*return*************************************************************************************/
adastrategy:return
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
