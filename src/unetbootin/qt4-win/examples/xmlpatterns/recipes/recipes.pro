QT          += xmlpatterns
FORMS       += forms/querywidget.ui
HEADERS     = querymainwindow.h
RESOURCES   = recipes.qrc
SOURCES     = main.cpp querymainwindow.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/recipes
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.xq *.html forms files
sources.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/recipes
INSTALLS += target sources
