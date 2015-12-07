(: All recipes taking 10 minutes or less to prepare. :)
declare variable $inputDocument external;

doc($inputDocument)/cookbook/recipe/time[@unit = "minutes" and xs:integer(@quantity) <= 10]/
<p>
    {
       concat(../title, ' (', @quantity, ' ', @unit, ')')
    }
</p>
