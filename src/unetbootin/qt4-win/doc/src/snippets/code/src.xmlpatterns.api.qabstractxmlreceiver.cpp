//! [0]
  QXmlQuery query;
  query.setQuery("doc('index.html')/html/body/p[1]");

  QXmlSerializer serializer(query, myOutputDevice);
  query.evaluateTo(&serializer);
//! [0]


