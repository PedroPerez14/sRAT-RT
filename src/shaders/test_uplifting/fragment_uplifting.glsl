#version 450 core

in vec2 fTexcoords;

layout (location = 0) out vec4 original_tex;

layout (location = 1) out vec4 uplift_wl_0_3;
layout (location = 2) out vec4 uplift_wl_4_7;
layout (location = 3) out vec4 uplift_wl_8_11;

layout (binding = 0) uniform sampler2D tex_to_uplift;
layout (binding = 1) uniform sampler3D LUT_1;
layout (binding = 2) uniform sampler3D LUT_2;
layout (binding = 3) uniform sampler3D LUT_3;

uniform int n_wls;
// uniform float wls[40];

vec3 encode_sRGB(vec3 linear_RGB)
{
    vec3 a = 12.92 * linear_RGB;
    vec3 b = 1.055 * pow(linear_RGB, vec3(1.0 / 2.4)) - 0.055;
    vec3 c = step(vec3(0.0031308), linear_RGB);

    return mix(a, b, c);
}

float S(vec3 coeffs, float wl)
{
    float c0 = coeffs.x;
    float c1 = coeffs.y;
    float c2 = coeffs.z;

    float x = fma(fma(c0, wl, c1), wl, c2);
    return fma(.5 * x, inversesqrt(fma(x, x, 1)), .5);
}

// a translation attempt from wenzel jakob's rgb2spec code to glsl
vec3 fetch_coeffs_from_lut(vec3 rgb)
{
    // determine biggest component of our rgb color:
    int i = 0;                          // The index of the biggest color, also the lut to be consulted
    int res = textureSize(LUT_1, 0).x;  // Any channel of any of the 3 luts should be the same
    for(int j=1; j < 3; ++j)
    {
        if(rgb[j] >= rgb[i])
        {
            i = j;
        }
    }
    float z = rgb[i];
    float x = rgb[(i + 1) % 3] / z;
    float y = rgb[(i + 2) % 3] / z;

    // Sample the 3d texture, trilinear interpolation will be done automatically
    // since we have set GL_LINEAR when creating the 3d textures
    vec3 coeffs;
    vec3 uv_3d = vec3(x,y,z);
    if(i == 0)
    {
        // Get coeffs from the first texture
        coeffs = texture(LUT_1, uv_3d).rgb;
    }
    else if (i == 1)
    {
        // Get coeffs from the second texture
        coeffs = texture(LUT_2, uv_3d).rgb;
    }
    else if (i == 2)
    {
        // Get coeffs from the third texture
        coeffs = texture(LUT_3, uv_3d).rgb;
    }
    return coeffs;
}

void main()
{
    float _WAVELENGTH = 532.0;

    // Perform the uplifting here    
    vec3 color_rgb = texture(tex_to_uplift, fTexcoords).rgb;
    // TODO: fetch the coefficients in the texture, based on the colour we have
    // Then do the sigmoid function and evaluate it for the corresponding wavelength

    vec3 coeffs = fetch_coeffs_from_lut(color_rgb);
    
    for(int i = 0; i < min(n_wls, 12); i++)
    {
        if(i <4)
            uplift_wl_0_3[i % 4] = S(coeffs, _WAVELENGTH);
        else if(i <8)
            uplift_wl_4_7[i % 4] = S(coeffs, _WAVELENGTH);
        else if(i <12)
            uplift_wl_8_11[i% 4] = S(coeffs, _WAVELENGTH);
    }

    original_tex = vec4(color_rgb, 1.0);
}
