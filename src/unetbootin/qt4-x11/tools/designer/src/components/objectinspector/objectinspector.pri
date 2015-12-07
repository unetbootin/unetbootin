include($$QT_SOURCE_TREE/tools/shared/findwidget/findwidget.pri)

INCLUDEPATH += $$PWD

HEADERS += $$PWD/objectinspector.h \
    $$PWD/objectinspectormodel_p.h \
    $$PWD/objectinspector_global.h

SOURCES += $$PWD/objectinspector.cpp \
    $$PWD/objectinspectormodel.cpp
