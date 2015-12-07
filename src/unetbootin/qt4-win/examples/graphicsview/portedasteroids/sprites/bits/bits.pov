
#version 3.0
global_settings { assumed_gamma 2.0 }

#include "colors.inc"
#include "textures.inc"
#include "metals.inc"

camera {
  location <15, -15, -100>
  look_at <0, 0, 0>
}

light_source { <50, 50, -50> colour White }
light_source { <0, 0, -50> colour White }

prism {
    linear_sweep
    linear_spline
    0,
    0.2,
    5,
    <2, 0>, <0, 2>, <-1, 1>, <0, -3>, <2, 0>
    texture { T_Silver_2A }

    rotate <360*clock, 50, 30>
    scale <20, 20, 20>
}



