#version 330 core

layout(location = 0) out vec4 color;
uniform vec2 u_Resolution;
uniform vec4 u_BoundingBox;
uniform float a;
uniform float b;
uniform float c;

float f(float x, float y)
{
    // != 0
    return float(EXPRESSION);
}

void main()
{
    vec2 uv = (gl_FragCoord.xy / u_Resolution) * u_BoundingBox.zw + u_BoundingBox.xy;
    vec2 size = u_BoundingBox.zw / u_Resolution;

    // .x -> x_min, .y -> y_min, .z -> x_max, .w -> y_max
    vec4 v = vec4(uv, uv + size);
    vec4 r = vec4(f(v.x, v.y), f(v.x, v.w), f(v.z, v.w), f(v.z, v.y));

    color = vec4(all(lessThan(r, vec4(0.0))) || all(greaterThan(r, vec4(0.0))));
}