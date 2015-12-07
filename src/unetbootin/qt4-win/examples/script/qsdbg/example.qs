function bar() {
  var x = 1;
  var y = 2;
  return x + y;
}

function foo(a, b, c) {
  var i = a + bar();
  var j = b - bar();
  var k = c * bar();
  return Math.cos(i) + Math.sin(j) - Math.atan(k);
}

var first = foo(1, 2, 3);
var second = foo(4, 5, 6);
print("first was:", first, ", and second was:", second);

