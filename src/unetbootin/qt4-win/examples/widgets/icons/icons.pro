HEADERS       = iconpreviewarea.h \
                iconsizespinbox.h \
                imagedelegate.h \
                mainwindow.h
SOURCES       = iconpreviewarea.cpp \
                iconsizespinbox.cpp \
                imagedelegate.cpp \
                main.cpp \
                mainwindow.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/icons
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS icons.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/icons
INSTALLS += target sources

wince*: {
    imageFiles.sources = images/*
    wincewm*: {
        imageFiles.path = "/My Documents/My Pictures"
    } else {
        imageFiles.path    = images
    }
    DEPLOYMENT += imageFiles
}
