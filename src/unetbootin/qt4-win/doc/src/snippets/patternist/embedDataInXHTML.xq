declare default element namespace "http://www.w3.org/1999/xhtml/";
<html>
    <body>
        {
            for $i in doc("testResult.xml")/tests/test[@status = "failure"]
            order by $i/@name
            return <p>{$i/@name}</p>
        }
    </body>
</html>
