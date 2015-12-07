declare namespace s = "http://www.w3.org/2000/svg";
declare default element namespace "http://www.w3.org/2000/svg";
let $doc := doc('image.svg')
return ($doc/svg,
        $doc/s:svg)
