HEADERS       = ftpwindow.h
SOURCES       = ftpwindow.cpp \
                main.cpp
RESOURCES    += ftp.qrc
QT           += network

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/ftp
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS ftp.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/network/ftp
INSTALLS += target sources
