//! [0]
        QDBusReply<QString> reply = interface->call("RemoteMethod");
        if (reply.isValid())
            // use the returned value
            useValue(reply.value());
        else
            // call failed. Show an error condition.
            showError(reply.error());
//! [0]


//! [1]
        QString reply = interface->call("RemoteMethod");
//! [1]


