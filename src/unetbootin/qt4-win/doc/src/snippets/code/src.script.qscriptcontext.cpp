//! [0]
  foo(20.5, "hello", new Object())
//! [0]


//! [1]
  QScriptValue Person_prototype_fullName(QScriptContext *context, QScriptEngine *engine)
  {
    QScriptValue self = context->thisObject();
    QString result;
    result += self.property("firstName").toString();
    result += QLatin1String(" ");
    result += self.property("lastName").toString();
    return QScriptValue(engine, result);
  }
//! [1]


//! [2]
  QScriptValue myInclude(QScriptContext *ctx, QScriptEngine *eng)
  {
    QString fileName = ctx->argument(0).toString();
    QString contents = readTheFile(fileName);
    ctx->setActivationObject(ctx->parentContext()->activationObject());
    ctx->setThisObject(ctx->parentContext()->thisObject());
    return eng->evaluate(contents, fileName);
  }
//! [2]


