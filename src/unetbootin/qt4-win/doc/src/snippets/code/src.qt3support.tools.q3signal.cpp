//! [0]
	#include <q3signal.h>

	class MyClass
	{
	public:
	    MyClass();
	    ~MyClass();

	    void doSomething();

	    void connect(QObject *receiver, const char *member);

	private:
	    Q3Signal *sig;
	};

	MyClass::MyClass()
	{
	    sig = new Q3Signal;
	}

	MyClass::~MyClass()
	{
	    delete sig;
	}

	void MyClass::doSomething()
	{
	    // ... does something
	    sig->activate(); // emits the signal
	}

	void MyClass::connect(QObject *receiver, const char *member)
	{
	    sig->connect(receiver, member);
	}
//! [0]


