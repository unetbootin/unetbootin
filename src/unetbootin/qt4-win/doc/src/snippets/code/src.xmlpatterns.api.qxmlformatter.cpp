//! [0]
   QXmlQuery query;
   query.setQuery("doc('index.html')/html/body/p[1]");

   QXmlFormatter formatter(query, myOutputDevice);
   formatter.setIndentationDepth(2);
   query.evaluateTo(&formatter);
//! [0]


