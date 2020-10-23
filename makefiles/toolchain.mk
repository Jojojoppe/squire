.PHONY: newlib_hosted

TARGET_BARE			= $(ARCH)-elf
TARGET_HOSTED		= $(ARCH)-squire

# DOWNLOAD SOURCES
# ----------------
$(PREFIX)/tar/binutils-2.31_squire.tar.gz:
	-mkdir $(PREFIX)/tar
	curl -k https://git.joppeb.nl/squire/binutils.git/snapshot/binutils-master.tar.gz > $(PREFIX)/tar/binutils-2.31_squire.tar.gz
$(PREFIX)/src/.binutils_untar: $(PREFIX)/tar/binutils-2.31_squire.tar.gz
	-mkdir $(PREFIX)/src
	tar -xf $(PREFIX)/tar/binutils-2.31_squire.tar.gz -C $(PREFIX)/src
	touch $(PREFIX)/src/.binutils_untar

$(PREFIX)/tar/gcc-8.2.0_squire.tar.gz:
	-mkdir $(PREFIX)/tar
	curl -k https://git.joppeb.nl/squire/gcc.git/snapshot/gcc-master.tar.gz/ > $(PREFIX)/tar/gcc-8.2.0_squire.tar.gz
$(PREFIX)/src/.gcc_untar: $(PREFIX)/tar/gcc-8.2.0_squire.tar.gz
	-mkdir $(PREFIX)/src
	tar -xf $(PREFIX)/tar/gcc-8.2.0_squire.tar.gz -C $(PREFIX)/src
	touch $(PREFIX)/src/.gcc_untar

$(PREFIX)/tar/autoconf-2.65_squire.tar.gz:
	-mkdir $(PREFIX)/tar
	curl -k https://git.joppeb.nl/squire/autoconf.git/snapshot/autoconf-master.tar.gz > $(PREFIX)/tar/autoconf-2.65_squire.tar.gz
$(PREFIX)/src/.autoconf_untar: $(PREFIX)/tar/autoconf-2.65_squire.tar.gz
	-mkdir $(PREFIX)/src
	tar -xf $(PREFIX)/tar/autoconf-2.65_squire.tar.gz -C $(PREFIX)/src
	touch $(PREFIX)/src/.autoconf_untar

$(PREFIX)/tar/automake-1.11_squire.tar.gz:
	-mkdir $(PREFIX)/tar
	curl -k https://git.joppeb.nl/squire/automake.git/snapshot/automake-master.tar.gz > $(PREFIX)/tar/automake-1.11_squire.tar.gz
$(PREFIX)/src/.automake_untar: $(PREFIX)/tar/automake-1.11_squire.tar.gz
	-mkdir $(PREFIX)/src
	tar -xf $(PREFIX)/tar/automake-1.11_squire.tar.gz -C $(PREFIX)/src
	touch $(PREFIX)/src/.automake_untar

$(PREFIX)/tar/newlib-2.50_squire.tar.gz:
	-mkdir $(PREFIX)/tar
	curl -k https://git.joppeb.nl/squire/newlib.git/snapshot/newlib-master.tar.gz > $(PREFIX)/tar/newlib-2.50_squire.tar.gz
$(PREFIX)/src/.newlib_untar: $(PREFIX)/tar/newlib-2.50_squire.tar.gz
	-mkdir $(PREFIX)/src
	tar -xf $(PREFIX)/tar/newlib-2.50_squire.tar.gz -C $(PREFIX)/src
	touch $(PREFIX)/src/.newlib_untar

# Clean all files except for tarballs
clean-local:
	rm -rf $(PREFIX)/src $(PREFIX)/bin $(PREFIX)/share $(PREFIX)/$(ARCH)* $(PREFIX)/include $(PREFIX)/lib $(PREFIX)/libexec $(PREFIX)/usr
# Clean all files
clean-all:
	rm -rf $(PREFIX)/src $(PREFIX)/tar $(PREFIX)/bin $(PREFIX)/share $(PREFIX)/$(ARCH)* $(PREFIX)/include $(PREFIX)/lib $(PREFIX)/libexec $(PREFIX)/usr

