# We don't have any C++ files to build, so in order to trick qmake which
# doesn't understand that, we use the subdirs template, but without specifying
# any subdirs.
TEMPLATE = subdirs

target.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/xquery/globalVariables
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.cpp *.pro *.xq *.html globals.gccxml
sources.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/xquery/globalVariables
INSTALLS += target sources
