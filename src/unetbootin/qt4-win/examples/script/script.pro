TEMPLATE = subdirs
SUBDIRS = helloscript context2d defaultprototypes customclass

!wince*:SUBDIRS += qscript marshal
!wince*:!cross_compile:SUBDIRS += calculator tetrix

# install
target.path = $$[QT_INSTALL_EXAMPLES]/script
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS script.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/script
INSTALLS += target sources
