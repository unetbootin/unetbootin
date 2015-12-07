(: All ingredients for Mushroom Soup. :)
declare variable $inputDocument external;

doc($inputDocument)/cookbook/recipe[@xml:id = "MushroomSoup"]/ingredient/
<p>{@name, @quantity}</p>
