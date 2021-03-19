AC_INIT([squire kernel], [0.0.2])

AC_CONFIG_HEADERS([src/config.h])
AC_CANONICAL_HOST
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
dnl AM_SILENT_RULES([yes])

hostarch=unkown
hostplatform=unknown
extracflags=""

case "$host_cpu" in
	i*86)
		hostarch=i386
		hostplatform=
		;;
	x86_64)
		hostarch=x86_64
		hostplatform=
		;;
	arm | armv7[arm])
		hostarch=arm
		case "$platform" in
			ebaz4205)
				hostplatform=ebaz4205
				CFLAGS+=-mcpu=cortex-a9
				;;
			*)
				hostplatform=unknown
				;;
		esac
		;;
	*)
		hostarch=unknown
		;;
esac
if test "$hostarch" = "unknown" ; then
	AC_MSG_ERROR(["Unsupported cpu type '$host_cpu'"]);
fi
if test "$hostplatform" = "unknown" ; then
	AC_MSG_ERROR(["Unsupported platform '$platform' for '$host_cpu'"]);
fi
ARCH_DIR=src/arch/$hostarch
AC_SUBST([ARCH_DIR])
PLATFORM_DIR="platform/$hostplatform"
AC_SUBST([PLATFORM_DIR])
AC_SUBST([hostplatform])

LIBS=-lgcc
LDFLAGS="${LDFLAGS} -nostdlib -static"
AC_PROG_CC
AM_PROG_AS
AM_PROG_AR
AC_PROG_RANLIB

dnl CONFIGURE ARGUMENTS
dnl -------------------
AC_ARG_VAR(platform, [Platform for various host types])
AC_ARG_VAR(bootdir, [Directory for boot files, defaults to exec_prefix/boot])
if test "X$bootdir" = "X" ; then
	bootdir=$exec_prefix/boot
fi
AC_ARG_VAR(kernel_log_lenght, [Length of kernel log block in bytes, defaults to 1kB (1024)])
if test "X$kernel_log_length" = "X" ; then
	kernel_log_length=1024
fi
AC_DEFINE_UNQUOTED([KERNEL_LOG_LENGTH], [$kernel_log_length], [Length of kernel log])
AC_ARG_VAR(kernel_log_print, [Enable or disable the kernel log debug print, defaults to true (1)])
if test "X$kernel_log_print" = "X" ; then
	kernel_log_print=1
fi
AC_DEFINE_UNQUOTED([KERNEL_LOG_PRINT_ENABLE], [$kernel_log_print], [Enable kernel log debug print])
dnl -------------------

AC_CONFIG_FILES([
	Makefile
	src/arch/arm/Makefile
	src/arch/i386/Makefile
	src/arch/x86_64/Makefile
])

AC_OUTPUT
