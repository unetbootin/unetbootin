(: 
    This query opens a C++ GCC-XML output file, and outputs a report describing the use
    of globals variables.

    Run it, by invoking:

    xmlpatterns reportGlobals.xq -param fileToOpen=globals.gccxml -output globals.html

    "fileToOpen=globals.gccxml" binds the string "globals.gccxml" to the variable
    "fileToOpen." It identifies the GCC-XML file to open.

:)
declare variable $fileToOpen as xs:anyURI external;
declare variable $inDoc as document-node() := doc($fileToOpen);

(: Determines whether the type by ID @p typeId is a complex type such as QString. :)
declare function local:isComplexType($typeID as xs:string) as xs:boolean
{
    (: We're being a bit crude here and only checks whether it's a class. We
       actually should check whether it has non-synthesized,
        constructors, I believe. :)
    
    exists($inDoc/GCC_XML/Class[@id = $typeID])
    or
    (: We also want const-qualified variables. :)
    exists($inDoc/GCC_XML/Class[@id = $inDoc/GCC_XML/CvQualifiedType[@id = $typeID]/@type])
};

declare function local:isPrimitive($typeId as xs:string) as xs:boolean
{
    exists($inDoc/GCC_XML/FundamentalType[@id = $typeId])
};

(: Returns a string for human consumption that describes
   the location of @p block. :)
declare function local:location($block as element()) as xs:string
{
    concat($inDoc/GCC_XML/File[@id = $block/@file]/@name, " at line ", $block/@line)
};

declare function local:report() as element()+
{
            let $complexVariables as element(Variable)* := $inDoc/GCC_XML/Variable[local:isComplexType(@type)]
            return if(exists($complexVariables)) (: Is the length larger than zero? :)

                   then (<p xmlns="http://www.w3.org/1999/xhtml/">The following global, complex variables were found:</p>,
                         <ol xmlns="http://www.w3.org/1999/xhtml/">
                            {
                                (: For each Variable in $complexVariables... :)
                                $complexVariables/<li><span class="variableName">{string(@name)}</span> in {local:location(.)}</li>
                            }
                         </ol>)

                   else <p xmlns="http://www.w3.org/1999/xhtml/">No global variables that are of complex types were found.</p>

            ,

            (: List primitive, mutable types. :)
            let $primitiveVariables as element(Variable)+ := $inDoc/GCC_XML/Variable[local:isPrimitive(@type)]
            return if(exists($primitiveVariables))

                   then (<p xmlns="http://www.w3.org/1999/xhtml/">The following mutable primitives were found:</p>,
                         <ol xmlns="http://www.w3.org/1999/xhtml/">
                            {
                                (: For each Variable in $complexVariables... :)
                                $primitiveVariables/<li><span class="variableName">{string(@name)}</span> in {local:location(.)}</li>
                            }
                         </ol>)

                   else <p xmlns="http://www.w3.org/1999/xhtml/">No global variables that are mutable primitives were found.</p>
};

<html xmlns="http://www.w3.org/1999/xhtml/" xml:lang="en" lang="en">
    <head>
        <title>Global variables report for {$fileToOpen}</title>
    </head>
    <style type="text/css">
        .details
        {{
            text-align: center;
            font-size: 80%;
            color: gray
        }}
        .variableName
        {{
            font-family: courier
        }}
    </style>

    <body>
        {
            (: We don't want simple types that are const, but all other types.
               One could frown upon const integers and say enums should be used instead, but
               let's be gentle. :)

            local:report()
        }

        <p class="details">This report was generated on {current-dateTime()}</p>
    </body>

</html>
