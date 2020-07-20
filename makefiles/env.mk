env_check:
# MAKE ENVIRONMENT VARIABLES
# --------------------------
#  PREFIX:		the system root for the toolchain
ifndef PREFIX
	$(error PREFIX not defined! Run make with PREFIX environment variable)
endif
# ARCH:			the architecture for wich to cross compile
# Accepted architectures:
# 	- i386
ifndef ARCH
	$(error ARCH not defined! Run make with ARCH environment variable)
endif
ifeq ($(ARCH), i386)
else
	$(error "ARCH [${ARCH}] not accepted!")
endif
