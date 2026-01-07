all: mount-nullfs

mount-nullfs:
	mount -t nullfs /usr/src /mnt/src

umount-nullfs:
	umount /mnt/src

read-file:
	./nullfs-test.d -c 'cat /mnt/src/123'
