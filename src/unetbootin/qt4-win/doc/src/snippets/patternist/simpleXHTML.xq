declare namespace x = "http://www.w3.org/1999/xhtml/";
(: Select all paragraphs that contains examples. :)
doc("index.html")/x:html/x:body/x:p[@class="example"]
