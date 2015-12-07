let $docURI := 'maybeNotWellformed.xml'
return if(doc-available($docURI))
       then doc($docURI)//p/<para>{./node()}</para>
       else <para>Failed to load {$docURI}</para>
