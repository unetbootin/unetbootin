HEADERS     = window.h
SOURCES     = main.cpp \
              window.cpp
CONFIG     += qt

# install
target.path = $$[QT_INSTALL_EXAMPLES]/itemviews/basicsortfiltermodel
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/basicsortfiltermodel
INSTALLS += target sources
