AC_DEFUN([AX_OPENMP], [
AC_PREREQ(2.59) dnl for _AC_LANG_PREFIX

AC_CACHE_CHECK([for OpenMP flag of _AC_LANG compiler], ax_cv_[]_AC_LANG_ABBREV[]_openmp, [save[]_AC_LANG_PREFIX[]FLAGS=$[]_AC_LANG_PREFIX[]FLAGS
ax_cv_[]_AC_LANG_ABBREV[]_openmp=unknown
# Flags to try:  -fopenmp (gcc), -openmp (icc), -mp (SGI & PGI),
#                -xopenmp (Sun), -omp (Tru64), -qsmp=omp (AIX), none
ax_openmp_flags="-fopenmp -openmp -mp -xopenmp -omp -qsmp=omp none"
if test "x$OPENMP_[]_AC_LANG_PREFIX[]FLAGS" != x; then
  ax_openmp_flags="$OPENMP_[]_AC_LANG_PREFIX[]FLAGS $ax_openmp_flags"
fi
for ax_openmp_flag in $ax_openmp_flags; do
  case $ax_openmp_flag in
    none) []_AC_LANG_PREFIX[]FLAGS=$save[]_AC_LANG_PREFIX[] ;;
    *) []_AC_LANG_PREFIX[]FLAGS="$save[]_AC_LANG_PREFIX[]FLAGS $ax_openmp_flag" ;;
  esac
  AC_TRY_LINK_FUNC(omp_set_num_threads,
        [ax_cv_[]_AC_LANG_ABBREV[]_openmp=$ax_openmp_flag; break])
done
[]_AC_LANG_PREFIX[]FLAGS=$save[]_AC_LANG_PREFIX[]FLAGS
])
if test "x$ax_cv_[]_AC_LANG_ABBREV[]_openmp" = "xunknown"; then
  m4_default([$2],:)
else
  if test "x$ax_cv_[]_AC_LANG_ABBREV[]_openmp" != "xnone"; then
    OPENMP_[]_AC_LANG_PREFIX[]FLAGS=$ax_cv_[]_AC_LANG_ABBREV[]_openmp
  fi
  m4_default([$1], [AC_DEFINE(HAVE_OPENMP,1,[Define if OpenMP is enabled])])
fi
])dnl AX_OPENMP
