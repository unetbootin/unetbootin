#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
// #include "stones.inc"

camera {
	location        <2,2,-6>
	up              <0, 1, 0>
//	right           <4/3, 0, 0>
	look_at         <0,0,0>
}

object { light_source { <10, 5, -5>  color red 1.1 green 1.1 blue 1.0 } }

#declare Rock =
mesh {
 #include "rock.inc"   /* collection of triangle or smooth_triangle data */
}

object {
  Rock
  texture {  pigment {White}  }
  scale 1.9
  rotate <60, 45, 360*clock>
}

