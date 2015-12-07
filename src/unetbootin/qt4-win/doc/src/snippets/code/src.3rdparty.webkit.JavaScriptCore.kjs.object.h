//! [0]

     *   class BarImp : public JSObject {
     *     virtual const ClassInfo *classInfo() const { return &info; }
     *     static const ClassInfo info;
     *     // ...
     *   };
     *
     *   class FooImp : public JSObject {
     *     virtual const ClassInfo *classInfo() const { return &info; }
     *     static const ClassInfo info;
     *     // ...
     *   };
     * //! [0]


//! [1]

     *   const ClassInfo BarImp::info = {"Bar", 0, 0, 0}; // no parent class
     *   const ClassInfo FooImp::info = {"Foo", &BarImp::info, 0, 0};
     * //! [1]


