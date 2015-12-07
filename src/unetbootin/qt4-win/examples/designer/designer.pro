TEMPLATE    = subdirs
SUBDIRS     = calculatorform

!static:SUBDIRS += calculatorbuilder \
                   containerextension \
                   customwidgetplugin \
                   taskmenuextension \
                   worldtimeclockbuilder \
                   worldtimeclockplugin

# the sun cc compiler has a problem with the include lines for the form.prf
solaris-cc*:SUBDIRS -= calculatorbuilder \
		       worldtimeclockbuilder
		     

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/designer
INSTALLS += sources
