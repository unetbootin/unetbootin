//! [0]
  if (extension == Callable) {
    QScriptContext *context = qvariant_cast<QScriptContext*>(argument);
    QScriptEngine *engine = context->engine();
    double sum = 0;
    for (int i = 0; i < context->argumentCount(); ++i)
        sum += context->argument(i).toNumber();
    return sum;
  }
//! [0]


