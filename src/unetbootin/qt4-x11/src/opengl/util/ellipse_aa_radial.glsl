#include "ellipse_functions.glsl"

uniform sampler1D palette;
uniform vec2 fmp;
uniform float fmp2_m_radius2;
uniform vec4 inv_matrix;
uniform vec2 inv_matrix_offset;

void main()
{
    //     float2 A = frag_coord.xy;//mul(inv_matrix, frag_coord.xy) + inv_matrix_offset;
    mat2 mat;
    mat[0][0] = inv_matrix.x;
    mat[0][1] = inv_matrix.y;
    mat[1][0] = inv_matrix.z;
    mat[1][1] = inv_matrix.w;
    vec2 A = gl_FragCoord.xy * mat + inv_matrix_offset;
    vec2 B = fmp;
    float a = fmp2_m_radius2;
    float b = 2.0*dot(A, B);
    float c = -dot(A, A);
    float val = (-b + sqrt(b*b - 4.0*a*c)) / (2.0*a);
    gl_FragColor = texture1D(palette, val) * ellipse_aa();
}
