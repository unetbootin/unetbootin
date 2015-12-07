HEADERS     = domitem.h \
              dommodel.h \
              mainwindow.h
SOURCES     = domitem.cpp \
              dommodel.cpp \
              main.cpp \
              mainwindow.cpp
CONFIG  += qt
QT      += xml

# install
target.path = $$[QT_INSTALL_EXAMPLES]/itemviews/simpledommodel
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/simpledommodel
INSTALLS += target sources
