//! [0]
      QString value = retrieveValue();
      QDBusMessage reply;

      QDBusReply<int> api = interface->call(QLatin1String("GetAPIVersion"));
      if (api >= 14)
        reply = interface->call(QLatin1String("ProcessWorkUnicode"), value);
      else
        reply = interface->call(QLatin1String("ProcessWork"), QLatin1String("UTF-8"), value.toUtf8());
//! [0]


