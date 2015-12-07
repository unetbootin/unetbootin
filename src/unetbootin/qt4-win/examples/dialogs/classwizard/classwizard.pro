HEADERS       = classwizard.h
SOURCES       = classwizard.cpp \
                main.cpp
RESOURCES     = classwizard.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/dialogs/classwizard
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/dialogs/classwizard
INSTALLS += target sources
