//! [0]
        MyClass &MyClass:operator=(const MyClass &other)
        { qAtomicAssign(d, other.d); return *this; }
//! [0]


//! [1]
        void MyClass::detach()
        { qAtomicDetach(d); }
//! [1]


