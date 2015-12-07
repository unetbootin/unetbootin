TEMPLATE      = subdirs
SUBDIRS       = cachedtable \
                drilldown \
                relationaltablemodel

!wince*: SUBDIRS += querymodel tablemodel masterdetail

# install
sources.files = connection.h sql.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/sql
INSTALLS += sources
