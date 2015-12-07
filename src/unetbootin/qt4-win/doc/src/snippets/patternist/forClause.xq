for $i in(reverse(1 to 10)),
    $d in xs:integer(doc("numbers.xml")/numbers/number)
return ($i + $d)
