AC_INIT([squire], [0.0.2])
AC_CANONICAL_HOST
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AM_SILENT_RULES([yes])

AC_CONFIG_SUBDIRS([kernel])

AC_ARG_VAR(bootdir, [Directory for boot files, defaults to exec_prefix/boot])

if test "X$bootdir" = "X" ; then
	bootdir=$exec_prefix/boot
fi

AC_CONFIG_FILES([
	Makefile
])

AC_OUTPUT
