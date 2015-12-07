TEMPLATE   = app

HEADERS   += remotecontrol.h
SOURCES   += main.cpp \
             remotecontrol.cpp
FORMS     += remotecontrol.ui
RESOURCES += remotecontrol.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/help/remotecontrol
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.png *.doc doc
sources.path = $$[QT_INSTALL_EXAMPLES]/help/remotecontrol
INSTALLS += target sources
