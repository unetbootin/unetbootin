QT          += opengl

HEADERS     = bubble.h \
              glwidget.h
SOURCES     = bubble.cpp \
              glwidget.cpp \
              main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/overpainting
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS overpainting.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/opengl/overpainting
INSTALLS += target sources
