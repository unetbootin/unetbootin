//! [0]
 myInstance = QXmlNodeModelIndex();
//! [0]

//! [1]
  QFile queryFile(argv[1]);
  QFile chemistryData(argv[2]);
  QString moleculeName = argv[3];

  QXmlQuery query;
  query.setQuery(&queryFile, QUrl::fromLocalFile(queryFile.fileName()));

  ChemistryNodeModel myNodeModel(query.namePool(), chemistryData);
  QXmlNodeModelIndex startNode = myNodeModel.nodeFor(moleculeName);
  query.bindVariable("queryRoot", startNode);

  QFile out;
  out.open(stdout, QIODevice::WriteOnly);

  QXmlSerializer serializer(query, &out);
  query.evaluateTo(&serializer);
//! [1]
