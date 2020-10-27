.PHONY: compiler

compiler: $(PREFIX)/bin/i386-tcc

$(PREFIX)/bin/i386-tcc:
	-rm -rf tools/tinycc/build
	mkdir tools/tinycc/build
	cd tools/tinycc/build; \
		../configure --enable-cross --cpu=$(ARCH) --triplet=$(ARCH)-elf-squire --prefix=$(PREFIX) \
			--libpaths=$(PREFIX)/usr/lib:$(PREFIX)/lib --sysincludepaths=$(PREFIX)/include:$(PREFIX)/usr/include:$(PREFIX)/lib/tcc/include \
			--crtprefix=$(PREFIX)/usr/lib/ ; \
		make -j cross-$(ARCH); \
		make install

compiler_uninstall:
	cd tools/tinycc/build; \
		make uninstall