HEADERS       = window.h
SOURCES       = window.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/groupbox
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS groupbox.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/groupbox
INSTALLS += target sources
