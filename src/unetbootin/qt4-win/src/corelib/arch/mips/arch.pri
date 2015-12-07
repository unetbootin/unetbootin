#
# MIPS 3/4 architecture
#
*-64:SOURCES += $$QT_ARCH_CPP/qatomic64.s
else:SOURCES += $$QT_ARCH_CPP/qatomic32.s
