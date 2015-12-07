#
# Mac OS X architecture
#
!*-icc*:!*-g++* {
   contains($$list($$system(uname -m)), .*86):SOURCES += $$QT_ARCH_CPP/../i386/qatomic.s
   else:SOURCES += $$QT_ARCH_CPP/../powerpc/qatomic32.s
}
