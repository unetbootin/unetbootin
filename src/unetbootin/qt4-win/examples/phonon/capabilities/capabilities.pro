QT      += phonon

HEADERS += window.h
SOURCES += window.cpp \
           main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/phonon/capabilities
sources.files = $$SOURCES $$HEADERS capabilities.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/phonon/capabilities
INSTALLS += target sources
