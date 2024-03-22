#version 450 core

in vec2 fTexcoords;

out vec4 out_color;

layout (binding = 0) uniform sampler2D tex;
layout (binding = 1) uniform sampler2D uplift_wl_0_3;
layout (binding = 2) uniform sampler2D uplift_wl_4_7;
layout (binding = 3) uniform sampler2D uplift_wl_8_11;
uniform int show_original_tex_or_spec2rgb = 0;  // 0 = original tex, 1 = wl_tex to rgb

vec3 encode_sRGB(vec3 linear_RGB)
{
    vec3 a = 12.92 * linear_RGB;
    vec3 b = 1.055 * pow(linear_RGB, vec3(1.0 / 2.4)) - 0.055;
    vec3 c = step(vec3(0.0031308), linear_RGB);

    return mix(a, b, c);
}

void main()
{
    if(show_original_tex_or_spec2rgb == 0)
    {
        vec3 col = texture(tex, fTexcoords).rgb;
        //col = encode_sRGB(col);
        out_color = vec4(clamp(col, 0.0, 1.0), 1.0);
    }
    else
    {
        /// TODO: Do the spectral to rgb part, use a standard response curve like CIE1932
        /// TODO TODO: Allow for users to choose the response curve they want?
        vec3 col = texture(uplift_wl_0_3, fTexcoords).rgb;
        //col = encode_sRGB(col);
        out_color = vec4(clamp(col, 0.0, 1.0), 1.0);
    }
}
