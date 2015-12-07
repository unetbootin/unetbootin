QT += script
RESOURCES += defaultprototypes.qrc
SOURCES += main.cpp prototypes.cpp
HEADERS += prototypes.h

# install
target.path = $$[QT_INSTALL_EXAMPLES]/script/defaultprototypes
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.js defaultprototypes.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/script/defaultprototypes
INSTALLS += target sources
