.PHONY: reset

# Make drive
drive:
	echo + Make drive
	dd if=/dev/zero status=progress of=drive bs=512 count=131072
	echo + Write partitions
	( echo 2048,,L,* ) | sfdisk drive
	echo + Mount partitions
	mkdir mnt
	sudo losetup /dev/loop0 drive
	sudo losetup /dev/loop1 drive -o 1048576
	sudo mkdosfs -F32 -f 2 /dev/loop1
	sudo mount /dev/loop1 mnt
	sudo mkdir mnt/boot
	echo + Install GRUB
	sudo grub-install --target=i386-pc --boot-directory=mnt/boot --modules "normal part_msdos ext2 multiboot" --force /dev/loop0
	echo + Finish
	sync
	sudo umount mnt
	sudo losetup -d /dev/loop1
	sudo losetup -d /dev/loop0
	rmdir mnt

# Reset all
# note: this will delete the drive with all its files
reset:
	echo + Remove drive
	-rm drive
	-rm -rf mnt
	${MAKE} clean

# Mount drive
mount:
	echo + Mount drive
	sudo losetup /dev/loop0 drive
	sudo losetup /dev/loop1 drive -o 1048576
	mkdir mnt
	sudo mount /dev/loop1 mnt

# Unmount drive
umount:
	echo + Unmount drive
	sync
	sudo umount mnt
	sudo losetup -d /dev/loop1
	sudo losetup -d /dev/loop0
	rmdir mnt
