TEMPLATE      = subdirs
SUBDIRS       = multipleinheritance

!wince*:contains(QT_BUILD_PARTS, tools): SUBDIRS += textfinder

# install
target.path = $$[QT_INSTALL_EXAMPLES]/uitools
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS uitools.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/uitools
INSTALLS += target sources
