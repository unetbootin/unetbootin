//! [0]
  QScriptEngine myEngine;
  QScriptValue myObject = myEngine.newObject();
  QScriptValue myOtherObject = myEngine.newObject();
  myObject.setProperty("myChild", myOtherObject);
  myObject.setProperty("name", QScriptValue(&myEngine, "John Doe"));
//! [0]


//! [1]
  QScriptValue val(&myEngine, 123);
  myObject.setProperty("myReadOnlyProperty", val, QScriptValue::ReadOnly);
//! [1]


//! [2]
  QScriptEngine engine;
  engine.evaluate("function fullName() { return this.firstName + ' ' + this.lastName; }");
  engine.evaluate("somePerson = { firstName: 'John', lastName: 'Doe' }");

  QScriptValue global = engine.globalObject();
  QScriptValue fullName = global.property("fullName");
  QScriptValue who = global.property("somePerson");
  qDebug() << fullName.call(who).toString(); // "John Doe"

  engine.evaluate("function cube(x) { return x * x * x; }");
  QScriptValue cube = global.property("cube");
  QScriptValueList args;
  args << QScriptValue(&engine, 3);
  qDebug() << cube.call(QScriptValue(), args).toNumber(); // 27
//! [2]


//! [3]
  QScriptValue myNativeFunction(QScriptContext *ctx, QScriptEngine *)
  {
      QScriptValue otherFunction = ...;
      return otherFunction.call(ctx->thisObject(), ctx->argumentsObject());
  }
//! [3]


