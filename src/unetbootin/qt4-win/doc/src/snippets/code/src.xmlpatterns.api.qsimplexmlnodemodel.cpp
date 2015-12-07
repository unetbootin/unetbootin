//! [0]
  QXmlNodeModelIndex MyTreeModel::nextFromSimpleAxis(SimpleAxis axis, const QXmlNodeModelIndex &ni) const
  {
    // Convert the QXmlNodeModelIndex to a value that is specific to what we represent.
    const MyValue value = toMyValue(ni);

    switch(axis)
    {
        case Parent:
            return toNodeIndex(value.parent());
        case FirstChild:
        case PreviousSibling:
        case NextSibling:
            // and so on
            ;
    }
    return QXmlNodeModelIndex();
  }
//! [0]
