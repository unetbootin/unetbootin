(: All liquid ingredients form Mushroom Soup. :)
declare variable $inputDocument external;

doc($inputDocument)/cookbook/recipe[@xml:id = "MushroomSoup"]/ingredient[@unit = "milliliters"]/
<p>{@name, @quantity, @unit}</p>
