declare variable $myRoot := <e/>; (: This line is a dummy and shouldn't appear in the documentation. :)
<html>
    <body>
        {
            $myRoot//file[@mimetype = 'text/xml' or @mimetype = 'application/xml']
            /
            (if(doc-available(@uri))
             then ()
             else <p>Failed to parse file {@uri}.</p>)
        }
    </body>
</html>
