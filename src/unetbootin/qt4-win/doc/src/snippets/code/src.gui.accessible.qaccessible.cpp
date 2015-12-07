//! [0]
        QAccessibleInterface *child = 0;
        int targetChild = object->navigate(Accessible::Child, 1, &child);
        if (child) {
            // ...
            delete child;
        }
//! [0]


