//! [0]
        struct MyStruct
        {
            int i;
            ...
        };

        Q_DECLARE_METATYPE(MyStruct)
//! [0]


//! [1]
        namespace MyNamespace
        {
            ...
        }

        Q_DECLARE_METATYPE(MyNamespace::MyStruct)
//! [1]


//! [2]
        MyStruct s;
        QVariant var;
        var.setValue(s); // copy s into the variant

        ...

        // retrieve the value
        MyStruct s2 = var.value<MyStruct>();
//! [2]


//! [3]
        int id = QMetaType::type("MyClass");
        if (id != -1) {
            void *myClassPtr = QMetaType::construct(id);
            ...
            QMetaType::destroy(id, myClassPtr);
            myClassPtr = 0;
        }
//! [3]


//! [4]
        qRegisterMetaType<MyClass>("MyClass");
//! [4]


//! [5]
        qRegisterMetaTypeStreamOperators<MyClass>("MyClass");
//! [5]


//! [6]
        QDataStream &operator<<(QDataStream &out, const MyClass &myObj);
        QDataStream &operator>>(QDataStream &in, MyClass &myObj);
//! [6]


//! [7]
        int id = qRegisterMetaType<MyStruct>();
//! [7]


//! [8]
        int id = qMetaTypeId<QString>();    // id is now QMetaType::QString
        id = qMetaTypeId<MyStruct>();       // compile error if MyStruct not declared
//! [8]


