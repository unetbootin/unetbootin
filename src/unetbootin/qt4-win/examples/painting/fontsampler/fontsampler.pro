FORMS     = previewdialogbase.ui \
            mainwindowbase.ui
HEADERS   = mainwindow.h \
            previewdialog.h \
            previewlabel.h
SOURCES   = main.cpp \
            mainwindow.cpp \
            previewdialog.cpp \
            previewlabel.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/painting/fontsampler
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS fontsampler.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/painting/fontsampler
INSTALLS += target sources
