//! [0]
        QDBusInterface remoteApp( "com.example.Calculator", "/Calculator/Operations",
                                  "org.mathematics.RPNCalculator" );
        remoteApp.call( "PushOperand", 2 );
        remoteApp.call( "PushOperand", 2 );
        remoteApp.call( "ExecuteOperation", "+" );
        QDBusReply<int> reply = remoteApp.call( "PopOperand" );

        if ( reply.isValid() )
            printf( "%d", reply.value() );          // prints 4
//! [0]


