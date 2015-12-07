//! [0]
        struct MyStructure
        {
            int count;
            QString name;
        };
        QT_DECLARE_METATYPE(MyStructure)

        // Marshall the MyStructure data into a D-BUS argument
        QDBusArgument &operator<<(QDBusArgument &argument, const MyStructure &mystruct)
        {
            argument.beginStructure();
            argument << mystruct.count << mystruct.name;
            argument.endStructure();
            return argument;
        }

        // Retrieve the MyStructure data from the D-BUS argument
        const QDBusArgument &operator>>(const QDBusArgument &argument, MyStructure &mystruct)
        {
            argument.beginStructure();
            argument >> mystruct.count >> mystruct.name;
            argument.endStructure();
            return argument;
        }
//! [0]


//! [1]
        qDBusRegisterMetaType<MyStructure>();
//! [1]


//! [2]
        MyType item = qdbus_cast<Type>(argument);
//! [2]


//! [3]
        MyType item;
        argument >> item;
//! [3]


//! [4]
        QDBusArgument &operator<<(QDBusArgument &argument, const MyStructure &mystruct)
        {
            argument.beginStructure();
            argument << mystruct.member1 << mystruct.member2 << ... ;
            argument.endStructure();
            return argument;
        }
//! [4]


//! [5]
        QDBusArgument &operator<<(QDBusArgument &argument, const MyStructure &mystruct)
        {
            argument.beginStructure();
            argument << mystruct.member1 << mystruct.member2;

            argument.beginStructure();
            argument << mystruct.member3.subMember1 << mystruct.member3.subMember2;
            argument.endStructure();

            argument << mystruct.member4;
            argument.endStructure();
            return argument;
        }
//! [5]


//! [6]
        // append an array of MyElement types
        QDBusArgument &operator<<(QDBusArgument &argument, const MyArray &myarray)
        {
            argument.beginArray( qMetaTypeId<MyElement>() );
            for ( int i = 0; i < myarray.length; ++i )
                argument << myarray.elements[i];
            argument.endArray();
            return argument;
        }
//! [6]


//! [7]
        // append a dictionary that associates ints to MyValue types
        QDBusArgument &operator<<(QDBusArgument &argument, const MyDictionary &mydict)
        {
            argument.beginMap( QVariant::Int, qMetaTypeId<MyValue>() );
            for ( int i = 0; i < mydict.length; ++i ) {
                argument.beginMapEntry();
                argument << mydict.data[i].key << mydict.data[i].value;
                argument.endMapEntry();
            }
            argument.endMap();
            return argument;
        }
//! [7]


//! [8]
        const QDBusArgument &operator>>(const QDBusArgument &argument, MyStructure &mystruct)
        {
            argument.beginStructure()
            argument >> mystruct.member1 >> mystruct.member2 >> mystruct.member3 >> ...;
            argument.endStructure();
            return argument;
        }
//! [8]


//! [9]
        // extract a MyArray array of MyElement elements
        const QDBusArgument &operator>>(const QDBusArgument &argument, MyArray &myarray)
        {
            argument.beginArray();
            myarray.clear();

            while ( !argument.atEnd() ) {
                MyElement element;
                argument >> element;
                myarray.append( element );
            }

            argument.endArray();
            return argument;
        }
//! [9]


//! [10]
        // extract a MyDictionary map that associates ints to MyValue elements
        const QDBusArgument &operator>>(const QDBusArgument &argument, MyDictionary &mydict)
        {
            argument.beginMap();
            mydict.clear();

            while ( !argMap.atEnd() ) {
                int key;
                MyValue value;
                argument.beginMapEntry();
                argument >> key >> value;
                argument.endMapEntry();
                mydict.append( key, value );
            }

            argument.endMap();
            return argument;
        }
//! [10]


