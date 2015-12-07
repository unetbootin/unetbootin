uniform vec2 r; // r_x and r_y

uniform sampler2D texture;
uniform vec2 inv_texture_size;

#include "ellipse_functions.glsl"

void main()
{
    gl_FragColor = ellipse_aa() * texture2D(texture, gl_FragCoord.xy * inv_texture_size);
}
