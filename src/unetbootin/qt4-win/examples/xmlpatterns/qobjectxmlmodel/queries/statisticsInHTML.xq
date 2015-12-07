<html>
    <head>
        <title></title>
    </head>
    <body>
            <p>In total the tree has {count($root//QObject)} QObject instances.</p>
            <p>Order by occurrence, the QObjects are:</p>

            <ol>
            {
                for $i in $root/preceding-sibling::metaObjects/metaObject
                let $count := count($root//QObject[@className eq $i/@className])
                stable order by $count descending
                return if($count > 1)
                       then <li>{string($i/@className), $count} occurrences</li>
                       else ()
            }
            </ol>

        <h1>Properties</h1>
        {
            (: For each QObject, we create a table listing
             : the properties of that object. :)
            for $object in $root//QObject
            return (<h2>{let $name := string($object/@objectName)
                         return if(string-length($name))
                                then $name
                                else "[no name]",
                         '(', string($object/@className), ')'}</h2>,
                    <table border="1">
                        <thead>
                            <tr>
                                <td>Property Name</td>
                                <td>Value</td>
                            </tr>
                        </thead>
                        <tbody>
                        {
                            $object/@*/<tr>
                                            <td>
                                                {
                                                 name()
                                                }
                                            </td>
                                            <td>
                                                {
                                                 if(data(.))
                                                 then string(.)
                                                 else "N/A"
                                                }
                                            </td>
                                      </tr>
                        }
                        </tbody>
                   </table>)
        }
    </body>
</html>
