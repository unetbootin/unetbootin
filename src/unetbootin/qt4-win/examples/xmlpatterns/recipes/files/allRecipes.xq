(: Select all recipes. :)
declare variable $inputDocument external;

doc($inputDocument)/cookbook/recipe/<p>{string(title)}</p>
