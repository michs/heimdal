dnl $Id$
dnl
dnl CHECK_COMPILE_ET
AC_DEFUN([CHECK_COMPILE_ET], [

AC_CHECK_PROG(COMPILE_ET, compile_et, [compile_et])

krb_cv_compile_et="no"
if test "${COMPILE_ET}" = "compile_et"; then

dnl We have compile_et.  Now let's see if it supports `prefix' and `index'.
AC_MSG_CHECKING(whether compile_et has the features we need)
cat > conftest.et <<'EOF'
error_table conf
prefix CONFTEST
error_code CODE1, "CODE1"
index 128
error_code CODE2, "CODE2"
end
EOF
if ${COMPILE_ET} conftest.et >/dev/null 2>&1; then
        krb_cv_compile_et="yes"
fi
AC_MSG_RESULT(${krb_cv_compile_et})
rm -fr conftest*
fi

if test "${krb_cv_compile_et}" = "yes"; then
  dnl Since compile_et seems to work, let's check libcom_err
  krb_cv_save_LIBS="${LIBS}"
  LIBS="${LIBS} -lcom_err"
  if test -d "/usr/include/et"; then
    CPPFLAGS="-I/usr/include/et ${CPPFLAGS}"
  fi
  AC_MSG_CHECKING(for com_err)
  AC_TRY_LINK([#include <com_err.h>],[
    const char *p;
    p = error_message(0);
  ],[krb_cv_com_err="yes"],[krb_cv_com_err="no"])
  AC_MSG_RESULT(${krb_cv_com_err})
  LIBS="${krb_cv_save_LIBS}"
else
  dnl Since compile_et doesn't work, forget about libcom_err
  krb_cv_com_err="no"
fi

dnl Only use the system's com_err if we found compile_et, libcom_err, and
dnl com_err.h.
if test "${krb_cv_com_err}" = "yes"; then
    DIR_com_err=""
    LIB_com_err="-lcom_err"
    LIB_com_err_a=""
    LIB_com_err_so=""
    AC_MSG_NOTICE(Using the already-installed com_err)
else
    COMPILE_ET="\$(top_builddir)/lib/com_err_compile_et"
    DIR_com_err="com_err"
    LIB_com_err="\$(top_builddir)/lib/com_err/libcom_err.la"
    LIB_com_err_a="\$(top_builddir)/lib/com_err/.libs/libcom_err.a"
    LIB_com_err_so="\$(top_builddir)/lib/com_err/.libs/libcom_err.so"
    AC_MSG_NOTICE(Using our own com_err)
fi
AC_SUBST(DIR_com_err)
AC_SUBST(LIB_com_err)
AC_SUBST(LIB_com_err_a)
AC_SUBST(LIB_com_err_so)

])
