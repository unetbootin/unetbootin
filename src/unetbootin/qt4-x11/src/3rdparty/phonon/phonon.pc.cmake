prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@EXEC_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@INCLUDE_INSTALL_DIR@

Name: Phonon
Description: Phonon library needed to build applications
Version: @PHONON_LIB_MAJOR_VERSION@.@PHONON_LIB_MINOR_VERSION@.@PHONON_LIB_PATCH_VERSION@
Requires: QtCore QtGui QtDBus
Libs: -L${libdir} -lphonon
Cflags: -I${includedir}
