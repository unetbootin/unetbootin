(: All recipes with preparation times. :)
declare variable $inputDocument external;

doc($inputDocument)/cookbook/recipe/
<recipe title="{title}" time="{time/@quantity} {time/@unit}"/>
