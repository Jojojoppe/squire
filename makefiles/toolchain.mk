.PHONY: newlib_hosted

TARGET_BARE			= $(ARCH)-elf
TARGET_HOSTED		= $(ARCH)-squire

all: newlib_hosted

# DOWNLOAD SOURCES
# ----------------
$(PREFIX)/tar/binutils-2.31_squire.tar.gz:
	-mkdir $(PREFIX)/tar
	curl -k https://joppeb.nl//projects/squire/tools/alpha/binutils-2.31_squire.tar.gz > $(PREFIX)/tar/binutils-2.31_squire.tar.gz
$(PREFIX)/src/.binutils_untar: $(PREFIX)/tar/binutils-2.31_squire.tar.gz
	-mkdir $(PREFIX)/src
	tar -xf $(PREFIX)/tar/binutils-2.31_squire.tar.gz -C $(PREFIX)/src
	touch $(PREFIX)/src/.binutils_untar

$(PREFIX)/tar/gcc-8.2.0_squire.tar.gz:
	-mkdir $(PREFIX)/tar
	curl -k https://joppeb.nl//projects/squire/tools/alpha/gcc-8.2.0_squire.tar.gz > $(PREFIX)/tar/gcc-8.2.0_squire.tar.gz
$(PREFIX)/src/.gcc_untar: $(PREFIX)/tar/gcc-8.2.0_squire.tar.gz
	-mkdir $(PREFIX)/src
	tar -xf $(PREFIX)/tar/gcc-8.2.0_squire.tar.gz -C $(PREFIX)/src
	touch $(PREFIX)/src/.gcc_untar

$(PREFIX)/tar/autoconf-2.65_squire.tar.gz:
	-mkdir $(PREFIX)/tar
	curl -k https://joppeb.nl//projects/squire/tools/alpha/autoconf-2.65_squire.tar.gz > $(PREFIX)/tar/autoconf-2.65_squire.tar.gz
$(PREFIX)/src/.autoconf_untar: $(PREFIX)/tar/autoconf-2.65_squire.tar.gz
	-mkdir $(PREFIX)/src
	tar -xf $(PREFIX)/tar/autoconf-2.65_squire.tar.gz -C $(PREFIX)/src
	touch $(PREFIX)/src/.autoconf_untar

$(PREFIX)/tar/automake-1.11_squire.tar.gz:
	-mkdir $(PREFIX)/tar
	curl -k https://joppeb.nl//projects/squire/tools/alpha/automake-1.11_squire.tar.gz > $(PREFIX)/tar/automake-1.11_squire.tar.gz
$(PREFIX)/src/.automake_untar: $(PREFIX)/tar/automake-1.11_squire.tar.gz
	-mkdir $(PREFIX)/src
	tar -xf $(PREFIX)/tar/automake-1.11_squire.tar.gz -C $(PREFIX)/src
	touch $(PREFIX)/src/.automake_untar

$(PREFIX)/tar/newlib-2.50_squire.tar.gz:
	-mkdir $(PREFIX)/tar
	curl -k https://joppeb.nl//projects/squire/tools/alpha/newlib-2.50_squire.tar.gz > $(PREFIX)/tar/newlib-2.50_squire.tar.gz
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
	-mkdir $(PREFIX)/src/build-binutils_bare
	cd $(PREFIX)/src/build-binutils_bare ;\
		../binutils-2.31_squire/configure --target=$(TARGET_BARE) --prefix="$(PREFIX)" --with-sysroot --disable-nls --disable-werror; \
		touch ../.binutils_configure
$(PREFIX)/src/.binutils: $(PREFIX)/src/.binutils_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-binutils_bare ;\
		make; \
		make install; \
		touch ../.binutils

$(PREFIX)/src/.gcc_configure: $(PREFIX)/src/.gcc_untar
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir $(PREFIX)/src/build-gcc_bare
	cd $(PREFIX)/src/build-gcc_bare; \
		../gcc-8.2.0_squire/configure --target=$(TARGET_BARE) --prefix="$(PREFIX)" --disable-nls --enable-languages=c,c++ --without-headers; \
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
	-mkdir $(PREFIX)/src/build-autoconf_bare
	cd $(PREFIX)/src/build-autoconf_bare; \
		../autoconf-2.65_squire/configure --prefix="$(PREFIX)"; \
		touch ../.autoconf_configure
