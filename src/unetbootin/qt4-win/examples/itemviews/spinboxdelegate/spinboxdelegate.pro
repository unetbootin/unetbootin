HEADERS     = delegate.h
SOURCES     = delegate.cpp \
              main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/itemviews/spinboxdelegate
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/spinboxdelegate
INSTALLS += target sources
