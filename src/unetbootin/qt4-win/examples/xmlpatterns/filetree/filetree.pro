SOURCES += main.cpp filetree.cpp mainwindow.cpp
HEADERS += filetree.h  mainwindow.h
FORMS += forms/mainwindow.ui
QT += xmlpatterns
CONFIG -= app_bundle
RESOURCES += queries.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/filetree
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.xq *.html
sources.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/filetree
INSTALLS += target sources
