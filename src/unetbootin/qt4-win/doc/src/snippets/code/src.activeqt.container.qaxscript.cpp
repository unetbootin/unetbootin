//! [0]
    function setNumber(number)
    {
        n = number;
    }
//! [0]


//! [1]
        QValueList args;
        args << 5;
        script->call("setNumber(const QVariant&)", args);
//! [1]


//! [2]
        script->call("setNumber(5)");
//! [2]


