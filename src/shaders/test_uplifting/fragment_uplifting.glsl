#version 450 core

in vec2 fTexcoords;

layout (location = 0) out vec4 original_tex;

layout (location = 1) out vec4 uplift_wl_0_3;
layout (location = 2) out vec4 uplift_wl_4_7;
layout (location = 3) out vec4 uplift_wl_8_11;
layout (location = 4) out vec4 uplift_wl_12_15;
layout (location = 5) out vec4 uplift_wl_16_19;
layout (location = 6) out vec4 uplift_wl_20_23;
layout (location = 7) out vec4 uplift_wl_24_27;
layout (location = 8) out vec4 uplift_wl_28_31;
layout (location = 9) out vec4 uplift_wl_32_35;
layout (location = 10) out vec4 uplift_wl_36_39;

layout (binding = 0) uniform sampler2D tex_to_uplift;
layout (binding = 1) uniform sampler3D LUT;

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

    // Perform the uplifting here    



    vec3 col = texture(tex, fTexcoords).rgb;
    //col = encode_sRGB(col);
    out_color = vec4(clamp(col, 0.0, 1.0), 1.0);
}
