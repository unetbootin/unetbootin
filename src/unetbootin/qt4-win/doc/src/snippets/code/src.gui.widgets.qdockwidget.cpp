//! [0]
       QDockWidget *dockWidget = qobject_cast<QDockWidget*>(parentWidget());
       if (dockWidget->features() & QDockWidget::DockWidgetVerticalTitleBar) {
           // I need to be vertical
       } else {
           // I need to be horizontal
       }
//! [0]


