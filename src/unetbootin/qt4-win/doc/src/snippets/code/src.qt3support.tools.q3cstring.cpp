//! [0]
    Q3CString str("helloworld", 6); // assigns "hello" to str
//! [0]


//! [1]
    Q3CString a;                // a.data() == 0,  a.size() == 0, a.length() == 0
    Q3CString b == "";        // b.data() == "", b.size() == 1, b.length() == 0
    a.isNull();                // true  because a.data() == 0
    a.isEmpty();        // true  because a.length() == 0
    b.isNull();                // false because b.data() == ""
    b.isEmpty();        // true  because b.length() == 0
//! [1]


//! [2]
    Q3CString s = "truncate this string";
    s.truncate(5);                      // s == "trunc"
//! [2]


//! [3]
    Q3CString s;
    s.sprintf("%d - %s", 1, "first");                // result < 256 chars

    Q3CString big(25000);                        // very long string
    big.sprintf("%d - %s", 2, longString);        // result < 25000 chars
//! [3]


//! [4]
    Q3CString s("apple");
    Q3CString t = s.leftJustify(8, '.');  // t == "apple..."
//! [4]


//! [5]
    Q3CString s("pie");
    Q3CString t = s.rightJustify(8, '.');  // t == ".....pie"
//! [5]


