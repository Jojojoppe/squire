.PHONY: compiler

PWD = $(shell pwd)

compiler: $(PREFIX)/bin/i386-tcc

$(PREFIX)/bin/i386-tcc:
	-rm -rf $(PREFIX)/src/build_tcc
	mkdir $(PREFIX)/src/build_tcc
	cd $(PREFIX)/src/build_tcc; \
		$(PWD)/tools/tinycc/configure --enable-cross --cpu=$(ARCH) --triplet=$(ARCH)-elf-squire --prefix=$(PREFIX) \
			--libpaths=$(PREFIX)/usr/lib:$(PREFIX)/lib --sysincludepaths=$(PREFIX)/include:$(PREFIX)/usr/include:$(PREFIX)/lib/tcc/include \
			--crtprefix=$(PREFIX)/usr/lib/ ; \
		make -j cross-$(ARCH); \
		make install

compiler_uninstall:
	cd $(PREFIX)/src/build_tcc; \
		make uninstall