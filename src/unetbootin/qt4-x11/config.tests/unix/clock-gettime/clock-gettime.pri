# clock_gettime() is implemented in librt on these systems
linux-*|hpux-*|solaris-*:LIBS *= -lrt