$(PREFIX)/src/.autoconf: $(PREFIX)/src/.autoconf_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-autoconf_bare ;\
		make; \
		make install; \
		touch ../.autoconf

$(PREFIX)/src/.automake_configure: $(PREFIX)/src/.automake_untar
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir $(PREFIX)/src/build-automake_bare
	cd $(PREFIX)/src/build-automake_bare; \
		../automake-1.11_squire/configure --prefix="$(PREFIX)"; \
		touch ../.automake_configure
$(PREFIX)/src/.automake: $(PREFIX)/src/.automake_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-automake_bare ;\
		make; \
		make install; \
		touch ../.automake

$(PREFIX)/src/.newlib_configure: $(PREFIX)/src/.newlib_untar
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir $(PREFIX)/src/build-newlib_bare
	cd $(PREFIX)/src/build-newlib_bare; \
		../newlib-2.50_squire/configure --prefix="$(PREFIX)" --target=$(TARGET_HOSTED); \
		touch ../.newlib_configure
$(PREFIX)/src/.newlib: $(PREFIX)/src/.autoconf $(PREFIX)/src/.automake $(PREFIX)/src/.binutils $(PREFIX)/src/.gcc $(PREFIX)/src/.newlib_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm $(PREFIX)/bin/i386-squire-*
	cd $(PREFIX)/bin; \
		ln -s i386-elf-ar i386-squire-ar; \
		ln -s i386-elf-as i386-squire-as; \
		ln -s i386-elf-gcc i386-squire-cc; \
		ln -s i386-elf-gcc i386-squire-gcc; \
		ln -s i386-elf-ranlib i386-squire-ranlib;
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
	-mkdir $(PREFIX)/src/build-binutils_hosted
	cd $(PREFIX)/src/build-binutils_hosted ;\
		../binutils-2.31_squire/configure --target=$(TARGET_HOSTED) --prefix="$(PREFIX)" --with-sysroot="$(PREFIX)" --disable-werror; \
		touch ../.binutils_configure_hosted
$(PREFIX)/src/.binutils_hosted: $(PREFIX)/src/.binutils_configure_hosted
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-binutils_hosted ;\
		make; \
		make install; \
		touch ../.binutils_hosted

$(PREFIX)/src/.gcc_configure_hosted:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir $(PREFIX)/src/build-gcc_hosted
	cd $(PREFIX)/src/build-gcc_hosted; \
		../gcc-8.2.0_squire/configure --target=$(TARGET_HOSTED) --prefix="$(PREFIX)" --with-sysroot="$(PREFIX)" --enable-languages=c,c++; \
		touch ../.gcc_configure_hosted
$(PREFIX)/src/.gcc_hosted: $(PREFIX)/src/.newlib $(PREFIX)/src/.binutils_hosted $(PREFIX)/src/.gcc_configure_hosted
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd $(PREFIX)/src/build-gcc_hosted ;\
		make all-gcc; \
	   	make install-gcc; \
		make all-target-libgcc; \
		make install-target-libgcc; \
		touch ../.gcc_hosted

$(PREFIX)/src/.newlib_configure_hosted:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir $(PREFIX)/src/build-newlib_hosted
	cd $(PREFIX)/src/build-newlib_hosted; \
		../newlib-2.50_squire/configure --prefix="$(PREFIX)" --target=$(TARGET_HOSTED); \
		touch ../.newlib_configure_hosted
newlib_hosted: env_check $(PREFIX)/src/.newlib  $(PREFIX)/src/.automake $(PREFIX)/src/.autoconf $(PREFIX)/src/.binutils_hosted $(PREFIX)/src/.gcc_hosted $(PREFIX)/src/.newlib_configure_hosted
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm $(PREFIX)/bin/i386-squire-cc
	cd $(PREFIX)/bin; \
		ln -s i386-squire-gcc i386-squire-cc
	cd $(PREFIX)/src/build-newlib_hosted ;\
		make all; \
		make install
	-mkdir -p $(PREFIX)/usr/include
	-mkdir -p $(PREFIX)/usr/lib
	cp -RT $(PREFIX)/$(ARCH)-squire/include $(PREFIX)/usr/include
	cp -RT $(PREFIX)/$(ARCH)-squire/lib $(PREFIX)/usr/lib
