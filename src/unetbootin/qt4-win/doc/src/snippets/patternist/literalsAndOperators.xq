declare variable $date := fn:current-date(); (: This line should not be part of the example. It exists in order to make the query valid. :)
fn:current-date() - $date > xs:dayTimeDuration("P365D")
