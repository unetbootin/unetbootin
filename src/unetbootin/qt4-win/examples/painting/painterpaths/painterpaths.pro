HEADERS       = renderarea.h \
                window.h
SOURCES       = main.cpp \
                renderarea.cpp \
                window.cpp
unix:!mac:LIBS += -lm

# install
target.path = $$[QT_INSTALL_EXAMPLES]/painting/painterpaths
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS painterpaths.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/painting/painterpaths
INSTALLS += target sources
