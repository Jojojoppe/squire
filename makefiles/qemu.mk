.PHONY: run runtty debug

# Run the test suite
run:
	echo + Run test suite
	qemu-system-$(ARCH) -s -m 64M -no-reboot -drive file=drive,format=raw -monitor stdio

# Run the test suite
runtty:
	echo + Run test suite in terminal
	qemu-system-$(ARCH) -s -m 64M -no-reboot -drive file=drive,format=raw,id=disk -nographic
# Debug
debug:
	echo + Debug
	qemu-system-$(ARCH) -s -S -m 64M -no-shutdown -no-reboot -drive file=drive,format=raw &
	sleep 1
	gdb -x kernel/DEBUG
