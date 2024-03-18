#version 450 core

in vec2 fTexcoords;

out vec4 out_color;

uniform sampler2D tex;
uniform float exposure;

vec3 encode_sRGB(vec3 linear_RGB)
{
    vec3 a = 12.92 * linear_RGB;
    vec3 b = 1.055 * pow(linear_RGB, vec3(1.0 / 2.4)) - 0.055;
    vec3 c = step(vec3(0.0031308), linear_RGB);

    return mix(a, b, c);
}

void main()
{
    vec3 col = texture(tex, fTexcoords).rgb;
    //col = encode_sRGB(col);
    out_color = vec4(clamp(col, 0.0, 1.0), 1.0);
}
