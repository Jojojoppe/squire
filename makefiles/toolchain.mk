.PHONY: toolchain toolchain_rebuild compiler compiler_uninstall

PWD = $(shell pwd)

toolchain: compiler
toolchain_rebuild: compiler_uninstall toolchain

compiler: $(PREFIX)/bin/i386-tcc

$(PREFIX)/bin/i386-tcc:
	-rm -rf $(PREFIX)/src/build_tcc
	-mkdir -p $(PREFIX)/src/build_tcc
	cd $(PREFIX)/src/build_tcc; \
		$(PWD)/tools/tcc/configure --enable-cross --cpu=$(ARCH) --triplet=$(ARCH)-elf-squire --prefix=$(PREFIX) \
			--libpaths=$(PREFIX)/lib --sysincludepaths=$(PREFIX)/include:$(PREFIX)/lib/tcc/include \
			--crtprefix=$(PREFIX)/lib/ --libdir=$(PREFIX)/lib --includedir=$(PREFIX)/include ;\
		make -j cross-$(ARCH); \
		make install

compiler_uninstall:
	cd $(PREFIX)/src/build_tcc; \
		make uninstall
