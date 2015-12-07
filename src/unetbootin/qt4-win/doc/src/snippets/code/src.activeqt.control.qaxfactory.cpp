//! [0]
        QStringList ActiveQtFactory::featureList() const
        {
            QStringList list;
            list << "ActiveX1";
            list << "ActiveX2";
            return list;
        }

        QObject *ActiveQtFactory::createObject(const QString &key)
        {
            if (key == "ActiveX1")
                return new ActiveX1(parent);
            if (key == "ActiveX2")
                return new ActiveX2(parent);
            return 0;
        }

        const QMetaObject *ActiveQtFactory::metaObject(const QString &key) const
        {
            if (key == "ActiveX1")
                return &ActiveX1::staticMetaObject;
            if (key == "ActiveX2")
                return &ActiveX2::staticMetaObject;
        }

        QUuid ActiveQtFactory::classID(const QString &key) const
        {
            if (key == "ActiveX1")
                return "{01234567-89AB-CDEF-0123-456789ABCDEF}";
            ...
            return QUuid();
        }

        QUuid ActiveQtFactory::interfaceID(const QString &key) const
        {
            if (key == "ActiveX1")
                return "{01234567-89AB-CDEF-0123-456789ABCDEF}";
            ...
            return QUuid();
        }

        QUuid ActiveQtFactory::eventsID(const QString &key) const
        {
            if (key == "ActiveX1")
                return "{01234567-89AB-CDEF-0123-456789ABCDEF}";
            ...
            return QUuid();
        }

        QAXFACTORY_EXPORT(
            ActiveQtFactory,			      // factory class
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
        )
//! [0]


//! [1]
        QAXFACTORY_BEGIN(
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
        )
            QAXCLASS(Class1)
            QAXCLASS(Class2)
        QAXFACTORY_END()
//! [1]


//! [2]
        #include <qapplication.h>
        #include <qaxfactory.h>

        #include "theactivex.h"

        QAXFACTORY_DEFAULT(
            TheActiveX,				  // widget class
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // class ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // interface ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // event interface ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
        )
//! [2]


//! [3]
        settings->setValue("/CLSID/" + classID(key)
                           + "/Implemented Categories/"
                           + "/{00000000-0000-0000-000000000000}/.",
                           QString());
//! [3]


//! [4]
        settings->remove("/CLSID/" + classID(key)
                         + "/Implemented Categories"
                         + "/{00000000-0000-0000-000000000000}/.");
//! [4]


//! [5]
        int main(int argc, char *argv[])
        {
            QApplication app(argc, argv);
            if (!QAxFactory::isServer()) {
                // initialize for stand-alone execution
            }
            return app.exec();
        }
//! [5]


//! [6]
    if (QAxFactory::isServer()) {
        QAxFactory::stopServer();
        QAxFactory::startServer(QAxFactory::SingleInstance);
    }
//! [6]


//! [7]
        #include <qaxfactory.h>

        #include "theactivex.h"

        QAXFACTORY_DEFAULT(
            TheActiveX,				  // widget class
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // class ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // interface ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // event interface ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
        )
//! [7]


//! [8]
        QAXFACTORY_EXPORT(
            MyFactory,			              // factory class
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
        )
//! [8]


//! [9]
        QAXFACTORY_BEGIN(
            "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
            "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
        )
            QAXCLASS(Class1)
            QAXCLASS(Class2)
        QAXFACTORY_END()
//! [9]


