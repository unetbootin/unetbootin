SOURCES = testqstring.cpp
CONFIG  += qtestlib

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial1
sources.files = $$SOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial1
INSTALLS += target sources
