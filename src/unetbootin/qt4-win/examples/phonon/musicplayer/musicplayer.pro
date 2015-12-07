QT        += phonon

HEADERS   += mainwindow.h
SOURCES   += main.cpp \
             mainwindow.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/phonon/musicplayer
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png images
sources.path = $$[QT_INSTALL_EXAMPLES]/phonon/musicplayer
INSTALLS += target sources
