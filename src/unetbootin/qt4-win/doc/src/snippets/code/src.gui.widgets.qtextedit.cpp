//! [0]
  void MyTextEdit::contextMenuEvent(QContextMenuEvent *event)
  {
      QMenu *menu = createStandardContextMenu();
      menu->addAction(tr("My Menu Item"));
      //...
      menu->exec(event->globalPos());
      delete menu;
  }
//! [0]


//! [1]
    edit->textCursor().insertText(text);
//! [1]


//! [2]
    edit->textCursor().insertHtml(fragment);
//! [2]


