//! [0]
        QObject *ANewExtensionFactory::createExtension(QObject *object,
                const QString &iid, QObject *parent) const
        {
            if (iid != Q_TYPEID(QDesignerContainerExtension))
                return 0;

            if (MyCustomWidget *widget = qobject_cast<MyCustomWidget*>
                   (object))
                return new MyContainerExtension(widget, parent);

            return 0;
        }
//! [0]


//! [1]
        QObject *AGeneralExtensionFactory::createExtension(QObject *object,
                const QString &iid, QObject *parent) const
        {
            MyCustomWidget *widget = qobject_cast<MyCustomWidget*>(object);

            if (widget && (iid == Q_TYPEID(QDesignerTaskMenuExtension))) {
                return new MyTaskMenuExtension(widget, parent);

            } else if (widget && (iid == Q_TYPEID(QDesignerContainerExtension))) {
                return new MyContainerExtension(widget, parent);

            } else {
                return 0;
            }
        }
//! [1]


