//! [0]
        QLinkedList<int> integerList;
        QLinkedList<QTime> timeList;
//! [0]


//! [1]
        QLinkedList<QString> list;
        list << "one" << "two" << "three";
        // list: ["one", "two", "three"]
//! [1]


//! [2]
        QLinkedList<QWidget *> list;
        ...
        while (!list.isEmpty())
            delete list.takeFirst();
//! [2]


//! [3]
        QLinkedList<QString> list;
        list.append("one");
        list.append("two");
        list.append("three");
        // list: ["one", "two", "three"]
//! [3]


//! [4]
        QLinkedList<QString> list;
        list.prepend("one");
        list.prepend("two");
        list.prepend("three");
        // list: ["three", "two", "one"]
//! [4]


//! [5]
        QList<QString> list;
        list << "sun" << "cloud" << "sun" << "rain";
        list.removeAll("sun");
        // list: ["cloud", "rain"]
//! [5]


//! [6]
        QList<QString> list;
        list << "sun" << "cloud" << "sun" << "rain";
        list.removeOne("sun");
        // list: ["cloud", "sun", "rain"]
//! [6]


//! [7]
        QLinkedList<QString> list;
        list.append("January");
        list.append("February");
        ...
        list.append("December");

        QLinkedList<QString>::iterator i;
        for (i = list.begin(); i != list.end(); ++i)
            cout << *i << endl;
//! [7]


//! [8]
        QLinkedList<QString> list;
        ...
        QLinkedList<QString>::iterator it = qFind(list.begin(),
                                                  list.end(), "Joel");
        if (it != list.end())
            cout << "Found Joel" << endl;
//! [8]


//! [9]
        QLinkedList<int>::iterator i;
        for (i = list.begin(); i != list.end(); ++i)
            *i += 2;
//! [9]


//! [10]
        QLinkedList<QString> list;
        ...
        QLinkedList<QString>::iterator i = list.begin();
        while (i != list.end()) {
            if ((*i).startsWith("_"))
                i = list.erase(i);
            else
                ++i;
        }
//! [10]


//! [11]
        QLinkedList<QString>::iterator i = list.begin();
        while (i != list.end()) {
            QLinkedList<QString>::iterator previous = i;
            ++i;
            if ((*previous).startsWith("_"))
                list.erase(previous);
        }
//! [11]


//! [12]
        // WRONG
        while (i != list.end()) {
            if ((*i).startsWith("_"))
                list.erase(i);
            ++i;
        }
//! [12]


//! [13]
        if (*it == "Hello")
            *it = "Bonjour";
//! [13]


//! [14]
        QLinkedList<QString> list;
        list.append("January");
        list.append("February");
        ...
        list.append("December");

        QLinkedList<QString>::const_iterator i;
        for (i = list.constBegin(); i != list.constEnd(); ++i)
            cout << *i << endl;
//! [14]


//! [15]
        QLinkedList<QString> list;
        ...
        QLinkedList<QString>::iterator it = qFind(list.constBegin(),
                                                  list.constEnd(), "Joel");
        if (it != list.constEnd())
            cout << "Found Joel" << endl;
//! [15]


//! [16]
        std::list<double> stdlist;
        list.push_back(1.2);
        list.push_back(0.5);
        list.push_back(3.14);

        QLinkedList<double> list = QLinkedList<double>::fromStdList(stdlist);
//! [16]


//! [17]
        QLinkedList<double> list;
        list << 1.2 << 0.5 << 3.14;

        std::list<double> stdlist = list.toStdList();
//! [17]


