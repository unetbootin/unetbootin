declare default element namespace "http://example.com/Namespace";
declare variable $documentElementName := "doc";

element {$documentElementName}
{
    attribute xml:base {"http://example.com/"},
    element anotherElement
    {
        comment {" a comment "},
        processing-instruction target {"data"},
        element anotherElement {()},
        text {"some text"}
    }
}
