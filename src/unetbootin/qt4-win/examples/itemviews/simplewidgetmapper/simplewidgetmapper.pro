HEADERS   = window.h
SOURCES   = main.cpp \
            window.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/itemviews/simplewidgetmapper
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/simplewidgetmapper
INSTALLS += target sources
