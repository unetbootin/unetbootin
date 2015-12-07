TEMPLATE = app

# Input
HEADERS += stylewidget.h
FORMS += stylewidget.ui
SOURCES += main.cpp stylewidget.cpp
RESOURCES += styledemo.qrc

target.path = $$[QT_INSTALL_DEMOS]/embedded/styledemo
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.html
sources.path = $$[QT_INSTALL_DEMOS]/embedded/styledemo
INSTALLS += target sources
