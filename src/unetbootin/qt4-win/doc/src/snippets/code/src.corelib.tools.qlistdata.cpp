//! [0]
        QList<int> integerList;
        QList<QDate> dateList;
//! [0]


//! [1]
        QList<QString> list;
        list << "one" << "two" << "three";
        // list: ["one", "two", "three"]
//! [1]


//! [2]
        if (list[0] == "Bob")
            list[0] = "Robert";
//! [2]


//! [3]
        for (int i = 0; i < list.size(); ++i) {
            if (list.at(i) == "Jane")
                cout << "Found Jane at position " << i << endl;
        }
//! [3]


//! [4]
        QList<QWidget *> list;
        ...
        while (!list.isEmpty())
            delete list.takeFirst();
//! [4]


//! [5]
        int i = list.indexOf("Jane");
        if (i != -1)
            cout << "First occurrence of Jane is at position " << i << endl;
//! [5]


//! [6]
        QList<QString> list;
        list.append("one");
        list.append("two");
        list.append("three");
        // list: ["one", "two", "three"]
//! [6]


//! [7]
        QList<QString> list;
        list.prepend("one");
        list.prepend("two");
        list.prepend("three");
        // list: ["three", "two", "one"]
//! [7]


//! [8]
        QList<QString> list;
        list << "alpha" << "beta" << "delta";
        list.insert(2, "gamma");
        // list: ["alpha", "beta", "gamma", "delta"]
//! [8]


//! [9]
        QList<QString> list;
        list << "sun" << "cloud" << "sun" << "rain";
        list.removeAll("sun");
        // list: ["cloud", "rain"]
//! [9]


//! [10]
        QList<QString> list;
        list << "sun" << "cloud" << "sun" << "rain";
        list.removeOne("sun");
        // list: ["cloud", ,"sun", "rain"]
//! [10]


//! [11]
        QList<QString> list;
        list << "A" << "B" << "C" << "D" << "E" << "F";
        list.move(1, 4);
        // list: ["A", "C", "D", "E", "B", "F"]
//! [11]


//! [12]
        QList<QString> list;
        list << "A" << "B" << "C" << "D" << "E" << "F";
        list.swap(1, 4);
        // list: ["A", "E", "C", "D", "B", "F"]
//! [12]


//! [13]
        QList<QString> list;
        list << "A" << "B" << "C" << "B" << "A";
        list.indexOf("B");          // returns 1
        list.indexOf("B", 1);       // returns 1
        list.indexOf("B", 2);       // returns 3
        list.indexOf("X");          // returns -1
//! [13]


//! [14]
        QList<QString> list;
        list << "A" << "B" << "C" << "B" << "A";
        list.lastIndexOf("B");      // returns 3
        list.lastIndexOf("B", 3);   // returns 3
        list.lastIndexOf("B", 2);   // returns 1
        list.lastIndexOf("X");      // returns -1
//! [14]


//! [15]
        QList<QString> list;
        list.append("January");
        list.append("February");
        ...
        list.append("December");

        QList<QString>::iterator i;
        for (i = list.begin(); i != list.end(); ++i)
            cout << *i << endl;
//! [15]


//! [16]
        QList<int>::iterator i;
        for (i = list.begin(); i != list.end(); ++i)
            *i += 2;
//! [16]


//! [17]
        QList<QWidget *> list;
        ...
        qDeleteAll(list.begin(), list.end());
//! [17]


//! [18]
        if (*it == "Hello")
            *it = "Bonjour";
//! [18]


//! [19]
        QList<QString> list;
        list.append("January");
        list.append("February");
        ...
        list.append("December");

        QList<QString>::const_iterator i;
        for (i = list.constBegin(); i != list.constEnd(); ++i)
            cout << *i << endl;
//! [19]


//! [20]
        QList<QWidget *> list;
        ...
        qDeleteAll(list.constBegin(), list.constEnd());
//! [20]


//! [21]
        QVector<double> vect;
        vect << "red" << "green" << "blue" << "black";

        QList<double> list = QVector<T>::fromVector(vect);
        // list: ["red", "green", "blue", "black"]
//! [21]


//! [22]
        QStringList list;
        list << "Sven" << "Kim" << "Ola";

        QVector<QString> vect = list.toVector();
        // vect: ["Sven", "Kim", "Ola"]
//! [22]


//! [23]
        QSet<double> set;
        set << "red" << "green" << "blue" << ... << "black";

        QList<double> list = QList<double>::fromSet(set);
        qSort(list);
//! [23]


//! [24]
        QStringList list;
        list << "Julia" << "Mike" << "Mike" << "Julia" << "Julia";

        QSet<QString> set = list.toSet();
        set.contains("Julia");  // returns true
        set.contains("Mike");   // returns true
        set.size();             // returns 2
//! [24]


//! [25]
        std::list<double> stdlist;
        list.push_back(1.2);
        list.push_back(0.5);
        list.push_back(3.14);

        QList<double> list = QList<double>::fromStdList(stdlist);
//! [25]


//! [26]
        QList<double> list;
        list << 1.2 << 0.5 << 3.14;

        std::list<double> stdlist = list.toStdList();
//! [26]


