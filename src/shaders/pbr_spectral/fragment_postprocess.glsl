#version 450 core

in vec2 fTexcoords;

out vec4 out_color;

layout (binding = 0) uniform sampler2D tex_render;
uniform bool do_spectral_uplifting;

vec3 encode_sRGB(vec3 linear_RGB)
{
    vec3 a = 12.92 * linear_RGB;
    vec3 b = 1.055 * pow(linear_RGB, vec3(1.0 / 2.4)) - 0.055;
    vec3 c = step(vec3(0.0031308), linear_RGB);

    return mix(a, b, c);
}

const float gamma = 2.2;
const float exposure = 0.5;

vec3 reinhard(vec3 v)
{
    v = pow(v, vec3(1. / gamma));
    return vec3(v / (vec3(1.0f) + v));
}

vec3 linearToneMapping(vec3 color)
{
	color = clamp(exposure * color, 0., 1.);
	color = pow(color, vec3(1. / gamma));
	return color;
}

vec3 exposureToneMapping(vec3 color)
{
    vec3 mapped = vec3(1.0) - exp(-color * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
    return mapped;
}

void main()
{
    // L=0.2125R+0.7154G+0.0721B
    vec3 col = texture(tex_render, fTexcoords).rgb;
    col = encode_sRGB(col);
    out_color = vec4(col.rgb, 1.0);
}
