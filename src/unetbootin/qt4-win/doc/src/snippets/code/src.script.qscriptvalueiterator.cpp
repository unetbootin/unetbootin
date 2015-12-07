//! [0]
  QScriptValue object;
  ...
  QScriptValueIterator it(object);
  while (it.hasNext()) {
    it.next();
    qDebug() << it.name() << ": " << it.value().toString();
  }
//! [0]


//! [1]
  QScriptValue obj = ...; // the object to iterate over
  while (obj.isObject()) {
    QScriptValueIterator it(obj);
    while (it.hasNext()) {
      it.next();
      qDebug() << it.name();
    }
    obj = obj.prototype();
  }
//! [1]


//! [2]
  while (it.hasNext()) {
    it.next();
    if (it.flags() & QScriptValue::SkipInEnumeration)
        continue;
    qDebug() << "found enumerated property:" << it.name();
  }
//! [2]


