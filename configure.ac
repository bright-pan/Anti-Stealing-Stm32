#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([stm32], [1.0], [loststriker@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CC
AM_PROG_AS
# Checks for libraries.
AC_DISABLE_SHARED
AC_PROG_LIBTOOL
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
				 lib/std_periph/Makefile
				 lib/std_periph/src/Makefile
				 lib/cm3/Makefile
				 lib/cm3/src/Makefile
				 lib/bsp/Makefile
				 lib/bsp/system/Makefile
				 lib/bsp/system/src/Makefile
				 lib/bsp/interrupt/Makefile
				 lib/bsp/interrupt/src/Makefile
				 lib/bsp/init/Makefile
				 lib/bsp/init/src/Makefile
				 lib/ucos/Makefile
				 lib/ucos/ports/Makefile
				 lib/ucos/ports/src/Makefile
				 lib/ucos/core/Makefile
				 lib/ucos/core/src/Makefile
				 src/Makefile])
AC_OUTPUT
