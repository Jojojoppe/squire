# MAKE EVIRONMENT VARIABLES
# --------------------------
#  PREFIX:		the system root for the toolchain
ifndef PREFIX
$(error PREFIX not defined! Run make with PREFIX environment variable)
endif
# ARCH:			the erchitecture for wich to cross compile
# Accepted architectures:
# 	- i386
ifndef ARCH
$(error ARCH not defined! Run make with ARCH environment variable)
endif
ifeq ($(ARCH), i386)
else ifeq($(ARCH), x86_64)
else
$(error "ARCH [${ARCH}] not accepted!")
endif