# Build bare variants
$(PREFIX)/src/.binutils_configure: $(PREFIX)/src/.binutils_untar
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm -rf $(PREFIX)/src/build-binutils_bare
	-mkdir $(PREFIX)/src/build-binutils_bare
	cd $(PREFIX)/src/build-binutils_bare ;\
		../binutils-master/configure --target=$(TARGET_BARE) --prefix="$(PREFIX)" --with-sysroot --disable-nls --disable-werror; \
		touch ../.binutils_configure
$(PREFIX)/src/.binutils: $(PREFIX)/src/.binutils_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-binutils_bare ;\
		make; \
		make install; \
		touch ../.binutils

$(PREFIX)/src/.gcc_configure: $(PREFIX)/src/.gcc_untar
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm -rf $(PREFIX)/src/build-gcc_bare
	-mkdir $(PREFIX)/src/build-gcc_bare
	cd $(PREFIX)/src/build-gcc_bare; \
		../gcc-master/configure --target=$(TARGET_BARE) --prefix="$(PREFIX)" --disable-nls --enable-languages=c,c++ --without-headers; \
		touch ../.gcc_configure
$(PREFIX)/src/.gcc: $(PREFIX)/src/.binutils $(PREFIX)/src/.gcc_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-gcc_bare ;\
		make all-gcc; \
	   	make install-gcc; \
		make all-target-libgcc; \
		make install-target-libgcc; \
		touch ../.gcc

$(PREFIX)/src/.autoconf_configure: $(PREFIX)/src/.autoconf_untar
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm -rf $(PREFIX)/src/build-autoconf_bare
	-mkdir $(PREFIX)/src/build-autoconf_bare
	cd $(PREFIX)/src/build-autoconf_bare; \
		../autoconf-master/configure --prefix="$(PREFIX)"; \
		touch ../.autoconf_configure
$(PREFIX)/src/.autoconf: $(PREFIX)/src/.autoconf_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-autoconf_bare ;\
		make; \
		make install; \
		touch ../.autoconf

$(PREFIX)/src/.automake_configure: $(PREFIX)/src/.automake_untar
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm -rf $(PREFIX)/src/build-automake_bare
	-mkdir $(PREFIX)/src/build-automake_bare
	cd $(PREFIX)/src/build-automake_bare; \
		../automake-master/configure --prefix="$(PREFIX)"; \
		touch ../.automake_configure
$(PREFIX)/src/.automake: $(PREFIX)/src/.automake_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-automake_bare ;\
		make; \
		make install; \
		touch ../.automake

$(PREFIX)/src/.newlib_configure: $(PREFIX)/src/.newlib_untar
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm -rf $(PREFIX)/src/build-newlib_bare
	-mkdir $(PREFIX)/src/build-newlib_bare
	-rm $(PREFIX)/src/.newlib
	-rm $(PREFIX)/src/.newlib_configure_hosted
	cd $(PREFIX)/src/build-newlib_bare; \
		../newlib-master/configure --prefix="$(PREFIX)" --target=$(TARGET_HOSTED); \
		touch ../.newlib_configure
	-rm $(PREFIX)/src/.binutils_hosted
	-rm $(PREFIX)/src/.gcc_hosted
	-rm $(PREFIX)/bin/$(ARCH)-squire-*
	cd $(PREFIX)/bin; \
		ln -s $(ARCH)-elf-ar $(ARCH)-squire-ar; \
		ln -s $(ARCH)-elf-as $(ARCH)-squire-as; \
		ln -s $(ARCH)-elf-gcc $(ARCH)-squire-cc; \
		ln -s $(ARCH)-elf-gcc $(ARCH)-squire-gcc; \
		ln -s $(ARCH)-elf-ranlib $(ARCH)-squire-ranlib;
