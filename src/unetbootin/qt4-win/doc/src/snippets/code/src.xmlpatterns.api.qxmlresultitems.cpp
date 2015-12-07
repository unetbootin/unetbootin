//! [0]
  QXmlQuery query;
  query.setQuery("<e/>, 1, 'two'");
  QXmlResultItems result;

  if (query.isValid()) {
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    while (!item.isNull()) {
        // use item
        item = result.next();
    }
    if (result.hasError())
        /* Runtime error! */;
  }
//! [0]


