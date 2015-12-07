TEMPLATE = subdirs
SUBDIRS  = t1 t2 t3 t4 t5 t6 t7 t8 t9 t10 t11 t12 t13 t14

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS tutorial.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial
INSTALLS += target sources
