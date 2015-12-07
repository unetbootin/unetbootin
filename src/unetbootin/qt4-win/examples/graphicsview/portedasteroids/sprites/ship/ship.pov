
#version 3.0
global_settings { assumed_gamma 2.0 }

#include "colors.inc"
#include "textures.inc"
#include "metals.inc"

camera {
  orthographic
  up <0, 130, 0>
  right <130, 0, 0>
  location <0, 0, -130>
  look_at <0, 0, 0>
}

light_source { <50, 25, -25> colour White }
light_source { <0, 0, -100> colour Gray80 }

#declare ShipColor = color red 1.0 green 1.0 blue 0.9

#declare BaseTexture =
texture {
    pigment { ShipColor }
}

#declare Grubby =
texture {
    pigment {
        bozo
        color_map {
            [0.0 color rgbt <1, 1, 1, 1>]
            [0.8 color rgbt <0.9, 0.9, 0.9, 0.5>]
            [1.0 color rgbt <0.8, 0.8, 0.8, 0.5>]
        }
        turbulence 2.0
        scale 3
    }
}

#declare ShipTexture =
texture { BaseTexture }
texture { Grubby }

union {
    cone {
	<12, 0, 0>, 0.5
	<11, 0, 0>, 1.0
	texture { ShipTexture }
    }
    cone {
	<11, 0, 0>, 1.0
	<8, 0, 0>, 2.0
	texture { ShipTexture }
    }
    cone {
	<8, 0, 0>, 2.0
	<3.5, 0, 0>, 3.8
	texture { ShipTexture }
    }
    difference {
        cone {
            <8, 0, -0.01>, 2.0
            <3.5, 0, -0.01>, 3.8
            pigment { color Gray20 }
        }
        box {
            <9, -4.0, -3.7>,
            <2, 4.0, 10>
            rotate <0, -18, 0>
        }
        box {
            <6.5, -4.0, -8>,
            <10, 4.0, 8>
        }
        box {
            <2, -4.0, -8>,
            <4.5, 4.0, 8>
        }
    }
    cone {
	<3.5, 0, 0>, 3.8
	<2, 0, 0>, 4.0
	texture { ShipTexture }
    }
    cylinder {
        <2, 0, 0>,
        <-9, 0, 0>,
        4.0
        texture { ShipTexture }
    }
    cone {
	<-9, 0, 0>, 4.0
	<-10, 0, 0>, 3.5
	texture { ShipTexture }
    }
    prism {
	linear_sweep
	linear_spline
	0,
	0.5,
	4,
	<7.5, 0>, <-7.5, 10>, <-7.5, -10>, <7.5, 0>
	rotate <90, 0, 0>
	texture { T_Silver_2A }
	texture { ShipTexture }
    }
    prism {
	linear_sweep
	linear_spline
	-0.5,
	0.5,
	4,
	<4, 0>, <-7.5, 5>, <-7.5, -5>, <4, 0>
	pigment { color White }
    }
    cone {
	<-12, 0, 0>, 3.0
	<-10, 0, 0>, 2.0
	texture { T_Silver_2A }
	pigment { color Gray60 }
    }

    rotate <0, 0,-360*clock>
    scale <5, 5, 5>
}


