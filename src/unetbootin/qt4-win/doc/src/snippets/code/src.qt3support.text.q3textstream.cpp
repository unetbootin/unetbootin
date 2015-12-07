//! [0]
    QString str;
    Q3TextStream ts( &str, IO_WriteOnly );
    ts << "pi = " << 3.14; // str == "pi = 3.14"
//! [0]


//! [1]
    QString str = "pi = 3.14";
    Q3TextStream ts( &str, IO_WriteOnly );
    ts <<  "2+2 = " << 2+2; // str == "2+2 = 414"
//! [1]


//! [2]
    QByteArray array;
    Q3TextStream ts( array, IO_WriteOnly );
    ts << "pi = " << 3.14 << '\0'; // array == "pi = 3.14"
//! [2]


//! [3]
    QByteArray array;
    QBuffer buf( array );
    buf.open( IO_WriteOnly );
    Q3TextStream ts( &buf );
    ts << "pi = " << 3.14 << '\0'; // array == "pi = 3.14"
    buf.close();
//! [3]