$(PREFIX)/src/.newlib: $(PREFIX)/src/.autoconf $(PREFIX)/src/.automake $(PREFIX)/src/.binutils $(PREFIX)/src/.gcc $(PREFIX)/src/.newlib_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-newlib_bare ;\
		make all; \
		make install
	-mkdir -p $(PREFIX)/usr/include
	-mkdir -p $(PREFIX)/usr/lib
	cp -RT $(PREFIX)/$(ARCH)-squire/include $(PREFIX)/usr/include
	cp -RT $(PREFIX)/$(ARCH)-squire/lib $(PREFIX)/usr/lib
	touch $(PREFIX)/src/.newlib

# Build hosted variants
$(PREFIX)/src/.binutils_configure_hosted:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm -rf $(PREFIX)/src/build-binutils_hosted
	-mkdir $(PREFIX)/src/build-binutils_hosted
	cd $(PREFIX)/src/build-binutils_hosted ;\
		../binutils-master/configure --target=$(TARGET_HOSTED) --prefix="$(PREFIX)" --with-sysroot="$(PREFIX)" --disable-werror; \
		touch ../.binutils_configure_hosted
$(PREFIX)/src/.binutils_hosted: $(PREFIX)/src/.binutils_configure_hosted
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-binutils_hosted ;\
		make -j; \
		make install; \
		touch ../.binutils_hosted

$(PREFIX)/src/.gcc_configure_hosted:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm -rf $(PREFIX)/src/build-gcc_hosted
	-mkdir $(PREFIX)/src/build-gcc_hosted
	cd $(PREFIX)/src/build-gcc_hosted; \
		../gcc-master/configure --target=$(TARGET_HOSTED) --prefix="$(PREFIX)" --with-sysroot="$(PREFIX)" --enable-languages=c,c++; \
		touch ../.gcc_configure_hosted
$(PREFIX)/src/.gcc_hosted: $(PREFIX)/src/.newlib $(PREFIX)/src/.binutils_hosted $(PREFIX)/src/.gcc_configure_hosted
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-gcc_hosted ;\
		make all-gcc -j; \
	   	make install-gcc; \
		make all-target-libgcc -j; \
		make install-target-libgcc; \
		touch ../.gcc_hosted

$(PREFIX)/src/.newlib_configure_hosted:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm -rf $(PREFIX)/src/build-newlib_hosted
	-mkdir $(PREFIX)/src/build-newlib_hosted
	-rm .$(PREFIX)/src/.newlib_hosted
	cd $(PREFIX)/src/build-newlib_hosted; \
		../newlib-master/configure --prefix="$(PREFIX)" --target=$(TARGET_HOSTED); \
		touch ../.newlib_configure_hosted
	-rm $(PREFIX)/bin/$(ARCH)-squire-cc
	cd $(PREFIX)/bin; \
		ln -s $(ARCH)-squire-gcc $(ARCH)-squire-cc
$(PREFIX)/src/.newlib_hosted: $(PREFIX)/src/.newlib  $(PREFIX)/src/.automake $(PREFIX)/src/.autoconf $(PREFIX)/src/.binutils_hosted $(PREFIX)/src/.gcc_hosted $(PREFIX)/src/.newlib_configure_hosted
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-newlib_hosted ;\
		make all -j; \
		make install
	-mkdir -p $(PREFIX)/usr/include
	-mkdir -p $(PREFIX)/usr/lib
	cp -RT $(PREFIX)/$(ARCH)-squire/include $(PREFIX)/usr/include
	cp -RT $(PREFIX)/$(ARCH)-squire/lib $(PREFIX)/usr/lib
	touch $(PREFIX)/src/.newlib_hosted

newlib: libsquire_headers $(PREFIX)/src/.newlib_hosted
	echo + Newlib compiled

rebuild_newlib: libsquire_headers
	cd $(PREFIX)/src/build-newlib_hosted ;\
		make clean
	-rm $(PREFIX)/src/.newlib_hosted
	# -rm $(PREFIX)/src/.newlib_configure_hosted
	# rm $(PREFIX)/src/.gcc_hosted
	# rm $(PREFIX)/src/.binutils_hosted
	${MAKE} ${MFLAGS} newlib
