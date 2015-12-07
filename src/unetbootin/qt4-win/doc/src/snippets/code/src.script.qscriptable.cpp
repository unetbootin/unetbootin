//! [0]
  class MyScriptableObject: public QObject,
        protected QScriptable
  {
    Q_OBJECT
  ...

  public slots:
    void doSomething();
    double doSomethingElse();
  }
//! [0]


//! [1]
    void MyScriptableObject::doSomething()
    {
      context()->throwError("Threw an error from a slot");
    }

    double MyScriptableObject::doSomethingElse()
    {
      return qscriptvalue_cast<double>(thisObject());
    }
//! [1]


