//! [0]
        label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
//! [0]


//! [1]
        class MyClass
        {
        public:
            enum Option {
                NoOptions = 0x0,
                ShowTabs = 0x1,
                ShowAll = 0x2,
                SqueezeBlank = 0x4
            };
            Q_DECLARE_FLAGS(Options, Option)
            ...
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(MyClass::Options)
//! [1]


//! [2]
        typedef QFlags<Enum> Flags;
//! [2]


//! [3]
        int myValue = 10;
        int minValue = 2;
        int maxValue = 6;

        int boundedValue = qBound(minValue, myValue, maxValue);
        // boundedValue == 6
//! [3]


//! [4]
        if (!driver()->isOpen() || driver()->isOpenError()) {
            qWarning("QSqlQuery::exec: database not open");
            return false;
        }
//! [4]


//! [5]
        qint64 value = Q_INT64_C(932838457459459);
//! [5]


//! [6]
        quint64 value = Q_UINT64_C(932838457459459);
//! [6]


//! [7]
        void myMsgHandler(QtMsgType, const char *);
//! [7]


//! [8]
        qint64 value = Q_INT64_C(932838457459459);
//! [8]


//! [9]
        quint64 value = Q_UINT64_C(932838457459459);
//! [9]


//! [10]
        int absoluteValue;
        int myValue = -4;

        absoluteValue = qAbs(myValue);
        // absoluteValue == 4
//! [10]


//! [11]
        qreal valueA = 2.3;
        qreal valueB = 2.7;

        int roundedValueA = qRound(valueA);
        \\ roundedValueA = 2
        int roundedValueB = qRound(valueB);
        \\ roundedValueB = 3
//! [11]


//! [12]
        qreal valueA = 42949672960.3;
        qreal valueB = 42949672960.7;

        int roundedValueA = qRound(valueA);
        \\ roundedValueA = 42949672960
        int roundedValueB = qRound(valueB);
        \\ roundedValueB = 42949672961
//! [12]


//! [13]
        int myValue = 6;
        int yourValue = 4;

        int minValue = qMin(myValue, yourValue);
        // minValue == yourValue
//! [13]


//! [14]
        int myValue = 6;
        int yourValue = 4;

        int maxValue = qMax(myValue, yourValue);
        // maxValue == myValue
//! [14]


//! [15]
        int myValue = 10;
        int minValue = 2;
        int maxValue = 6;

        int boundedValue = qBound(minValue, myValue, maxValue);
        // boundedValue == 6
//! [15]


//! [16]
        #if QT_VERSION >= 0x040100
            QIcon icon = style()->standardIcon(QStyle::SP_TrashIcon);
        #else
            QPixmap pixmap = style()->standardPixmap(QStyle::SP_TrashIcon);
            QIcon icon(pixmap);
        #endif
//! [16]


//! [17]
        // File: div.cpp

        #include <QtGlobal>

        int divide(int a, int b)
        {
            Q_ASSERT(b != 0);
            return a / b;
        }
//! [17]


//! [18]
        ASSERT: "b == 0" in file div.cpp, line 7
//! [18]


//! [19]
        // File: div.cpp

        #include <QtGlobal>

        int divide(int a, int b)
        {
            Q_ASSERT_X(b != 0, "divide", "division by zero");
            return a / b;
        }
//! [19]


//! [20]
        ASSERT failure in divide: "division by zero", file div.cpp, line 7
//! [20]


//! [21]
        int *a;

        Q_CHECK_PTR(a = new int[80]);   // WRONG!

        a = new (nothrow) int[80];      // Right
        Q_CHECK_PTR(a);
//! [21]


//! [22]
        template<typename TInputType>
        const TInputType &myMin(const TInputType &value1, const TInputType &value2)
        {
            qDebug() << Q_FUNC_INFO << "was called with value1:" << value1 << "value2:" << value2;

            if(value1 < value2)
                return value1;
            else
                return value2;
        }
//! [22]


//! [23]
        #include <qapplication.h>
        #include <stdio.h>
        #include <stdlib.h>

        void myMessageOutput(QtMsgType type, const char *msg)
        {
            switch (type) {
            case QtDebugMsg:
                fprintf(stderr, "Debug: %s\n", msg);
                break;
            case QtWarningMsg:
                fprintf(stderr, "Warning: %s\n", msg);
                break;
            case QtCriticalMsg:
                fprintf(stderr, "Critical: %s\n", msg);
                break;
            case QtFatalMsg:
                fprintf(stderr, "Fatal: %s\n", msg);
                abort();
            }
        }

        int main(int argc, char **argv)
        {
            qInstallMsgHandler(myMessageOutput);
            QApplication app(argc, argv);
            ...
            return app.exec();
        }
//! [23]


//! [24]
        qDebug("Items in list: %d", myList.size());
//! [24]


//! [25]
        qDebug() << "Brush:" << myQBrush << "Other value:" << i;
//! [25]


//! [26]
        void f(int c)
        {
            if (c > 200)
                qWarning("f: bad argument, c == %d", c);
        }
//! [26]


//! [27]
       qWarning() << "Brush:" << myQBrush << "Other value:"
       << i;
//! [27]


//! [28]
        void load(const QString &fileName)
        {
            QFile file(fileName);
            if (!file.exists())
                qCritical("File '%s' does not exist!", qPrintable(fileName));
        }
//! [28]


//! [29]
       qCritical() << "Brush:" << myQBrush << "Other
       value:" << i;
//! [29]


//! [30]
        int divide(int a, int b)
        {
            if (b == 0)                                // program error
                qFatal("divide: cannot divide by zero");
            return a / b;
        }
//! [30]


//! [31]
        forever {
            ...
        }
//! [31]


//! [32]
        CONFIG += no_keywords
//! [32]


//! [33]
        CONFIG += no_keywords
//! [33]


//! [34]
        QString FriendlyConversation::greeting(int type)
        {
        static const char *greeting_strings[] = {
            QT_TR_NOOP("Hello"),
            QT_TR_NOOP("Goodbye")
        };
        return tr(greeting_strings[type]);
        }
//! [34]


//! [35]
        static const char *greeting_strings[] = {
            QT_TRANSLATE_NOOP("FriendlyConversation", "Hello"),
            QT_TRANSLATE_NOOP("FriendlyConversation", "Goodbye")
        };

        QString FriendlyConversation::greeting(int type)
        {
            return tr(greeting_strings[type]);
        }

        QString global_greeting(int type)
        {
            return qApp->translate("FriendlyConversation",
                   greeting_strings[type]);
        }
//! [35]


//! [36]

        static { const char *source; const char *comment; } greeting_strings[] =
        {
            QT_TRANSLATE_NOOP3("FriendlyConversation", "Hello",
                               "A really friendly hello"),
            QT_TRANSLATE_NOOP3("FriendlyConversation", "Goodbye",
                               "A really friendly goodbye")
        };

        QString FriendlyConversation::greeting(int type)
        {
            return tr(greeting_strings[type].source,
                      greeting_strings[type].comment);
        }

        QString global_greeting(int type)
        {
            return qApp->translate("FriendlyConversation",
                   greeting_strings[type].source,
                   greeting_strings[type].comment);
        }
//! [36]


//! [37]
        qWarning("%s: %s", qPrintable(key), qPrintable(value));
//! [37]


//! [38]
        struct Point2D
        {
            int x;
            int y;
        };

        Q_DECLARE_TYPEINFO(Point2D, Q_PRIMITIVE_TYPE);
//! [38]


//! [39]
        class Point2D
        {
        public:
            Point2D() { data = new int[2]; }
            Point2D(const Point2D &other) { ... }
            ~Point2D() { delete[] data; }

            Point2D &operator=(const Point2D &other) { ... }

            int x() const { return data[0]; }
            int y() const { return data[1]; }

        private:
            int *data;
        };

        Q_DECLARE_TYPEINFO(Point2D, Q_MOVABLE_TYPE);
//! [39]


//! [40]
    #if Q_BYTE_ORDER == Q_BIG_ENDIAN
    ...
    #endif

    or

    #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    ...
    #endif

//! [40]


//! [41]

    #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    ...
    #endif

//! [41]


//! [42]
    #if Q_BYTE_ORDER == Q_BIG_ENDIAN
    ...
    #endif

//! [42]

//! [begin namespace macro]
namespace QT_NAMESPACE {
//! [begin namespace macro]

//! [end namespace macro]
}
//! [end namespace macro]
