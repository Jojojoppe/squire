#.SILENT:
.PHONY: sources clean-local clean-all binutils_bare gcc_bare automake autoconf

all: bare hosted
bare: sources binutils_bare gcc_bare autoconf_bare automake_bare newlib_bare
hosted: binutils_hosted gcc_hosted newlib_hosted

sources: \
	src/.binutils_untar \
	src/.gcc_untar \
	src/.autoconf_untar \
	src/.automake_untar \
	src/.newlib_untar \

# DOWNLOAD SOURCES
# ----------------
tar/binutils-2.31_squire.tar.gz:
	-mkdir tar
	curl -k https://joppeb.nl//projects/squire/tools/alpha/binutils-2.31_squire.tar.gz > tar/binutils-2.31_squire.tar.gz
src/.binutils_untar: tar/binutils-2.31_squire.tar.gz
	-mkdir src
	tar -xf tar/binutils-2.31_squire.tar.gz -C src
	touch src/.binutils_untar

tar/gcc-8.2.0_squire.tar.gz:
	-mkdir tar
	curl -k https://joppeb.nl//projects/squire/tools/alpha/gcc-8.2.0_squire.tar.gz > tar/gcc-8.2.0_squire.tar.gz
src/.gcc_untar: tar/gcc-8.2.0_squire.tar.gz
	-mkdir src
	tar -xf tar/gcc-8.2.0_squire.tar.gz -C src
	touch src/.gcc_untar

tar/autoconf-2.65_squire.tar.gz:
	-mkdir tar
	curl -k https://joppeb.nl//projects/squire/tools/alpha/autoconf-2.65_squire.tar.gz > tar/autoconf-2.65_squire.tar.gz
src/.autoconf_untar: tar/autoconf-2.65_squire.tar.gz
	-mkdir src
	tar -xf tar/autoconf-2.65_squire.tar.gz -C src
	touch src/.autoconf_untar

tar/automake-1.11_squire.tar.gz:
	-mkdir tar
	curl -k https://joppeb.nl//projects/squire/tools/alpha/automake-1.11_squire.tar.gz > tar/automake-1.11_squire.tar.gz
src/.automake_untar: tar/automake-1.11_squire.tar.gz
	-mkdir src
	tar -xf tar/automake-1.11_squire.tar.gz -C src
	touch src/.automake_untar

tar/newlib-2.50_squire.tar.gz:
	-mkdir tar
	curl -k https://joppeb.nl//projects/squire/tools/alpha/newlib-2.50_squire.tar.gz > tar/newlib-2.50_squire.tar.gz
src/.newlib_untar: tar/newlib-2.50_squire.tar.gz
	-mkdir src
	tar -xf tar/newlib-2.50_squire.tar.gz -C src
	touch src/.newlib_untar

# Clean all files except for tarballs
clean-local:
	rm -rf src bin share i386* 
# Clean all files
clean-all:
	rm -rf src tar bin share i386* 

# Build bare variants
PREFIX_BARE			= $(shell pwd)
PREFIX_HOSTED		= $(shell pwd)
TARGET_BARE			= i386-elf
TARGET_HOSTED		= i386-squire

src/.binutils_configure:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir src/build-binutils_bare
	cd src/build-binutils_bare ;\
		../binutils-2.31_squire/configure --target=$(TARGET_BARE) --prefix="$(PREFIX_BARE)" --with-sysroot --disable-nls --disable-werror; \
		touch ../.binutils_configure
binutils_bare: src/.binutils_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd src/build-binutils_bare ;\
		make; \
		make install

src/.gcc_configure:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir src/build-gcc_bare
	cd src/build-gcc_bare; \
		../gcc-8.2.0_squire/configure --target=$(TARGET_BARE) --prefix="$(PREFIX_BARE)" --disable-nls --enable-languages=c,c++ --without-headers; \
		touch ../.gcc_configure
gcc_bare: src/.gcc_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd src/build-gcc_bare ;\
		make all-gcc; \
	   	make install-gcc; \
		make all-target-libgcc; \
		make install-target-libgcc

src/.autoconf_configure:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir src/build-autoconf_bare
	cd src/build-autoconf_bare; \
		../autoconf-2.65_squire/configure --prefix="$(PREFIX_BARE)"; \
		touch ../.autoconf_configure
autoconf_bare: src/.autoconf_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd src/build-autoconf_bare ;\
		make; \
		make install

src/.automake_configure:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir src/build-automake_bare
	cd src/build-automake_bare; \
		../automake-1.11_squire/configure --prefix="$(PREFIX_BARE)"; \
		touch ../.automake_configure
automake_bare: src/.automake_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd src/build-automake_bare ;\
		make; \
		make install

src/.newlib_configure:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir src/build-newlib_bare
	cd src/build-newlib_bare; \
		../newlib-2.50_squire/configure --prefix="$(PREFIX_BARE)" --target=$(TARGET_HOSTED); \
		touch ../.newlib_configure
newlib_bare: src/.newlib_configure
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm bin/i386-squire-*
	cd bin; \
		ln -s i386-elf-ar i386-squire-ar; \
		ln -s i386-elf-as i386-squire-as; \
		ln -s i386-elf-gcc i386-squire-cc; \
		ln -s i386-elf-gcc i386-squire-gcc; \
		ln -s i386-elf-ranlib i386-squire-ranlib; 
	cd src/build-newlib_bare ;\
		make all; \
		make install
	-mkdir -p usr/include
	-mkdir -p usr/lib
	cp -RT i386-squire/include usr/include
	cp -RT i386-squire/lib usr/lib

# Build hosted variants
src/.binutils_configure_hosted:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir src/build-binutils_hosted
	cd src/build-binutils_hosted ;\
		../binutils-2.31_squire/configure --target=$(TARGET_HOSTED) --prefix="$(PREFIX_HOSTED)" --with-sysroot="$(PREFIX_HOSTED)" --disable-werror; \
		touch ../.binutils_configure_hosted
binutils_hosted: src/.binutils_configure_hosted
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd src/build-binutils_hosted ;\
		make; \
		make install

src/.gcc_configure_hosted:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir src/build-gcc_hosted
	cd src/build-gcc_hosted; \
		../gcc-8.2.0_squire/configure --target=$(TARGET_HOSTED) --prefix="$(PREFIX_HOSTED)" --with-sysroot="$(PREFIX_HOSTED)" --enable-languages=c,c++; \
		touch ../.gcc_configure_hosted
gcc_hosted: src/.gcc_configure_hosted
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	cd src/build-gcc_hosted ;\
		make all-gcc; \
	   	make install-gcc; \
		make all-target-libgcc; \
		make install-target-libgcc

src/.newlib_configure_hosted:
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-mkdir src/build-newlib_hosted
	cd src/build-newlib_hosted; \
		../newlib-2.50_squire/configure --prefix="$(PREFIX_HOSTED)" --target=$(TARGET_HOSTED); \
		touch ../.newlib_configure_hosted
newlib_hosted: src/.newlib_configure_hosted
	$(eval export PATH=$(PREFIX)/bin:$(PATH))
	-rm bin/i386-squire-cc
	cd bin; \
		ln -s i386-squire-gcc i386-squire-cc
	cd src/build-newlib_hosted ;\
		make all; \
		make install
	-mkdir -p usr/include
	-mkdir -p usr/lib
	cp -RT i386-squire/include usr/include
	cp -RT i386-squire/lib usr/lib
